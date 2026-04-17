#pragma once

#include "CoreMinimal.h"
#include "Ability/PFGlideAbility.h"
#include "StateMachine/State/PFCombinedState.h"
#include "PFBaseCombinedState.generated.h"

class UPFWingBeatSubState;
class UPFAutoDiveState;
class UPFDiveSubState;
class UPFTurnSubState;
class UPFCheckRollSubState;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFBaseCombinedState : public UPFCombinedState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFGlideAbility> GlidePtr_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TSubclassOf<UPFCheckRollSubState> CheckRollSubState_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TSubclassOf<UPFTurnSubState> TurnSubState_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TSubclassOf<UPFDiveSubState> DiveSubState_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TSubclassOf<UPFWingBeatSubState> WingBeatSubState_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TSubclassOf<UPFAutoDiveState> AutoDiveState_;
	
public:
	virtual void OnEnter_Implementation() override;
	virtual void OnTick_Implementation(float deltaTime) override;
	virtual void OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent, const FInputActionValue& value) override;
};
