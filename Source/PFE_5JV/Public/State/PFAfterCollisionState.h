#pragma once

#include "CoreMinimal.h"
#include "Resource/PFCollisionResource.h"
#include "StateMachine/State/PFState.h"
#include "PFAfterCollisionState.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFAfterCollisionState : public UPFState
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UPFCollisionResource> CollisionResourcePtr_;

protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnTick_Implementation(float deltaTime) override;
};
