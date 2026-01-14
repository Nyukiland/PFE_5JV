#pragma once

#include "CoreMinimal.h"
#include "Data/PFStaminaResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFStaminaResource.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFStaminaResource : public UPFResource
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	TObjectPtr<UPFStaminaResourceData> DataPtr_;
	
};
