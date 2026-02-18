#pragma once

#include "CoreMinimal.h"
#include "Data/PFTurnAbilityData.h"
#include "Resource/PFVisualResource.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFTurnAbility.generated.h"

class UPFPhysicResource;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFTurnAbility : public UPFAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn")
	float InputRight_;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn")
	float InputLeft_;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn")
	TObjectPtr<UPFTurnAbilityData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn")
	TObjectPtr<UPFVisualResource> VisualResourcePtr_;
	
	float RotationValue_;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentDisable_Implementation() override;
	virtual FString GetInfo_Implementation() override;
	
	UFUNCTION(BlueprintCallable, Category="Turn")
	void ReceiveInputLeft(float left);

	UFUNCTION(BlueprintCallable, Category="Turn")
	void ReceiveInputRight(float right);
	
	UFUNCTION(BlueprintCallable, Category="Turn")
	void Turn(float deltaTime);

	UFUNCTION(BlueprintCallable, Category="Turn")
	void TurnVisual();

private:
	void GetRotationValue();
};
