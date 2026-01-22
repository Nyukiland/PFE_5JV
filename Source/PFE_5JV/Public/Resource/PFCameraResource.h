#pragma once

#include "CoreMinimal.h"
#include "Ability/PFDiveAbility.h"
#include "Data/PFCameraResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFCameraResource.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PFE_5JV_API UPFCameraResource : public UPFResource
{
	GENERATED_BODY()

public:
	UPFCameraResource();
	
protected:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
	TObjectPtr<USceneComponent> CameraRootPtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
	TObjectPtr<USpringArmComponent> SpringArmPtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
	TObjectPtr<UCameraComponent> CameraPtr_;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Camera|References")
	TObjectPtr<UPFDiveAbility> DiveAbilityPtr_;

	UFUNCTION(BlueprintCallable)
	void UpdateRotation(float deltaTime);
	UFUNCTION(BlueprintCallable)
	void UpdatePosition(float deltaTime);
	UFUNCTION(blueprintCallable)
	void UpdateZoom(float deltaTime);

	FVector ComputeTargetLocation() const;

	UPROPERTY(EditAnywhere, blueprintReadWrite)
	TObjectPtr<UPFCameraResourceData> CameraResourceData_;

private:
	//Dive
	bool IsDiveActive_ = false;
	float DiveTheTimer_ = 0.f;
	float DiveTransitionTimer_ = 0.f;
	bool WasDiving_ = false;
	float ZoomStartDistance_;
	FRotator ZoomStartRotation_;

	// Flap
	float FlapTimer_ = 0.f;
	float FlapTransitionTimer_ = 0.f;
	bool WasFlapping_ = false;
};