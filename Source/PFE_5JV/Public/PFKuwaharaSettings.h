#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PFKuwaharaSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Kuwahara Renderer"))
class PFE_5JV_API UPFKuwaharaSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Shader Parameters")
	float MaxRadius = 10.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Shader Parameters")
	float EdgeSensitivity = 1000.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Resources", meta=(AllowedClasses="/Script/Engine.Texture2D"))
	TSoftObjectPtr<UTexture2D> BlueNoiseTexture;
};
