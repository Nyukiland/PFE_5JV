#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFState.h"
#include "PFWingBeatState.generated.h"

class UPFWingBeatAbility;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFWingBeatState : public UPFState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFWingBeatAbility> WingBeatPtr_;
	
public:
	virtual void OnEnter_Implementation() override;
};
