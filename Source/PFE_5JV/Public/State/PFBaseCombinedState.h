#pragma once

#include "CoreMinimal.h"
#include "Ability/PFGlideAbility.h"
#include "StateMachine/State/PFCombinedState.h"
#include "PFBaseCombinedState.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFBaseCombinedState : public UPFCombinedState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFGlideAbility> GlidePtr_;
	
public:
	virtual void OnEnter_Implementation() override;
};
