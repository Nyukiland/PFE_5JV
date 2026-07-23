#include "PFKuwaharaViewExtension.h"
#include "PFKuwaharaPS.h"
#include "SystemTextures.h"
#include "RenderTargetPool.h"
#include "RendererInterface.h"
#include "TextureResource.h"
#include "GlobalShader.h"
#include "PFKuwaharaSettings.h"
#include "PostProcess/PostProcessMaterialInputs.h"
#include "ScreenPass.h"

PFKuwaharaViewExtension::PFKuwaharaViewExtension(const FAutoRegister& AutoRegister, FTextureRHIRef InJitterTexture) 
	: FSceneViewExtensionBase(AutoRegister), JitterTextureRHI(InJitterTexture)
{
}

void PFKuwaharaViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass PassId, const FSceneView& InView, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
    bool bShouldRender = InView.bIsGameView || (InView.UnscaledViewRect.Width() > 512);
    if (!bShouldRender)
    {
        return;
    }
    
    // Cache settings here on the game thread when subscribing per view
    const UPFKuwaharaSettings* Settings = GetDefault<UPFKuwaharaSettings>();
    CachedMaxRadius = Settings->MaxRadius;
    CachedEdgeSensitivity = Settings->EdgeSensitivity;
    
	// Inject before tonemap
	if (PassId == EPostProcessingPass::Tonemap)
	{
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &PFKuwaharaViewExtension::PostProcessPass_RenderThread));
	}
}

FScreenPassTexture PFKuwaharaViewExtension::PostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs)
{
    check(Inputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor].IsValid());
    
    FScreenPassTexture SceneColor(Inputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor]);
    FIntPoint FullRes = SceneColor.Texture->Desc.Extent;
    
    // 1. Calculate Target Resolution (Locking height to 720p for performance)
    float AspectRatio = (float)FullRes.X / (float)FullRes.Y;
    FIntPoint LowRes(FMath::RoundToInt(720.0f * AspectRatio), 720);
    FIntRect LowResRect(FIntPoint::ZeroValue, LowRes);

    // 2. Allocate Downscaled Buffer (Input for Kuwahara)
    FRDGTextureDesc LowResDesc = FRDGTextureDesc::Create2D(
        LowRes, 
        SceneColor.Texture->Desc.Format, 
        FClearValueBinding::Black, 
        TexCreate_RenderTargetable | TexCreate_ShaderResource
    );
    FRDGTextureRef LowResInputTex = GraphBuilder.CreateTexture(LowResDesc, TEXT("Kuwahara_LowResInput"));
    
    // Downscale SceneColor to LowResInputTex using the active ViewRect offset and size
    AddDrawTexturePass(
        GraphBuilder, 
        View, 
        SceneColor.Texture, 
        LowResInputTex, 
        SceneColor.ViewRect.Min, 
        FIntPoint::ZeroValue, 
        SceneColor.ViewRect.Size(), 
        LowRes
    );

    // 3. Allocate Kuwahara Output Buffer
    FRDGTextureRef LowResOutputTex = GraphBuilder.CreateTexture(LowResDesc, TEXT("Kuwahara_LowResOutput"));

    // 4. Setup Shader Parameters
    PFKuwaharaPS::FParameters* PassParameters = GraphBuilder.AllocParameters<PFKuwaharaPS::FParameters>();
    
    PassParameters->InputTexture = LowResInputTex;
    PassParameters->InputSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
    
    PassParameters->JitterTexture = JitterTextureRHI.IsValid() ? JitterTextureRHI : GBlackTexture->TextureRHI;
    PassParameters->JitterSampler = TStaticSamplerState<SF_Point, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI();
    
    PassParameters->InvSize = FVector2f(1.0f / LowRes.X, 1.0f / LowRes.Y);
    PassParameters->MaxRadius = CachedMaxRadius;
    PassParameters->EdgeSensitivity = CachedEdgeSensitivity;
    
    PassParameters->RenderTargets[0] = FRenderTargetBinding(LowResOutputTex, ERenderTargetLoadAction::ENoAction);

    // 5. Dispatch Kuwahara
    TShaderMapRef<PFKuwaharaPS> PixelShader(GetGlobalShaderMap(View.GetFeatureLevel()));
    FScreenPassTextureViewport LowResViewport(LowResInputTex, LowResRect);
    
    AddDrawScreenPass(
        GraphBuilder, 
        RDG_EVENT_NAME("Kuwahara_Process"), 
        View, 
        LowResViewport, 
        LowResViewport, 
        PixelShader, 
        PassParameters
    );

    // 6. Allocate FullRes Output Buffer for Upscale
    FRDGTextureDesc FullResDesc = SceneColor.Texture->Desc;
    FRDGTextureRef FullResOutputTex = GraphBuilder.CreateTexture(FullResDesc, TEXT("Kuwahara_FullResOutput"));

    // Upscale LowResOutput back to FullRes output, restoring the original ViewRect placement
    AddDrawTexturePass(
        GraphBuilder, 
        View, 
        LowResOutputTex, 
        FullResOutputTex, 
        FIntPoint::ZeroValue, 
        SceneColor.ViewRect.Min, 
        LowRes, 
        SceneColor.ViewRect.Size()
    );

    // 7. Explicitly construct returning texture and assign the original ViewRect
    FScreenPassTexture ReturnTexture(FullResOutputTex, SceneColor.ViewRect);
    
    return ReturnTexture; 
}