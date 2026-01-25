#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFState.h"
#include "PFDiveState.generated.h"

class UPFDiveAbility;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFDiveState : public UPFState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TObjectPtr<UPFDiveAbility> DivePtr_;
	
public:
	virtual void OnEnter_Implementation() override;
};
