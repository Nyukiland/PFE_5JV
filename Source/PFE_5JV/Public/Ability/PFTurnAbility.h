#pragma once

#include "CoreMinimal.h"
#include "Data/PFTurnAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFTurnAbility.generated.h"

class UPFPhysicResource;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFTurnAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn")
	TObjectPtr<UPFTurnAbilityData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;
	
	float RotationValue_;
	float InputRight_;
	float InputLeft_;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	
	UFUNCTION(BlueprintCallable, Category="Dive")
	void ReceiveInputLeft(float left);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void ReceiveInputRight(float right);
	
	UFUNCTION(BlueprintCallable, Category="Dive")
	void Turn(float deltaTime);

private:
	void GetRotationValue();
};
