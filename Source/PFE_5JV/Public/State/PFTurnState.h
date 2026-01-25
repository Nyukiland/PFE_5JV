#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFState.h"
#include "PFTurnState.generated.h"

class UPFTurnAbility;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFTurnState : public UPFState
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFTurnAbility> TurnPtr_;
	
public:
	virtual void OnEnter_Implementation() override;
};
