#pragma once

#include "CoreMinimal.h"
#include "Data/PFWingBeatAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFWingBeatAbility.generated.h"

class UPFDiveAbility;
class UPFVisualResource;
class UPFWingBeatAbilityData;
class UPFPhysicResource;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWingBeatCalled, int, wingBeatCount);

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFWingBeatAbility : public UPFAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="WingBeat")
	FOnWingBeatCalled OnWingBeatCalled;
	
	virtual FString GetInfo_Implementation() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat")
	TObjectPtr<UPFWingBeatAbilityData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WingBeat")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WingBeat")
	TObjectPtr<UPFDiveAbility> DiveAbilityPtr_;
		
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentEnable_Implementation() override;
	virtual void ComponentTick_Implementation(float deltaTime) override;

	float WingBeatInARowTimer;
	int WingBeatInARowCount;
	float TargetPitchAccumulator_;
	
	// Debug datas :
	float CurrentHeight_;
	float HeightAtWingBeatBeginning_;
	float MaxHeightGain_;
	
public:
	UFUNCTION(BlueprintCallable, Category="WingBeat")
	void WingBeat();

	UFUNCTION(BlueprintCallable, Category="WingBeat")
	bool IsCurrentlyGoingUp();
	
	UFUNCTION(BlueprintCallable, Category="WingBeat")
	float GetCurrentWingBeatPercentage();
	
private:
	void DebugHeight();
	float FindClosestClapValue(float pitch) const;
};
