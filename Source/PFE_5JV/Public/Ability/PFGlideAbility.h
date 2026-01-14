#pragma once

#include "CoreMinimal.h"
#include "Data/PFGlideAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFGlideAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFGlideAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide")
	TObjectPtr<UPFGlideAbilityData> DataPtr_;
};
