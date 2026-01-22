#pragma once

#include "CoreMinimal.h"
#include "Ability/PFDiveAbility.h"
#include "Ability/PFGlideAbility.h"
#include "Ability/PFRollAbility.h"
#include "Ability/PFTurnAbility.h"
#include "Ability/PFWingVisualAbility.h"
#include "Ability/PFWingBeatAbility.h"
#include "StateMachine/State/PFCombinedState.h"
#include "PFBaseCombinedState.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFBaseCombinedState : public UPFCombinedState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFGlideAbility> GlidePtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFTurnAbility> TurnPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFDiveAbility> DivePtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFWingVisualAbility> WingVisualPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFWingBeatAbility> WingBeatPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFRollAbility> RollPtr_;
	
public:
	virtual void OnEnter_Implementation() override;
};
