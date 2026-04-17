#pragma once

#include "CoreMinimal.h"
#include "Ability/PFDiveAbility.h"
#include "Ability/PFWingBeatAbility.h"
#include "StateMachine/State/PFCombinedState.h"
#include "PFAutoDiveState.generated.h"

class UPFWingBeatSubState;
class UPFBaseCombinedState;

UCLASS()
class PFE_5JV_API UPFAutoDiveState : public UPFCombinedState
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AutoDive")
	TObjectPtr<UPFDiveAbility> DivePtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AutoDive")
	TObjectPtr<UPFWingBeatAbility> WingBeatPtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AutoDive")
	TSubclassOf<UPFBaseCombinedState> BaseCombinedState_;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AutoDive")
	TSubclassOf<UPFWingBeatSubState> WingBeatSubState_;

public:
	virtual void OnEnter_Implementation() override;
	virtual void OnTick_Implementation(float deltaTime) override;

private:
	UFUNCTION()
	void OnWingBeat(int count);	
};
