#include "PFKuwaharaViewExtension.h"
#include "PFKuwaharaPS.h"
#include "SystemTextures.h"
#include "RendererInterface.h"
#include "TextureResource.h"
#include "GlobalShader.h"
#include "PFKuwaharaSettings.h"
#include "Engine/Texture2D.h"
#include "PostProcess/PostProcessMaterialInputs.h"
#include "ScreenPass.h"

PFKuwaharaViewExtension::PFKuwaharaViewExtension(const FAutoRegister& AutoRegister, UTexture2D* InJitterTexture)
	: FSceneViewExtensionBase(AutoRegister)
	, JitterTextureAsset(InJitterTexture)
{
}

void PFKuwaharaViewExtension::PreRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily)
{
	if (UTexture2D* Tex = JitterTextureAsset.Get())
	{
		if (FTextureResource* Resource = Tex->GetResource())
		{
			JitterTextureRHI = Resource->GetTexture2DRHI();
		}
	}
}

void PFKuwaharaViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass PassId, const FSceneView& InView, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	const UPFKuwaharaSettings* Settings = GetDefault<UPFKuwaharaSettings>();
	if (!Settings || !Settings->bEnableKuwahara)
	{
		return;
	}

	const bool bShouldRender = InView.bIsGameView || (InView.UnscaledViewRect.Width() > 512);
	if (!bShouldRender)
	{
		return;
	}

	CachedMaxRadius = Settings->MaxRadius;
	CachedEdgeSensitivity = Settings->EdgeSensitivity;

	if (PassId == EPostProcessingPass::Tonemap)
	{
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &PFKuwaharaViewExtension::PostProcessPass_RenderThread));
	}
}

FScreenPassTexture PFKuwaharaViewExtension::PostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs)
{
	FScreenPassTexture SceneColor(Inputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor]);
	check(SceneColor.IsValid());

	const FIntPoint ViewSize = SceneColor.ViewRect.Size();
	if (ViewSize.X <= 0 || ViewSize.Y <= 0)
	{
		return SceneColor;
	}

	FRDGTextureDesc OutputDesc = FRDGTextureDesc::Create2D(
		ViewSize,
		SceneColor.Texture->Desc.Format,
		FClearValueBinding::Black,
		TexCreate_RenderTargetable | TexCreate_ShaderResource
	);

	FRDGTextureRef OutputTex = GraphBuilder.CreateTexture(OutputDesc, TEXT("Kuwahara_Output"));
	FScreenPassTexture Output(OutputTex, FIntRect(FIntPoint::ZeroValue, ViewSize));

	PFKuwaharaPS::FParameters* PassParameters = GraphBuilder.AllocParameters<PFKuwaharaPS::FParameters>();
	PassParameters->InputTexture = SceneColor.Texture;
	PassParameters->InputSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	PassParameters->JitterTexture = JitterTextureRHI.IsValid() ? JitterTextureRHI : GBlackTexture->TextureRHI;
	PassParameters->JitterSampler = TStaticSamplerState<SF_Point, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI();

	const FIntPoint PhysicalExtent = SceneColor.Texture->Desc.Extent;
	PassParameters->InvSize = FVector2f(1.0f / PhysicalExtent.X, 1.0f / PhysicalExtent.Y);
	PassParameters->MaxRadius = CachedMaxRadius;
	PassParameters->EdgeSensitivity = CachedEdgeSensitivity;
	PassParameters->RenderTargets[0] = FRenderTargetBinding(Output.Texture, ERenderTargetLoadAction::ENoAction);

	TShaderMapRef<PFKuwaharaPS> PixelShader(GetGlobalShaderMap(View.GetFeatureLevel()));
	AddDrawScreenPass(
		GraphBuilder,
		RDG_EVENT_NAME("Kuwahara_Process"),
		View,
		FScreenPassTextureViewport(Output),
		FScreenPassTextureViewport(SceneColor),
		PixelShader,
		PassParameters
	);

	return Output;
}