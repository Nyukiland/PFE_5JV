#pragma once

#include "CoreMinimal.h"
#include "Data/PFGlideAbilityData.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFGlideAbility.generated.h"

class UPFPhysicResource;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFGlideAbility : public UPFAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Glide")
	FVector2D MoveInput_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Glide")
	TObjectPtr<UPFPhysicResource> PhysicResource_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide")
	TObjectPtr<UPFGlideAbilityData> DataPtr_;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Glide")
	FVector2D Input_;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;

	UFUNCTION(BlueprintCallable, Category = "Glide")
	void ReceiveInputMovement(FVector2D inputValue);
	
protected:	
	UFUNCTION(BlueprintCallable, Category = "Glide")
	void MovementBasedJoystick();
	
	UFUNCTION(BlueprintCallable, Category = "Glide")
	void ConstantMovement(float deltaTime);
};
