#pragma once

#include "CoreMinimal.h"
#include "Ability/PFDiveAbility.h"
#include "Ability/PFWingBeatAbility.h"
#include "StateMachine/State/PFCombinedState.h"
#include "PFAutoDiveState.generated.h"

UCLASS()
class PFE_5JV_API UPFAutoDiveState : public UPFCombinedState
{
	GENERATED_BODY()

	protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AutoDive")
	TObjectPtr<UPFDiveAbility> DivePtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AutoDive")
	TObjectPtr<UPFWingBeatAbility> WingBeatPtr_;

public:
	virtual void OnEnter_Implementation() override;
};
