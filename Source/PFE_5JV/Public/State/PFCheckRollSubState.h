#pragma once

#include "CoreMinimal.h"
#include "PFRollState.h"
#include "StateMachine/State/PFState.h"
#include "PFCheckRollSubState.generated.h"

class UPFRollAbility;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFCheckRollSubState : public UPFState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TSubclassOf<UPFRollState> RollState_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFRollAbility> RollPtr_;
	
public:
	virtual void OnEnter_Implementation() override;
	virtual void OnTick_Implementation(float deltaTime) override;
	virtual void OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent, const FInputActionValue& value) override;
};
