#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include "PFPlayerCharacter.generated.h"

class UPFStateComponent;
class UPFState;

UCLASS(Blueprintable)
class PFE_5JV_API APFPlayerCharacter : public APawn
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<UPFState> DefaultState_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<UPFState> FirstState_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TObjectPtr<UInputMappingContext> InputMappingContextPtr_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TArray<TObjectPtr<UInputAction>> InputActionsPtr_;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UPFStateComponent>> StateComponentsPtr_;

	UPROPERTY()
	TMap<TSubclassOf<UPFStateComponent>, int> ComponentIndexMap_;
	
	UPROPERTY()
	TObjectPtr<UPFState> CurrentStatePtr_;
	
	int ResourcesCount_ = 0;
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

	UFUNCTION(BlueprintCallable, Category = "Player")
	UPFStateComponent* GetAndActivateComponent(TSubclassOf<UPFStateComponent> componentClass);

	//State
	UFUNCTION(BlueprintCallable, Category = "Player")
	void ChangeState(TSubclassOf<UPFState> newState);

	private:
	//State Component
	UPFStateComponent* GetStateComponent(TSubclassOf<UPFStateComponent> componentClass, int& outIndex);

	void ActivateAbilityComponent(UPFStateComponent* componentClass, int index);

	void DeactivateAbilityComponent(UPFStateComponent* componentClass, int index);

	void DeactivateAllAbilityComponents();

	void SwapComponents(int a, int b);

	//Input
	void OnInputAction(const FInputActionInstance& instance);
};
