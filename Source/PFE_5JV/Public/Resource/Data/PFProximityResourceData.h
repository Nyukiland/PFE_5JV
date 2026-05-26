#pragma once
#include "PFProximityResourceData.generated.h"

// Struct sérialisée pour la détection sphérique (OverlapMultiByChannel)
USTRUCT(BlueprintType)
struct FProximitySweepConfig
{
	GENERATED_BODY()

	// Rayon de la sphère de détection autour du joueur
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity Sweep")
	float DetectionRadius = 300.f;

	// Canal de collision utilisé pour le sweep
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity Sweep")
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_WorldStatic;

	// Dessine la sphère de debug en jeu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity Sweep")
	bool bDrawDebug = false;

	// Durée d'affichage du debug (secondes)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity Sweep")
	float DebugDrawDuration = 0.1f;
};

// Struct sérialisée pour la détection de distance exacte (OverlapAnyTestByChannel)
USTRUCT(BlueprintType)
struct FClosestObstacleConfig
{
	GENERATED_BODY()

	// Rayon initial de la sphère (point de départ du shrink)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Closest Obstacle")
	float InitialRadius = 300.f;

	// Rayon minimum en dessous duquel on arrête de chercher
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Closest Obstacle")
	float MinRadius = 10.f;

	// Pas de réduction du rayon à chaque itération
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Closest Obstacle")
	float RadiusStep = 10.f;

	// Canal de collision utilisé
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Closest Obstacle")
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_WorldStatic;

	// Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Closest Obstacle")
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Closest Obstacle")
	float DebugDrawDuration = 0.1f;
};

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFProximityResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	float BrushSphereDistance = 4000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	FVector2D BrushSizesMinMax = FVector2D(50, 150);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	float BrushForwardOffset = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	FProximitySweepConfig ProximitySweepConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	FClosestObstacleConfig ClosestObstacleConfig;
};
