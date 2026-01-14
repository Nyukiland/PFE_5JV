#pragma once

#include "CoreMinimal.h"
#include "Data/PFDeathAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFDeathAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFDeathAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Death")
	TObjectPtr<UPFDeathAbilityData> DataPtr_;
};
