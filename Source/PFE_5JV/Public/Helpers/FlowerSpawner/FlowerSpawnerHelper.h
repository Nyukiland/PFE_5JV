// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlowerSpawnerHelper.generated.h"


UCLASS()
class PFE_5JV_API UFlowerSpawnerHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "FlowerSpawner")
	static bool TryGetFlowerColorFromEnum(const EPFFlowerColor FlowerColor, FLinearColor& ColorValue);
	
	UFUNCTION(BlueprintPure, Category = "FlowerSpawner")
	static float GetCustomDataAlphaFromEnum(const EPFCustomDataVersion Version);
	
	UFUNCTION(BlueprintPure, Category = "FlowerSpawner")
	static FString GetFlowerEnvironmentNameFromEnum(const EPFFlowerEnvironment Environment);
};
