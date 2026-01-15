#pragma once

#include "CoreMinimal.h"
#include "Ability/PFDiveAbility.h"
#include "Ability/PFGlideAbility.h"
#include "Ability/PFTurnAbility.h"
#include "Ability/PFWingVisualAbility.h"
#include "StateMachine/State/PFCombinedState.h"
#include "PFBaseCombinedState.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFBaseCombinedState : public UPFCombinedState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFGlideAbility> Glide_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFTurnAbility> Turn_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFDiveAbility> Dive_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFWingVisualAbility> WingVisual_;
	
public:
	virtual void OnEnter_Implementation() override;
};
