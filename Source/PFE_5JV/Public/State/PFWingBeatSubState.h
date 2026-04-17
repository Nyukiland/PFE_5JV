#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFState.h"
#include "PFWingBeatSubState.generated.h"

class UPFWingBeatAbility;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFWingBeatSubState : public UPFState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFWingBeatAbility> WingBeatPtr_;
	
public:
	virtual void OnEnter_Implementation() override;
	virtual void OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent, const FInputActionValue& value) override;
};
