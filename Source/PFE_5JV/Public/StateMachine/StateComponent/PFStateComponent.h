#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PFStateComponent.generated.h"

class APFPlayerCharacter;

UCLASS(Blueprintable)
class PFE_5JV_API UPFStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool bIsActive = false;

protected:
	UPROPERTY()
	TObjectPtr<APFPlayerCharacter> Owner_;
	
public:	
	UPFStateComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentEarlyInit();
	virtual void ComponentEarlyInit_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentInit(APFPlayerCharacter* owner);
	virtual void ComponentInit_Implementation(APFPlayerCharacter* owner);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentEnable();
	virtual void ComponentEnable_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentDisable();
	virtual void ComponentDisable_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentUpdate(float deltaTime);
	virtual void ComponentUpdate_Implementation(float deltaTime);
};
