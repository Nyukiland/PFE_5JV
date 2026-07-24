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
    const UPFKuwaharaSettings* Settings = GetDefault<UPFKuwaharaSettings>();
    
    if (!InView.bIsGameView && !Settings->bEnableKuwahara)
    {
        return;
    }
    
    bool bShouldRender = InView.bIsGameView || (InView.UnscaledViewRect.Width() > 512);
    if (!bShouldRender)
    {
        return;
    }
    
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
    
    // 1. Calculate Target Resolution (Locking height to 720p for performance)
    FIntPoint FullResSize = SceneColor.ViewRect.Size();
    float AspectRatio = (float)FullResSize.X / (float)FullResSize.Y;
    FIntPoint LowRes(FMath::RoundToInt(720.0f * AspectRatio), 720);

    // 2. Allocate Downscaled Buffers
    FRDGTextureDesc LowResDesc = FRDGTextureDesc::Create2D(
        LowRes, 
        SceneColor.Texture->Desc.Format, 
        FClearValueBinding::Black, 
        TexCreate_RenderTargetable | TexCreate_ShaderResource
    );
    
    FRDGTextureRef LowResInputTex = GraphBuilder.CreateTexture(LowResDesc, TEXT("Kuwahara_LowResInput"));
    FRDGTextureRef LowResOutputTex = GraphBuilder.CreateTexture(LowResDesc, TEXT("Kuwahara_LowResOutput"));

    // Ensure our ViewRects start exactly at 0,0 within the pooled textures
    FScreenPassTexture LowResInput(LowResInputTex, FIntRect(FIntPoint::ZeroValue, LowRes));
    FScreenPassTexture LowResOutput(LowResOutputTex, FIntRect(FIntPoint::ZeroValue, LowRes));

    // Downscale SceneColor to LowResInput automatically handling viewrects
    AddDrawTexturePass(GraphBuilder, View, SceneColor, LowResInput);

    // 3. Setup Shader Parameters
    PFKuwaharaPS::FParameters* PassParameters = GraphBuilder.AllocParameters<PFKuwaharaPS::FParameters>();
    
    PassParameters->InputTexture = LowResInput.Texture;
    PassParameters->InputSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
    
    PassParameters->JitterTexture = JitterTextureRHI.IsValid() ? JitterTextureRHI : GBlackTexture->TextureRHI;
    PassParameters->JitterSampler = TStaticSamplerState<SF_Point, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI();
    
    // --- THE FIX: Calculate physical pooled texture transforms ---
    FIntPoint PhysicalExtent = LowResInput.Texture->Desc.Extent;
    
    PassParameters->InvSize = FVector2f(1.0f / PhysicalExtent.X, 1.0f / PhysicalExtent.Y);

    PassParameters->MaxRadius = CachedMaxRadius;
    PassParameters->EdgeSensitivity = CachedEdgeSensitivity;
    
    // FIX 1: Manually bind the underlying texture rather than calling a non-existent method
    PassParameters->RenderTargets[0] = FRenderTargetBinding(LowResOutput.Texture, ERenderTargetLoadAction::ENoAction);

    // 4. Dispatch Kuwahara
    TShaderMapRef<PFKuwaharaPS> PixelShader(GetGlobalShaderMap(View.GetFeatureLevel()));
    
    // FIX 2: Convert the FScreenPassTextures into FScreenPassTextureViewports
    FScreenPassTextureViewport OutputViewport(LowResOutput);
    FScreenPassTextureViewport InputViewport(LowResInput);
    
    AddDrawScreenPass(
        GraphBuilder, 
        RDG_EVENT_NAME("Kuwahara_Process"), 
        View, 
        OutputViewport, 
        InputViewport, 
        PixelShader, 
        PassParameters
    );

    // 5. Allocate FullRes Output Buffer for Upscale
    FRDGTextureDesc FullResDesc = SceneColor.Texture->Desc;
    FRDGTextureRef FullResOutputTex = GraphBuilder.CreateTexture(FullResDesc, TEXT("Kuwahara_FullResOutput"));
    FScreenPassTexture FullResOutput(FullResOutputTex, SceneColor.ViewRect);

    // Upscale LowResOutput back to FullRes output
    AddDrawTexturePass(GraphBuilder, View, LowResOutput, FullResOutput);

    return FullResOutput; 
}