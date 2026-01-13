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
	bool bShoudEndAdded;
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
	
	FForceToAdd(const FVector& force, const bool bShouldResetForce, const bool bShouldAddAtTheEnd, const float& duration, UCurveFloat* curvePtr)
	{
		Force = force;
		bShouldReset = bShouldResetForce;
		bShoudEndAdded = bShouldAddAtTheEnd;
		Duration = duration;
		CurvePtr = curvePtr;
	}
};

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFPhysicResource : public UPFResource
{
	GENERATED_BODY()

public:
	float AirFriction;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicResource")
	TObjectPtr<UPFPhysicResourceData> DataPtr_;

	FVector Velocity_;
	UPROPERTY()
	TArray<FForceToAdd> AllForces_;
	
public:
	UFUNCTION(BlueprintCallable, Category = "PhysicResource", meta = (AdvancedDisplay = "bisAdded, duration, curve"))
	void AddForce(FVector force, bool bShouldResetForce = true, bool bShouldAddAtTheEnd = false, float duration = 0, UCurveFloat* curve = nullptr);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	FVector GetCurrentVelocity() const;

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	float GetAlignmentWithUp() const;
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	float GetCurrentAirFriction() const;
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void ProcessAirFriction(const float deltaTime);
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void ProcessVelocity(const float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void ProcessMaxSpeed(const float deltaTime);
};