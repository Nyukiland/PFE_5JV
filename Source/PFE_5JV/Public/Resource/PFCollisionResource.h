#pragma once

#include "CoreMinimal.h"
#include "Data/PFCollisionResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFCollisionResource.generated.h"

class UPFPhysicResource;
class UPFTurnAbility;
class UPFDiveAbility;
class UPFWingBeatAbility;

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

public:
	FStoredCollisionInfo()
	{
		ForwardForce = 0;
		GlobalForce = FVector::ZeroVector;

		Position = FVector::ZeroVector;
		PhysicRotation = FRotator::ZeroRotator;
	}
	
	FStoredCollisionInfo(float forward, FVector global, FVector position,
		FRotator physicRotation)
	{
		ForwardForce = forward;
		GlobalForce = global;

		Position = position;
		PhysicRotation = physicRotation;
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

UENUM()
enum class ERayDir : uint8
{
	Forward,
	Right,
	Left,
	Top,
	Bottom,
	TopRight,
	TopLeft,
	BottomRight,
	BottomLeft,
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

	bool bHitRight;
	bool bHitLeft;
	bool bHitUp; 
	bool bHitDown;  

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFCollisionResourceData> DataPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFTurnAbility> TurnAbilityPtr_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFDiveAbility> DiveAbilityPtr_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFWingBeatAbility> WingBeatAbilityPtr_;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bCanRecord_ = true;

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bIsHelperActive_ = true;
	
	UPROPERTY()
	TObjectPtr<UWorld> OwnerWorld;
	
	UPROPERTY()
	TArray<float> TimeSavedList_;
	
	UPROPERTY()
	TArray<FStoredCollisionInfo> StoredCollisionInfoList_;

	UPROPERTY()
	TArray<FStoredPlaytestInfo> GameInfoList_;

	float TimeSavedOnImpact_ = 0;
	FVector CurrentRepulsion_ = FVector::ZeroVector;
	bool bIsInHardAvoid_ = false;
	
	bool RayDirHits_[9];
	
public:
	UPFCollisionResource();
	
	UFUNCTION(BlueprintCallable, Category = "Collision")
	bool RewindAfterCollision(float deltaTime);
	
	UFUNCTION(BlueprintCallable, Category = "Collision")
	bool HasHitDirection(ERayDir Direction) const;
	
	virtual FString GetInfo_Implementation() override;
	
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void ChangeHelperActive(bool newActive);

	UFUNCTION(BlueprintCallable, Category = "Collision")
	bool GetHelperActive() const;
	
protected:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	// Use after physic required
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;

	bool IsHardCollision(const FVector& impactNormal, const FVector& currentVelocity) const;
	void HandleSoftCollision(const FVector& impactNormal, const FVector& currentVelocity);

	FVector GetPlayerIntendedDirection() const;
	void CheckFlank(float deltaTime);
	void CheckPredictiveCollision(float deltaTime, const FVector& playerDesiredDir);
	void FirePredictiveRays(const FVector& startPos, const FVector* rayDirs, const float* rayDist, const FVector& playerDesiredDir,
		FVector& outTotalRepulsion, FVector& outFirstOpenDir, float& outClosestDistance, bool& bOutHitCenter, int& outOpenRayCount);
	void UpdateSteeringRepulsion(float deltaTime, const FVector& forwardDir, const FVector& totalRepulsion, const FVector& bestDir, bool bHitCenter);
	void ApplyPredictiveForces(float DeltaTime, float ClosestDistance, float DynamicAvoidDistance, float SpeedMultiplier);
	void DrawDebugWhiskerCone(const FVector& StartPos, const FVector& EndPos, bool bHit, const FHitResult& HitResult);
	
	void RecordInfoForRollBack(float deltaTime);
	void RecordInfoForPlayTest();
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnHardCollisionEventCalled();
};
