#pragma once

#include "CoreMinimal.h"
#include "PFWingVisualAbility.h"
#include "Data/PFRollAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFRollAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFRollAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Roll")
	TObjectPtr<UPFRollAbilityData> Data_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingVisual")
	TObjectPtr<UPrimitiveComponent> BirdVisualPtr_;
	
	float Timer_;
	int RotationDir_;
	bool bIsRollComplete_ = true;
	FRotator PrevRotator_;
	FQuat PrevQuat_;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	
	UFUNCTION(BlueprintCallable, Category = "Roll")
	void CallRoll(int sideRoll);
	
	UFUNCTION(BlueprintCallable, Category = "Roll")
	bool Roll(float deltaTime);
};
