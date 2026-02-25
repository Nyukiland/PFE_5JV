#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFState.h"
#include "PFCheckRollSubState.generated.h"

class UPFRollAbility;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFCheckRollSubState : public UPFState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFRollAbility> RollPtr_;
	
public:
	virtual void OnEnter_Implementation() override;
};
