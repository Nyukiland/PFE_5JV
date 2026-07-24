#pragma once
#include "SceneViewExtension.h"

class PFKuwaharaViewExtension : public FSceneViewExtensionBase
{
private:
	FTextureRHIRef JitterTextureRHI;
	float CachedMaxRadius = 10.0f;
	float CachedEdgeSensitivity = 1000.0f;
	
public:
	PFKuwaharaViewExtension(const FAutoRegister& AutoRegister, FTextureRHIRef InJitterTexture);

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) override {}

	virtual void SubscribeToPostProcessingPass(EPostProcessingPass PassId, const FSceneView& InView, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;

private:
	FScreenPassTexture PostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
};