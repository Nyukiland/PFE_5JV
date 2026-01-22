#pragma once

#include "CoreMinimal.h"
#include "Ability/PFGlideAbility.h"
#include "StateMachine/State/PFState.h"
#include "PFAutoDiveState.generated.h"

UCLASS()
class PFE_5JV_API UPFAutoDiveState : public UPFState
{
	GENERATED_BODY()

	protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AutoDive")
	TObjectPtr<UPFGlideAbility> GlidePtr_;

public:
	virtual void OnEnter_Implementation() override;
};
