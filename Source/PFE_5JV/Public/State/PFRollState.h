#pragma once

#include "CoreMinimal.h"
#include "Ability/PFRollAbility.h"
#include "StateMachine/State/PFState.h"
#include "PFRollState.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFRollState : public UPFState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFRollAbility> RollPtr_;
	
public:
	virtual void OnEnter_Implementation() override;
};
