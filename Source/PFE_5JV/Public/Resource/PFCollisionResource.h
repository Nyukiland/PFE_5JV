#pragma once

#include "CoreMinimal.h"
#include "PFPhysicResource.h"
#include "Ability/PFDiveAbility.h"
#include "Data/PFCollisionResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFCollisionResource.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCollision);

USTRUCT()
struct FStoredCollisionInfo
{
	GENERATED_BODY()
	
public:
	float ForwardForce;
	FVector GlobalForce;

	FVector Position;
	FRotator PhysicRotation;
	float Pitch;

public:
	FStoredCollisionInfo()
	{
		ForwardForce = 0;
		GlobalForce = FVector::ZeroVector;

		Position = FVector::ZeroVector;
		PhysicRotation = FRotator::ZeroRotator;
		Pitch = 0;
	}
	
	FStoredCollisionInfo(float forward, FVector global, FVector position,
		FRotator physicRotation, float pitch)
	{
		ForwardForce = forward;
		GlobalForce = global;

		Position = position;
		PhysicRotation = physicRotation;
		Pitch = pitch;
	}
};

USTRUCT()
struct FStoredPlaytestInfo
{
	GENERATED_BODY()
	
public:
	float ForwardForce;
	FVector Position;

public:
	FStoredPlaytestInfo()
	{
		ForwardForce = 0;
		Position = FVector::ZeroVector;
	}
	
	FStoredPlaytestInfo(float forward, FVector position)
	{
		ForwardForce = forward;
		Position = position;
	}
};

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFCollisionResource : public UPFResource
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Collision")
	FOnPlayerCollision OnSoftCollision;

	UPROPERTY(BlueprintAssignable, Category = "Collision")
	FOnPlayerCollision OnHardCollision;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFCollisionResourceData> DataPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFPhysicResource> PhysicResource_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFDiveAbility> DiveAbility_;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bCanRecord_ = true;

	UPROPERTY()
	TObjectPtr<UWorld> OwnerWorld;
	
	UPROPERTY()
	TArray<float> TimeSavedList_;
	
	UPROPERTY()
	TArray<FStoredCollisionInfo> StoredCollisionInfoList_;

	UPROPERTY()
	TArray<FStoredPlaytestInfo> GameInfoList_;

	float TimeSavedOnImpact_ = 0;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Collision")
	bool RewindAfterCollision(float deltaTime);
	
protected:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;

	bool IsHardCollision(const FVector& impactNormal, const FVector& currentVelocity) const;
	void HandleSoftCollision(const FVector& impactNormal, const FVector& currentVelocity);
	
	void CheckPredictiveCollision(float deltaTime);
	void RecordInfoForRollBack(float deltaTime);
	void RecordInfoForPlayTest();

	void ApplyAvoidanceSteering(const FHitResult& hit, float deltaTime);
	FVector FindBestEvasionDirection(const FVector& startPos, const FVector& currentVelocityNormal, const FVector& impactNormal);
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnHardCollisionEventCalled();
};
