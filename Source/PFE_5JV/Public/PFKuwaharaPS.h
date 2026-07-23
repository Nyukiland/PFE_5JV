#pragma once
#include "GlobalShader.h"
#include "ScreenPass.h"

class PFKuwaharaPS : public FGlobalShader
{
	// Global shader declaration
	DECLARE_GLOBAL_SHADER(PFKuwaharaPS);
	SHADER_USE_PARAMETER_STRUCT(PFKuwaharaPS, FGlobalShader);

	// Binds C++ variables to usf
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputSampler)
		SHADER_PARAMETER_TEXTURE(Texture2D, JitterTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, JitterSampler)
    
		SHADER_PARAMETER(FVector2f, InvSize)
		SHADER_PARAMETER(float, MaxRadius)
		SHADER_PARAMETER(float, EdgeSensitivity)
    
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};