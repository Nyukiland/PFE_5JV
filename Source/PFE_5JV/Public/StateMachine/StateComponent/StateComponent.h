#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"

class APlayerCharacter;

UCLASS(Blueprintable)
class PFE_5JV_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool bIsActive = false;

protected:
	UPROPERTY()
	TObjectPtr<APlayerCharacter> Owner_;
	
public:	
	UStateComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentEarlyInit();
	virtual void ComponentEarlyInit_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ComponentInit(APlayerCharacter* owner);
	virtual void ComponentInit_Implementation(APlayerCharacter* owner);
	
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
