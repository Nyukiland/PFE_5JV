#pragma once

#include "CoreMinimal.h"
#include "Data/PFVisualResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFVisualResource.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFVisualResource : public UPFResource
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual")
	TObjectPtr<UPrimitiveComponent> BirdVisualPtr_;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<UPFVisualResourceData> DataPtr_;

	float RollRotation_;
	int RollPriority_;
	
public:
	void SetRollRotation(float rotation, int priority);

	UFUNCTION(BlueprintCallable, Category = "Visual")
	void ProcessRollRotation(float deltaTime);
};
