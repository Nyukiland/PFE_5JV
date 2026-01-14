#pragma once

#include "CoreMinimal.h"
#include "Data/PFWingBeatAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFWingBeatAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFWingBeatAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPFWingBeatAbilityData> DataPtr_;
};
