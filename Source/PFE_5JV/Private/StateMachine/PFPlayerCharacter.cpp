#include "StateMachine/PFPlayerCharacter.h"

#include "StateMachine/State/PFState.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "StateMachine/StateComponent/PFStateComponent.h"

APFPlayerCharacter::APFPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* pc = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				pc->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(InputMappingContextPtr_, 0);
		}
	}

	StateComponentsPtr_.Empty();
	ComponentIndexMap_.Empty();
	ResourcesCount_ = 0;
	ActiveAbilities_ = 0;

	TArray<UPFStateComponent*> stateComponents;
	GetComponents(stateComponents);
	TArray<UPFAbility*> abilities;

	for (UPFStateComponent* comp : stateComponents)
	{
		if (UPFResource* resource = Cast<UPFResource>(comp))
		{
			StateComponentsPtr_.Add(resource);
			resource->bIsActive = true;
			ResourcesCount_++;
		}
		else if (UPFAbility* ability = Cast<UPFAbility>(comp))
		{
			abilities.Add(ability);
		}
	}

	StateComponentsPtr_.Append(abilities);

	for (int i = 0; i < StateComponentsPtr_.Num(); i++)
	{
		UPFStateComponent* comp = StateComponentsPtr_[i];

		UClass* classNative = comp->GetClass();

		while (classNative && Cast<UBlueprintGeneratedClass>(classNative))
		{
			classNative = classNative->GetSuperClass();
		}

		if (ComponentIndexMap_.Contains(classNative))
			UE_LOG(LogTemp, Error, TEXT("[Player] Duplicate component detected"));

		ComponentIndexMap_.Add(classNative, i);
		comp->ComponentEarlyInit();
	}

	for (UPFStateComponent* comp : StateComponentsPtr_)
		comp->ComponentInit(this);

	if (FirstState_)
		ChangeState(FirstState_);

	if (!DefaultState_)
	{
		UE_LOG(LogTemp, Error, TEXT("[Player] No default state"));
		return;
	}

	ChangeState(DefaultState_);
}

void APFPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentStatePtr_)
		CurrentStatePtr_->OnTick(DeltaTime);

	for (int i = 0; i < ResourcesCount_ + ActiveAbilities_; i++)
		StateComponentsPtr_[i]->ComponentTick(DeltaTime);
}

void APFPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* inputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	const ETriggerEvent events[] =
	{
		ETriggerEvent::Started,
		ETriggerEvent::Triggered,
		ETriggerEvent::Completed,
		ETriggerEvent::Canceled
	};

	for (UInputAction* action : InputActionsPtr_)
	{
		if (!action) continue;

		for (ETriggerEvent trigger : events)
			inputComponent->BindAction(action, trigger, this, &APFPlayerCharacter::OnInputAction);
	}
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

UPFStateComponent* APFPlayerCharacter::GetAndActivateComponent(TSubclassOf<UPFStateComponent> componentClass)
{
	int i = 0;
	UPFStateComponent* comp = GetStateComponent(componentClass, i);
	ActivateAbilityComponent(comp, i);

	return comp;
}

FText APFPlayerCharacter::GetInfoFromComponent() const
{
	FString text;

	FString stateName = CurrentStatePtr_ == nullptr ? TEXT("") : CurrentStatePtr_->GetName();
	
	text = TEXT("<hb>State:</>\n");
	text += TEXT("<b>CurrentState</>: ") + FString::Printf(TEXT( "%s"), *stateName);
	
	for (int i = 0; i < ResourcesCount_ + ActiveAbilities_; i++)
	{
		if (StateComponentsPtr_[i] == nullptr)
			continue;

		FString temp = StateComponentsPtr_[i]->GetInfo();

		if (!temp.IsEmpty())
		{
			text += TEXT("\n") + temp;
			text += TEXT("\n");
			text += TEXT("\n");
		}
	}

	return FText::FromString(text);
}

