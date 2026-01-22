#pragma once

#include "CoreMinimal.h"
#include "Data/PFDiveAbilityData.h"
#include "Resource/PFPhysicResource.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFDiveAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFDiveAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UPFDiveAbilityData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;
	
	float HighestInput_;
	float InputRight_;
	float InputLeft_;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;

	UFUNCTION(BlueprintCallable, Category="Dive")
	void ReceiveInputLeft(float left);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void ReceiveInputRight(float right);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void Dive(float deltaTime);

	UFUNCTION(BlueprintCallable, Category="Dive")
	bool IsDiving()const;
	
private:
	void GetHighestValue();
};
