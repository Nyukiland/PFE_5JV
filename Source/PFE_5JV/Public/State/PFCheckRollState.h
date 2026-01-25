#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFState.h"
#include "PFCheckRollState.generated.h"

class UPFRollAbility;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFCheckRollState : public UPFState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFRollAbility> RollPtr_;
	
public:
	virtual void OnEnter_Implementation() override;
};
