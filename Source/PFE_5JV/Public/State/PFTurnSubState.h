#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFState.h"
#include "PFTurnSubState.generated.h"

class UPFTurnAbility;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFTurnSubState : public UPFState
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFTurnAbility> TurnPtr_;
	
public:
	virtual void OnEnter_Implementation() override;
	virtual void OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent, const FInputActionValue& value) override;
};
