#pragma once

#include "CoreMinimal.h"
#include "Data/PFProximityResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFProximityResource.generated.h"

UCLASS()
class PFE_5JV_API UPFProximityResource : public UPFResource
{
	GENERATED_BODY()

public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Proximity")
	float GetMaxClosestDistance() const;

	UFUNCTION(BlueprintCallable, Category = "Proximity")
	float GetMaxDistanceWithBelow() const;
	
protected:
	
	UFUNCTION(BlueprintCallable, Category = "Proximity")
	void CheckCollisionInFront();
	
	UFUNCTION(BlueprintCallable, Category = "Proximity")
	void CheckClosestHit();
	
	UFUNCTION(BlueprintCallable, Category = "Proximity")
	void CheckBelowHit();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	TArray<FHitResult> ValidHitResults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	FHitResult ClosestHitResult;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	FHitResult HitBelowResult;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Proximity")
	TObjectPtr<UPFProximityResourceData> DataPtr_;

	UPROPERTY()
	TObjectPtr<UWorld> OwnerWorldPtr_;	
};