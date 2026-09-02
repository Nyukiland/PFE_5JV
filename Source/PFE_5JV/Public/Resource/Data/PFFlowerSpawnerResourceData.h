// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFFlowerSpawnerResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFFlowerSpawnerResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General",
	meta=(ToolTip="Distance maximal de spawn par rapport à l'oiseau"))
	float MaximalSpawnDistanceFromBird;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General",
	meta=(ToolTip="La pente maximale (en degrés) sur laquelle on spawn les fleurs"))
	float MaximalSlopInDegreesToSpawn;

	// VITESSE DE SPAWN DES PLANTES :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Speed|Flower",
	meta=(ToolTip="Nombre de secondes entre 2 spawns de fleurs, au minimum"))
	float DelayBetweenTwoFlowerSpawnsAtMinimalVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Speed|Flower",
	meta=(ToolTip="Nombre de secondes entre 2 spawns de fleurs, au maximum"))
	float DelayBetweenTwoFlowerSpawnsAtMaximalVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Speed|Waterlily",
	meta=(ToolTip="Nombre de secondes entre 2 spawns de nénuphars, au minimum"))
	float DelayBetweenTwoWaterLilySpawnsAtMinimalVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Speed|Waterlily",
	meta=(ToolTip="Nombre de secondes entre 2 spawns de nénuphars, au maximum"))
	float DelayBetweenTwoWaterLilySpawnsAtMaximalVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Speed|Ivy",
	meta=(ToolTip="Nombre de secondes entre 2 spawns de lierre, au minimum"))
	float DelayBetweenTwoIvySpawnsAtMinimalVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Speed|Ivy",
	meta=(ToolTip="Nombre de secondes entre 2 spawns de lierre, au maximum"))
	float DelayBetweenTwoIvySpawnsAtMaximalVelocity;
	
	// TAILLLE DES PLANTES :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Size|Flower",
    meta=(ToolTip="La taille minimum possible pour une fleur"))
    float MinimumFlowerScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Size|Flower",
	meta=(ToolTip="La taille maximum possible pour une fleur"))
	float MaximumFlowerScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Size|Waterlily",
	meta=(ToolTip="La taille minimum possible pour un nénuphar"))
	float MinimumWaterLilyScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Size|Waterlily",
	meta=(ToolTip="La taille maximum possible pour un nénuphar"))
	float MaximumWaterLilyScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Size|Ivy",
	meta=(ToolTip="La taille minimum possible pour le lierre"))
	float MinimumIvyScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Size|Ivy",
	meta=(ToolTip="La taille maximum possible pour le lierre"))
	float MaximumIvyScale;
	
	// HAUTEUR DE SPAWN DES PLANTES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Height Above Ground|Flower",
	meta=(ToolTip="La hauteur minimale au dessus du sol où spawn la fleur."))
	float MinimalHeightAboveGroundForFlower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Height Above Ground|Flower",
	meta=(ToolTip="La hauteur maximale au dessus du sol où spawn la fleur."))
	float MaximalHeightAboveGroundForFlower;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Height Above Ground|Waterlily",
	meta=(ToolTip="La hauteur minimale au dessus du sol où spawn le nénuphar."))
	float MinimalHeightAboveGroundForWaterLily;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Height Above Ground|Waterlily",
	meta=(ToolTip="La hauteur maximale au dessus du sol où spawn le nénuphar."))
	float MaximalHeightAboveGroundForWaterLily;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Height Above Ground|Ivy",
	meta=(ToolTip="La hauteur minimale au dessus du sol où spawn le lierre."))
	float MinimalHeightAboveGroundForIvy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Height Above Ground|Ivy",
	meta=(ToolTip="La hauteur maximale au dessus du sol où spawn le lierre."))
	float MaximalHeightAboveGroundForIvy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Speed|Flower",
	meta=(ToolTip="Vitesse de croissance des fleurs. Ne peut pas être inférieur à 0.1f"))
	float GrowthSpeedForFlower;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Speed|Waterlily",
	meta=(ToolTip="Vitesse de croissance des nénuphars. Ne peut pas être inférieur à 0.1f"))
	float GrowthSpeedForWaterLily;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth Speed|Ivy",
	meta=(ToolTip="Vitesse de croissance du lierre. Ne peut pas être inférieur à 0.1f"))
	float GrowthSpeedForIvy;
	
	
};
