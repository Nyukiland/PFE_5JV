#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PFMathHelper.generated.h"

UENUM()
enum class ERandomPinOut : uint8
{
	Random1,
	Random2
};

UCLASS()
class PFE_5JV_API UPFMathHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static float& IncrementFloat(UPARAM(ref) float& value, float amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static int& IncrementInt(UPARAM(ref) int& value, int amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static float& DecrementFloat(UPARAM(ref) float& value, float amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static int& DecrementInt(UPARAM(ref) int& value, int amount);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Vector")
	static float GetAngleBetweenVectors(const FVector vectorA = FVector(0,0,0), const FVector vectorB = FVector(0,0,0));

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Vector")
	static FVector GetClosestPoint(const FVector firstVector, const TArray<FVector> vectors);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Vector")
	static AActor* GetClosestActor(const AActor* baseActor, const TArray<AActor*> actors);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Random")
	static FVector2D RandomPointInCircle(float radius = 0);

	UFUNCTION(BlueprintCallable, Category = "Math Helper|Random")
	static FVector RandomPointInSphere(float radius = 0);

	UFUNCTION(BlueprintCallable, Category = "Math Helper", meta = (ExpandEnumAsExecs = "outputPins"))
	static void RandomOutput(ERandomPinOut& outputPins);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static float& ClampFloatByRef(UPARAM(ref) float& value, const float min, const float max);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static int& ClampIntByRef(UPARAM(ref) int& value, const int min, const int max);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static FVector GetOffsetBasedOnTransform(const FVector offset, const USceneComponent* toUseTransform);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static FVector RandomVector(const float intensity);

	UFUNCTION(BlueprintCallable, Category = "Math Helper")
	static float Get2dDistance(FVector a, FVector b);	
};
