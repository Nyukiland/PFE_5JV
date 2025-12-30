#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PFPlayerCharacter.generated.h"

class UPFStateComponent;
class UPFState;

UCLASS(Blueprintable)
class PFE_5JV_API APFPlayerCharacter : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UPFStateComponent>> DefaultStateComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPFState> DefaultState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPFState> FirstState;

private:
	UPROPERTY()
	TArray<TObjectPtr<UPFStateComponent>> StateComponentsPtr_;

	UPROPERTY()
	TObjectPtr<UPFState> CurrentStatePtr_;
	
	int ResourcesCount_;
	int ActiveAbilities_ = 0;
	
public:	
	APFPlayerCharacter();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//State Component
	UFUNCTION(BlueprintCallable, Category = "Player")
	UPFStateComponent* GetStateComponent(TSubclassOf<UPFStateComponent> componentClass);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void ActivateAbilityComponent(TSubclassOf<UPFStateComponent> componentClass);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void DeactivateAbilityComponent(TSubclassOf<UPFStateComponent> componentClass);

	//State
	UFUNCTION(BlueprintCallable, Category = "Player")
	void ChangeState(TSubclassOf<UPFState> newState);

	private:
	//State Component
	UPFStateComponent* GetStateComponent(TSubclassOf<UPFStateComponent> componentClass, int& index);

	void ActivateAbilityComponent(UPFStateComponent* componentClass, int index);

	void DeactivateAbilityComponent(UPFStateComponent* componentClass, int index);

	void DeactivateAllAbilityComponents();
};
