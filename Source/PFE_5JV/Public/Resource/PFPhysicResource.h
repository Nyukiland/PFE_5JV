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
		bShoudEndAdded = false;
		Duration = 0;
		CurvePtr = nullptr;
	}

	FForceToAdd(const FVector& force)
	{
		Force = force;
		bShouldReset = true;
		bShoudEndAdded = false;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FVector CurrentForwardVelo_;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FVector CurrentGlobalVelo_;	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicResource")
	TObjectPtr<UPFPhysicResourceData> DataPtr_;

	float GravityTimer_;
	FVector GlobalVelocity_;
	FVector ForwardVelo_;
	FVector AngularVelocity_;
	UPROPERTY()
	TArray<FForceToAdd> AllForces_;
	UPROPERTY()
	TArray<FForceToAdd> ForwardForces_;
	UPROPERTY()
	TArray<FForceToAdd> AngularForces_;

	float PitchRotation_;
	int PitchPriority_;
	
public:
	UFUNCTION(Blueprintable, Category = "PhysicResource")
	float GetCurrentSpeedPercentage();

	UFUNCTION(Blueprintable, Category = "PhysicResource")
	float GetForwardSpeedPercentage();
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource", meta = (AdvancedDisplay = "bShouldResetForce, bShouldAddAtTheEnd, duration, curve"))
	void AddForce(FVector force, bool bShouldResetForce = true, bool bShouldAddAtTheEnd = false, float duration = 0, UCurveFloat* curve = nullptr);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource", meta = (AdvancedDisplay = "bShouldResetForce, bShouldAddAtTheEnd, duration, curve"))
	void AddForwardForce(float force, bool bShouldResetForce = true, bool bShouldAddAtTheEnd = false, float duration = 0, UCurveFloat* curve = nullptr);

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
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource", meta = (AdvancedDisplay = "bShouldResetForce, bShouldAddAtTheEnd, duration, curve"))
	void SetYawRotationForce(float rotation, bool bShouldResetForce = true, bool bShouldAddAtTheEnd = false, float duration = 0, UCurveFloat* curve = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void ProcessAngularVelocity(const float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void SetPitchRotationVisual(float rotation, int priority);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void ProcessPitchVisual();
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void DoGravity(const float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void ResetGravityTimer();

private:
	FVector CalculateForce(FForceToAdd* force, float deltaTime, FVector& VelocityGlobal);
};