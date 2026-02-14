#pragma once

#include "CoreMinimal.h"
#include "Data/PFRollAbilityData.h"
#include "Resource/PFPhysicResource.h"
#include "Resource/PFVisualResource.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFRollAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFRollAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Roll")
	TObjectPtr<UPFRollAbilityData> Data_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Roll")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category="Roll")
	TObjectPtr<UPFVisualResource> VisualResourcePtr_;

	float PressTimeRight_;
	bool bIsDownRight_;
	float PressTimeLeft_;
	bool bIsDownLeft_;
	float TimerRoll_;
	float TimerPreRoll_;
	int RotationDir_;
	bool bIsRollComplete_ = true;
	FQuat PrevQuat_;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;

	virtual void ComponentEnable_Implementation() override;
	
	UFUNCTION(BlueprintCallable, Category = "Roll")
	void TryCallRollRight(float inputValue, int rotationSide);

	UFUNCTION(BlueprintCallable, Category = "Roll")
	void TryCallRollLeft(float inputValue, int rotationSide);

	UFUNCTION(BlueprintCallable, Category = "Roll")
	bool PreRollCheck(float deltatTime);
	
	UFUNCTION(BlueprintCallable, Category = "Roll")
	bool Roll(float deltaTime);
};
