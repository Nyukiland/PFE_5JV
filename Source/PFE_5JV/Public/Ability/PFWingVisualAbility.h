#pragma once

#include "CoreMinimal.h"
#include "Data/PFWingVisualAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFWingVisualAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFWingVisualAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual")
	TObjectPtr<UPFWingVisualAbilityData> DataPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WingVisual",
		meta=(UseComponentPicker, AllowedClasses="SceneComponent"))
	TObjectPtr<USceneComponent> LeftWingRoot_;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WingVisual",
		meta=(UseComponentPicker, AllowedClasses="SceneComponent"))
	TObjectPtr<USceneComponent> RightWingRoot_;

	float InputLeft_;
	float InputRight_;

	float CurrentMedianValue_;
	
public:
	UFUNCTION(BlueprintCallable, Category = "WingVisual")
	void ReceiveInputRight(float right);
	
	UFUNCTION(BlueprintCallable, Category = "WingVisual")
	void ReceiveInputLeft(float left);

	UFUNCTION(BlueprintCallable, Category = "WingVisual")
	void ChangeRotation(float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "WingVisual")
	void ChangeWingRotation(float deltaTime);
};
