#pragma once

#include "CoreMinimal.h"
#include "PFPhysicResource.h"
#include "Ability/PFDiveAbility.h"
#include "Data/PFCollisionResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFCollisionResource.generated.h"

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
	// TODO: Add Scoring info 

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
	TArray<double> TimeSavedList_;
	
	UPROPERTY()
	TArray<FStoredCollisionInfo> StoredCollisionInfoList_;

	UPROPERTY()
	TArray<FStoredPlaytestInfo> GameInfoList_;

protected:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;

	void RecordInfoForRollBack(float deltaTime);

	void RecordInfoForPlayTest();
};
