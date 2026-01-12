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
	UPROPERTY(editanywhere, BlueprintReadWrite)
	FVector Force;
	UPROPERTY(editanywhere, BlueprintReadWrite)
	float Duration;
	UPROPERTY(editanywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> CurvePtr;

	FForceToAdd(const FVector& force, const float& duration, UCurveFloat* curvePtr)
	{
		Force = force;
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

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void AddForce(float ouais) const;

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void AddForce(FVector force, float duration = 0, UCurveFloat* curve = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void SetVelocity(FVector velocity);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	FVector GetVelocity() const;
};