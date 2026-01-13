#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveEvaluation.h"
#include "Data/PFPhysicResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFPhysicResource.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FForceToAdd
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Force;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldReset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> CurvePtr;

	FForceToAdd()
	{
		Force = FVector::ZeroVector;
		bShouldReset = false;
		Duration = 0;
		CurvePtr = nullptr;
	}
	
	FForceToAdd(const FVector& force, const bool bShouldResetForce, const float& duration, UCurveFloat* curvePtr)
	{
		Force = force;
		bShouldReset = bShouldResetForce;
		Duration = duration;
		CurvePtr = curvePtr;
	}
};

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFPhysicResource : public UPFResource
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicResource")
	TObjectPtr<UPFPhysicResourceData> DataPtr_;

	FVector Velocity_;
	UPROPERTY()
	TArray<FForceToAdd> AllForces_;
	
public:
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	float GetMaxSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "PhysicResource", meta = (AdvancedDisplay = "bisAdded, duration, curve"))
	void AddForce(FVector force, bool bShouldResetForce, float duration = 0, UCurveFloat* curve = nullptr);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void SetVelocity(FVector velocity);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	FVector GetVelocity() const;
};