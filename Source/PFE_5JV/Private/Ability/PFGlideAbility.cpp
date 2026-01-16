#include "Ability/PFGlideAbility.h"

#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFGlideAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResource_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));
}

void UPFGlideAbility::ConstantMovement(float deltaTime)
{
	if (!DataPtr_ || !PhysicResource_)
	{
		UE_LOG(LogTemp, Error, TEXT("UPFGlideAbility::ConstantMovement"));
		return;
	}
	
	PhysicResource_->AddForwardForce(DataPtr_->BaseForwardMovement);
}
