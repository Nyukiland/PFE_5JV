// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFFlowerSpawnerResourceData.generated.h"

/**
 * 
 */
UCLASS()
class PFE_5JV_API UPFFlowerSpawnerResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner",
	meta=(ToolTip="Evolution de la durée entre 2 spawns en fonction de la vélocité"))
	TObjectPtr<UCurveFloat> SpawnDelayBasedOnVelocityCurvePtr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
	meta=(ToolTip="Nombre de secondes entre 2 spawns au minimum"))
	float DelayBetweenTwoSpawnsAtMinimalVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
	meta=(ToolTip="Nombre de secondes entre 2 spawns au maximum"))
	float DelayBetweenTwoSpawnsAtMaximalVelocity;
	
};
