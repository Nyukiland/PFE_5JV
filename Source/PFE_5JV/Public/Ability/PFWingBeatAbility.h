#pragma once

#include "CoreMinimal.h"
#include "Data/PFWingBeatAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFWingBeatAbility.generated.h"

class UPFWingBeatAbilityData;
class UPFPhysicResource;


UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFWingBeatAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UPFWingBeatAbilityData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFPhysicResource> PhysicResource_;

	virtual void ComponentTick_Implementation(float deltaTime) override;

	// Previous input values 
	float PreviousInputRight_;
	float PreviousInputLeft_;

	// previous input values for calculation
	float PreviousInputRightRegistered_;
	float PreviousInputLeftRegistered_;

	// Current input values 
	float InputRight_;
	float InputLeft_;

	// input values for calculation
	float InputRightRegistered_;
	float InputLeftRegistered_;
	
	float AverageInputValue_;
	float TimeUntilNextInputRegistration;
	float TimeLeftToTriggerSuperBeatWing;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;

	UFUNCTION(BlueprintCallable, Category="WingBeat")
	void ReceiveInputLeft(float left);

	UFUNCTION(BlueprintCallable, Category="WingBeat")
	void ReceiveInputRight(float right);

	UFUNCTION(BlueprintCallable, Category="WingBeat")
	void WingBeat(float deltaTime);
	
private:
	void GetAverageInputValue();
};
