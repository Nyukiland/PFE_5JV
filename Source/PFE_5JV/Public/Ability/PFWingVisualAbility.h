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
};
