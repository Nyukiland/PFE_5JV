#pragma once

#include "CoreMinimal.h"
#include "Data/PFDiveAbilityData.h"
#include "Resource/PFPhysicResource.h"
#include "Resource/PFVisualResource.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFVisualResource> VisualResourcePtr_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	float InputRight_;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	float InputLeft_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	float CurrentMedianValue_;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	float HighestInput_;

	float Timer_;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;

	virtual void ComponentDisable_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Dive")
	void ReceiveInputLeft(float left);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void ReceiveInputRight(float right);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void Dive(float deltaTime);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void DiveVisual(float deltaTime);

	UFUNCTION(BlueprintCallable, Category="Dive")
	bool IsDiving() const;

	UFUNCTION(BlueprintCallable, Category="AutoDive")
	void AutoDive(float deltaTime);
	
	UFUNCTION(BlueprintCallable, Category="AutoDive")
	bool AutoDiveComplete() const;
	
private:
	void GetHighestValue();
};
