#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveEvaluation.h"
#include "Data/PFPhysicResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFPhysicResource.generated.h"

class UPFDiveAbilityData;
class UPFWingBeatAbilityData;

USTRUCT(Blueprintable, BlueprintType)
struct FVelocityToAdd
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldReset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShoudEndAdded;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Timer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> CurvePtr;

	FVelocityToAdd()
	{
		Velocity = FVector::ZeroVector;
		bShouldReset = false;
		bShoudEndAdded = false;
		Duration = 0;
		Timer = 0;
		CurvePtr = nullptr;
	}

	FVelocityToAdd(const FVector& force)
	{
		Velocity = force;
		bShouldReset = true;
		bShoudEndAdded = false;
		Duration = 0;
		Timer = 0;
		CurvePtr = nullptr;
	}
	
	FVelocityToAdd(const FVector& force, const bool bShouldResetForce, const bool bShouldAddAtTheEnd, const float& duration, UCurveFloat* curvePtr)
	{
		Velocity = force;
		bShouldReset = bShouldResetForce;
		bShoudEndAdded = bShouldAddAtTheEnd;
		Duration = duration;
		Timer = duration;
		CurvePtr = curvePtr;
	}
};

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFPhysicResource : public UPFResource
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FVector CurrentForwardVelocity_;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FVector CurrentGlobalVelocity_;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pitch")
	float CurrentPitchValue_;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pitch")
	bool bIsFlipped;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicResource")
	TObjectPtr<UPFPhysicResourceData> DataPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingBeatResource")
	TObjectPtr<UPFWingBeatAbilityData> DataWingBeatPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DiveResource")
	TObjectPtr<UPFDiveAbilityData> DataDivePtr_;
	
	float MaxAbsoluteVelocity_;
	float GravityTimer_ = 0;
	float FrictionTimer_ = 0;
	FVector GlobalVelocity_;
	FVector ForwardVelocity_;
	FVector AngularVelocity_;
	UPROPERTY()
	TArray<FVelocityToAdd> GlobalVelocities_;
	UPROPERTY()
	TArray<FVelocityToAdd> ForwardVelocities_;
	UPROPERTY()
	TArray<FVelocityToAdd> AngularVelocities_;

	bool PitchResetRot_;
	float PitchRotation_;
	int PitchPriority_;

	float CurrentOverrideForwardVelocity_;
	float AutoSteerTimer_ = 0.f;
	FRotator AutoSteerTargetRotation_ = FRotator::ZeroRotator;
	float AutoSteerTurnRate_ = 0.f;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;
	virtual FString GetInfo_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void SetKinematic(bool bisKinematic);
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	float GetMaxBoostVelocity() const;

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	float GetForwardVelocityPercentage() const;
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource", meta = (AdvancedDisplay = "bShouldResetForce, bShouldAddAtTheEnd, duration, curve"))
	void AddVelocity(FVector velocity, bool bShouldResetVelocity = true, bool bShouldAddAtTheEnd = false, float duration = 0, UCurveFloat* curve = nullptr);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource", meta = (AdvancedDisplay = "bShouldResetForce, bShouldAddAtTheEnd, duration, curve"))
	void AddForwardVelocity(float velocity, bool bShouldResetVelocity = true, bool bShouldAddAtTheEnd = false, float duration = 0, UCurveFloat* curve = nullptr);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	FVector GetCurrentVelocity() const;

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	float GetAlignmentWithUp() const;
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	float GetCurrentAirFriction() const;
	
	void ProcessAirFriction(const float deltaTime);
	
	void ProcessVelocity(const float deltaTime);

	void ProcessBaseMaxVelocity(const float deltaTime);
	
	void ProcessOverrideVelocity();
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource", meta = (AdvancedDisplay = "bShouldResetForce, bShouldAddAtTheEnd, duration, curve"))
	void SetYawRotationVelocity(float rotation, bool bShouldResetVelocity = true, bool bShouldAddAtTheEnd = false, float duration = 0, UCurveFloat* curve = nullptr);
	
	void ProcessAngularVelocity(const float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void ForceAutoSteer(FRotator targetRotation, float turnRate, float duration);
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	bool IsAutoSteering() const;

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void AddToPitchRotationVisual(float rotationToAdd, int priority);
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void SetPitchRotationVisual(float rotation, int priority);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void HardSetPitchRotationVisual(float rotation);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void ProcessPitchVisual(float deltaTime);
	
	void DoGravity(const float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	void ResetPhysicsTimer();
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource")
	float FrictionPercentValue() const;

	UFUNCTION(BlueprintCallable, Category = "PhysicResource/Debug")
	void OverrideForwardVelocity(float velocity);
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource/Debug")
	void RemoveGlobalVelocities();

	UFUNCTION(BlueprintCallable, Category = "PhysicResource/Debug")
	void RemoveForwardVelocities();

	UFUNCTION(BlueprintCallable, Category = "PhysicResource/Debug")
	void RemoveAngularVelocities();
	
	UFUNCTION(BlueprintCallable, Category = "PhysicResource/Debug")
	void RemoveVelocities();

	UFUNCTION(BlueprintCallable, Category = "PhysicResource/Debug")
	void RemoveAllVelocities();

	UFUNCTION(BlueprintCallable, Category = "PhysicResource/Debug")
	void StopAllMotion();
	
private:
	FVector CalculateVelocity(FVelocityToAdd* velocity, float deltaTime, FVector& VelocityGlobal) const;
};