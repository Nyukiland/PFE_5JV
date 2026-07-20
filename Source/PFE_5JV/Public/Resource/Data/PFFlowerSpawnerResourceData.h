// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFFlowerSpawnerResourceData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFFlowerSpawnerResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
	meta=(ToolTip="Distance maximal de spawn par rapport à l'oiseau"))
	float DelayBetweenTwoSpawnsAtMinimalVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
	meta=(ToolTip="Nombre de secondes entre 2 spawns au minimum"))
	float MaximalSpawnDistanceFromBird;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
	meta=(ToolTip="Nombre de secondes entre 2 spawns au maximum"))
	float DelayBetweenTwoSpawnsAtMaximalVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
    meta=(ToolTip="La taille minimum possible pour une fleur"))
    float MinimumFlowerScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
	meta=(ToolTip="La taille maximum possible pour une fleur"))
	float MaximumFlowerScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
	meta=(ToolTip="La pente maximale (en degrés) sur laquelle on spawn les fleurs"))
	float MaximalSlopInDegreesToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
	meta=(ToolTip="La hauteur minimale au dessus du sol où spawn la fleur."))
	float MinimalHeightAboveGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerSpawner",
	meta=(ToolTip="La hauteur maximale au dessus du sol où spawn la fleur."))
	float MaximalHeightAboveGround;

};
