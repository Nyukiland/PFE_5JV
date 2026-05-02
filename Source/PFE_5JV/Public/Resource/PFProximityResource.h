#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFProximityResource.generated.h"

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

// Struct sérialisée pour chaque direction de rayon (LineTrace vers le sol / obstacles directionnels)
USTRUCT(BlueprintType)
struct FDirectionalTraceConfig
{
    GENERATED_BODY()

    // Direction locale du rayon (ex: Down = (0,0,-1), Forward = (1,0,0), etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Trace")
    FVector LocalDirection = FVector(0.f, 0.f, -1.f);

    // Longueur maximale du rayon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Trace")
    float TraceLength = 1000.f;

    // Canal de collision utilisé
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Trace")
    TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_WorldStatic;

    // Nom lisible pour identifier la direction (ex: "Down", "Forward", "Left")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Trace")
    FString DirectionLabel = TEXT("Down");

    // Debug
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Trace")
    bool bDrawDebug = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Trace")
    float DebugDrawDuration = 0.1f;
};

// Struct de résultat pour un rayon directionnel
USTRUCT(BlueprintType)
struct FDirectionalTraceResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bHit = false;

    UPROPERTY(BlueprintReadOnly)
    float Distance = 0.f;

    UPROPERTY(BlueprintReadOnly)
    FVector ImpactPoint = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    FVector ImpactNormal = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    AActor* HitActor = nullptr;
};

UCLASS()
class PFE_5JV_API UPFProximityResource : public UPFResource
{
	GENERATED_BODY()

public:
	// ─────────────────────────────────────────────
	//  METHODS
	// ─────────────────────────────────────────────
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CheckCollisions();
	void CheckCollisions_Implementation();

	UFUNCTION(BlueprintCallable)
	void OverlapMultiByChannel();

	UFUNCTION(BlueprintCallable)
	float OverlapAnyTestByChannel() const;

	UFUNCTION(BlueprintCallable)
	TArray<FDirectionalTraceResult> DetectAllDirections() const;

private:
	UFUNCTION(BlueprintCallable)
	FDirectionalTraceResult LineTraceSingleByChannel(const FDirectionalTraceConfig& Config) const;
	
	// Initialise les directions par défaut si le tableau est vide
	void InitDefaultDirectionalTraces();

public :
	// ─────────────────────────────────────────────
	//  VARIABLES
	// ─────────────────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	float BrushSphereDistance = 4000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	FVector2D BrushSizesMinMax = FVector2D(50, 150);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	float BrushForwardOffset = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	TArray<FHitResult> ValidHitResults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	TArray<float> ValidBrushSizes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity | Proximity Sweep")
	FProximitySweepConfig ProximitySweepConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity | Closest Obstacle")
	FClosestObstacleConfig ClosestObstacleConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity | Directional Traces")
	TArray<FDirectionalTraceConfig> DirectionalTraces;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	FVector StartPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Proximity")
	TSubclassOf<UActorComponent> PaintableSurfaceClass;
};