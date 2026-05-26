#pragma once

#include "CoreMinimal.h"
#include "Data/PFProximityResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFProximityResource.generated.h"

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

	bool IsInit = false;
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Proximity")
	TObjectPtr<UPFProximityResourceData> DataPtr_;
	
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
	FDirectionalTraceResult DetectBottom() const;

private:
	UFUNCTION(BlueprintCallable)
	FDirectionalTraceResult LineTraceSingleByChannel(const FDirectionalTraceConfig& Config) const;
	
	// Initialise les directions par défaut si le tableau est vide
	void InitDefaultDirectionalTraces();

public :
	// ─────────────────────────────────────────────
	//  VARIABLES
	// ─────────────────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity | Private")
	FDirectionalTraceConfig DownTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity | Private")
	TArray<FHitResult> ValidHitResults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity | Private")
	TArray<float> ValidBrushSizes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity | Private")
	FVector StartPosition = FVector::ZeroVector;
};