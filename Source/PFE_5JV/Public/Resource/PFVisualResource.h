#pragma once

#include "CoreMinimal.h"
#include "Data/PFVisualResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFVisualResource.generated.h"

UENUM(BlueprintType)
enum class EStateMaterial : uint8
{
	ESM_Normal UMETA(DisplayName = "Normal"),
	ESM_SuperWingBeatPossible UMETA(DisplayName = "SuperWingBeat can be triggered"),
};

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFVisualResource : public UPFResource
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual")
	TObjectPtr<UPrimitiveComponent> BirdVisualPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual")
	TObjectPtr<UPrimitiveComponent> BirdMeshObjectPtr_;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<UPFVisualResourceData> DataPtr_;

	float RollRotation_;
	int RollPriority_;

	FRotator BaseBirdVisuRotator_;
	FRotator BirdVisuRotation_;
	int BirdVisuPriority_;

	float CurrentRollValue_;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Visual")
	FRotator GetBirdVisualRotation() const;
	
	UFUNCTION(BlueprintCallable, Category = "Visual")
	void AddToRollRotation(float rotationToAdd, int priority);
	
	UFUNCTION(BlueprintCallable, Category = "Visual")
	void SetRollRotation(float rotation, int priority);

	UFUNCTION(BlueprintCallable, Category = "Visual")
	void ProcessRollRotation(float deltaTime);

protected:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;
};
