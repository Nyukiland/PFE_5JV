#pragma once

#include "CoreMinimal.h"
#include "Data/PFWingBeatAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFWingBeatAbility.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UPFWingBeatAbilityData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;
		
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentEnable_Implementation() override;
	virtual void ComponentTick_Implementation(float deltaTime) override;

	float WingBeatInARowTimer;
	int WingBeatInARowCount;
	float WingBeatValue01;
	
	// Debug datas :
	float CurrentHeight_;
	float HeightAtWingBeatBeginning_;
	float MaxHeightGain_;
	
public:
	UFUNCTION(BlueprintCallable, Category="WingBeat")
	void WingBeat();

private:
	void DebugHeight();
	float RecalculateWingBeat01();
};
