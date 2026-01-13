#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PFStateComponent.generated.h"

class APFPlayerCharacter;

UCLASS(Abstract)
class PFE_5JV_API UPFStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool bIsActive = false;

	UPROPERTY()
	TObjectPtr<APFPlayerCharacter> Owner;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> Root;
	
public:	
	UPFStateComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentEarlyInit();
	virtual void ComponentEarlyInit_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentInit(APFPlayerCharacter* ownerObj);
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentEnable();
	virtual void ComponentEnable_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentDisable();
	virtual void ComponentDisable_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentTick(float deltaTime);
	virtual void ComponentTick_Implementation(float deltaTime);
};