void APFPlayerCharacter::ChangeState(TSubclassOf<UPFState> newState)
{
	if (!newState)
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerCharacter] invalid new state when trying to change state"));
		return;
	}

	if (CurrentStatePtr_)
	{
		CurrentStatePtr_->OnExit();
		DeactivateAllAbilityComponents();
	}

	CurrentStatePtr_ = NewObject<UPFState>(this, newState);
	CurrentStatePtr_->Owner = this;
	CurrentStatePtr_->OnEnter();
}

FName APFPlayerCharacter::GetCurrentStateName() const
{
	if (!CurrentStatePtr_)
		return "NONE";

	return CurrentStatePtr_.GetFName();
}

UPFStateComponent* APFPlayerCharacter::GetStateComponent(TSubclassOf<UPFStateComponent> componentClass, int& outIndex)
{
	outIndex = -1;

	if (int* foundIndex = ComponentIndexMap_.Find(componentClass))
	{
		outIndex = *foundIndex;
		return StateComponentsPtr_[outIndex];
	}

	outIndex = -1;
	UE_LOG(LogTemp, Error, TEXT("[PlayerCharacter] Failed to get component of class: %s"), *componentClass->GetName());
	return nullptr;
}

void APFPlayerCharacter::ActivateAbilityComponent(UPFStateComponent* comp, int index)
{
	if (!comp)
		return;

	if (Cast<UPFResource>(comp))
	{
		return;
	}

	int componentCount = StateComponentsPtr_.Num();
	if (!comp ||
		index < 0 || index >= componentCount ||
		index < ResourcesCount_ + ActiveAbilities_)
		return;

	int targetIndex = ResourcesCount_ + ActiveAbilities_;

	if (index != targetIndex)
	{
		SwapComponents(index, targetIndex);
		index = targetIndex;
	}

	comp->bIsActive = true;
	ActiveAbilities_++;
	comp->ComponentEnable();
}

void APFPlayerCharacter::DeactivateAbilityComponent(UPFStateComponent* comp, int index)
{
	int componentCount = StateComponentsPtr_.Num();
	if (!comp ||
		index < 0 || index >= componentCount ||
		ActiveAbilities_ <= 0 || index >= ResourcesCount_ + ActiveAbilities_)
		return;

	int lastIndex = ResourcesCount_ + ActiveAbilities_ - 1;

	if (index != lastIndex)
	{
		SwapComponents(index, lastIndex);
		index = lastIndex;
	}

	comp->bIsActive = false;
	comp->ComponentDisable();
	ActiveAbilities_--;
}

void APFPlayerCharacter::DeactivateAllAbilityComponents()
{
	for (int i = ActiveAbilities_ - 1; i >= 0; i--)
		StateComponentsPtr_[ResourcesCount_ + i]->ComponentDisable();

	ActiveAbilities_ = 0;
}

void APFPlayerCharacter::SwapComponents(int a, int b)
{
	if (a == b) return;

	auto GetNativeClass = [](UClass* nativeclass)
	{
		while (nativeclass && Cast<UBlueprintGeneratedClass>(nativeclass))
			nativeclass = nativeclass->GetSuperClass();
		return nativeclass;
	};

	ComponentIndexMap_[GetNativeClass(StateComponentsPtr_[a]->GetClass())] = b;
	ComponentIndexMap_[GetNativeClass(StateComponentsPtr_[b]->GetClass())] = a;

	StateComponentsPtr_.Swap(a, b);
}

void APFPlayerCharacter::OnInputAction(const FInputActionInstance& instance)
{
	const UInputAction* action = instance.GetSourceAction();
	const FInputActionValue& value = instance.GetValue();
	const ETriggerEvent triggerEvent = instance.GetTriggerEvent();

	if (!action || !CurrentStatePtr_) return;

	CurrentStatePtr_->OnInputTriggered(action->GetFName(), triggerEvent, value);
}
