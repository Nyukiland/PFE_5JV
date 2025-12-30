#include "StateMachine/PFPlayerCharacter.h"

#include "StateMachine/State/PFState.h"
#include "StateMachine/StateComponent/PFStateComponent.h"

APFPlayerCharacter::APFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APFPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APFPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UPFStateComponent* APFPlayerCharacter::GetStateComponent(TSubclassOf<UPFStateComponent> componentClass)
{
	int i = 0;
	return GetStateComponent(componentClass, i);
}

void APFPlayerCharacter::ActivateAbilityComponent(TSubclassOf<UPFStateComponent> componentClass)
{
	int i = 0;
	UPFStateComponent* comp = GetStateComponent(componentClass, i);
	ActivateAbilityComponent(comp, i);
}

void APFPlayerCharacter::DeactivateAbilityComponent(TSubclassOf<UPFStateComponent> componentClass)
{
	int i = 0;
	UPFStateComponent* comp = GetStateComponent(componentClass, i);
	DeactivateAbilityComponent(comp, i);
}

void APFPlayerCharacter::ChangeState(TSubclassOf<UPFState> newState)
{
	if (!newState)
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerCharacter] invalid new state when trying to change state"));
		return;
	}
	
	if (CurrentStatePtr_)
		CurrentStatePtr_->OnExit();

	DeactivateAllAbilityComponents();
	CurrentStatePtr_ = NewObject<UPFState>(this, newState);
	CurrentStatePtr_->OnEnter();
}

UPFStateComponent* APFPlayerCharacter::GetStateComponent(TSubclassOf<UPFStateComponent> componentClass, int& index)
{
	index = -1;

	for (int i = 0; i < StateComponentsPtr_.Num(); i++)
	{
		UPFStateComponent* comp = StateComponentsPtr_[i];

		if (comp && comp->IsA(componentClass))
		{
			index = i;
			return comp;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to get component of class: %s"), *componentClass->GetName());

	return nullptr;
}

void APFPlayerCharacter::ActivateAbilityComponent(UPFStateComponent* componentClass, int index)
{

}

void APFPlayerCharacter::DeactivateAbilityComponent(UPFStateComponent* componentClass, int index)
{
}

void APFPlayerCharacter::DeactivateAllAbilityComponents()
{
	
}

