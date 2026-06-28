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
	UPROPERTY()
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;
	UPROPERTY()
	TObjectPtr<UWorld> OwnerWorldPtr_;

	FVector SmoothedSteering_ = FVector::ZeroVector;

public:
	virtual void InitPreset_Implementation(UPFCollisionResource* collisionResource) override;
	virtual void OnTickActions_Implementation(float deltaTime) override;

private:
	void ProcessAssistedAvoidance(float deltaTime, const FVector& currentVelocity);
	void CalculatePlayerIntent(FVector& outIntentDir);
	void ApplySmoothSteering(const FVector& targetDir, float deltaTime);
};
