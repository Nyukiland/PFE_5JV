#pragma once

#include "CoreMinimal.h"
#include "PFAssistedAvoidanceData.h"
#include "Resource/PFPhysicResource.h"
#include "Resource/CollisionPreset/PFCollisionPreset.h"
#include "PFAssistedAvoidancePreset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PFE_5JV_API UPFAssistedAvoidancePreset : public UPFCollisionPreset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Avoidance Data")
	TObjectPtr<UPFAssistedAvoidanceData> DataPtr_;

private:
	UPROPERTY() TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;
	UPROPERTY() TObjectPtr<UWorld> OwnerWorldPtr_;
    
	FVector CurrentRepulsion_ = FVector::ZeroVector;
	bool bIsInHardAvoid_ = false;

public:
	virtual void InitPreset_Implementation(UPFCollisionResource* collisionResource) override;
	virtual void OnTickActions_Implementation(float deltaTime) override;

private:
	void ProcessAssistedAvoidance(float deltaTime, const FVector& currentVelocity);
	bool CheckAbsoluteShield(float deltaTime, const FVector& currentVelocity, const FVector& startPos, const FVector& forwardDir);
	void CalculateClampedIntent(const FVector& forwardDir, FVector& outIntentDir);
	void CalculateDynamicRays(const FVector& startPos, const FVector& forwardDir, const float* rayDist, FVector& outTotalRepulsion, FVector& outFirstOpenDir, float& outClosestDistance, bool& bOutHitCenter, int& outOpenRayCount);
	void JudgeAndApplyPredictiveForces(float deltaTime, const FVector& forwardDir, const FVector& intentDir, const FVector& totalRepulsion, const FVector& firstOpenDir, float closestDistance, float dynamicAvoidDistance, float speedMultiplier, bool bHitCenter, int openRayCount);
};
