#pragma once

#include "CoreMinimal.h"
#include "Data/PFWingBeatAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFWingBeatAbility.generated.h"

class UPFVisualResource;
class UPFWingBeatAbilityData;
class UPFPhysicResource;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWingBeatCalled);

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFWingBeatAbility : public UPFAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Dive")
	FOnWingBeatCalled OnWingBeatCalled;
	virtual FString GetInfo_Implementation() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UPFWingBeatAbilityData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFPhysicResource> PhysicResource_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFVisualResource> VisualResource_;

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

	// WingBeat Input datas : 
	float InputRightRegistered_;
	float InputLeftRegistered_;
	float AverageInputValue_;
	float InputRegistrationTimer_;

	// SuperWingBeat datas :
	float SuperBeatWingTimer_ = -1.f;
	float SuperWingBeatMinTiming_; 
	float SuperWingBeatMaxTiming_;
	bool bIsSuperBeatWingPossible_ = false;
	bool bIsSuperBeatWingTriggered_ = false;

	// Debug datas :
	float CurrentHeight_;
	float HeightAtWingBeatBeginning_;
	float MaxHeightGain_;
	float EffectiveHeightGainAfter3S_;
	float TimeEffectiveHeightCalculus_ = -1.f;
	
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
