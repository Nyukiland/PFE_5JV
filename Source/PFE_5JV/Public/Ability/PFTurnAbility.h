#pragma once

#include "CoreMinimal.h"
#include "Data/PFTurnAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFTurnAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFTurnAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn")
	TObjectPtr<UPFTurnAbilityData> DataPtr_;
};
