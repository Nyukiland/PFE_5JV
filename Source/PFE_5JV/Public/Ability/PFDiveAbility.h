#pragma once

#include "CoreMinimal.h"
#include "Data/PFDiveAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFDiveAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFDiveAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UPFDiveAbilityData> DataPtr_;
};
