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

	FRotator BaseBirdVisuRotator_;
	FRotator BirdVisuRotation_;
	int BirdVisuPriority_;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Visual")
	FRotator GetBirdVisualRotation() const;
	
	UFUNCTION(BlueprintCallable, Category = "Visual")
	void SetBirdVisualRotation(FRotator rotation, int priority);
	
	UFUNCTION(BlueprintCallable, Category = "Visual")
	void SetRollRotation(float rotation, int priority);

	UFUNCTION(BlueprintCallable, Category = "Visual")
	void ProcessRollRotation(float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "Visual")
	void ProcessBirdVisuRotation(float deltaTime);

protected:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
};
