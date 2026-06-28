#pragma once

#include "CoreMinimal.h"
#include "CollisionPreset/PFCollisionPreset.h"
#include "Data/PFCollisionResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFCollisionResource.generated.h"

class UPFPhysicResource;


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
	bool bHitRight;
	bool bHitLeft;
	bool bHitUp; 
	bool bHitDown;  

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFCollisionResourceData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UPFCollisionPreset> CurrentPresetPtr_;
	
	UPROPERTY()
	TObjectPtr<UWorld> OwnerWorld;

	UPROPERTY()
	TArray<FStoredPlaytestInfo> GameInfoList_;
	
public:
	UFUNCTION(BlueprintCallable)
	void ChangeCollisionPreset(TSubclassOf<UPFCollisionPreset> collisionPreset);

	virtual int GetPriority() const override;
	
protected:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;
	
	void CheckFlank(float deltaTime);
	void RecordInfoForPlayTest();
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* hitComp, AActor* otherActor,
		UPrimitiveComponent* otherComp,	FVector normalImpulse, const FHitResult& hit);
};
