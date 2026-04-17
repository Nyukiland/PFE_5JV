#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFState.h"
#include "PFDiveSubState.generated.h"

class UPFDiveAbility;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFDiveSubState : public UPFState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFDiveAbility> DivePtr_;
	
public:
	virtual void OnEnter_Implementation() override;
	virtual void OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent, const FInputActionValue& value) override;
};
