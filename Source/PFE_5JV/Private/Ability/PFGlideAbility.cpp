#include "Ability/PFGlideAbility.h"

#include "Resource/PFPhysicResource.h"

void UPFGlideAbility::ConstantMovement(float deltaTime)
{
	if (!DataPtr_ || !PhysicResource_)
	{
		UE_LOG(LogTemp, Error, TEXT("UPFGlideAbility::ConstantMovement"));
		return;
	}
	
	FVector forward = Root->GetForwardVector();
	PhysicResource_->AddForce(deltaTime * DataPtr_->BaseForwardMovement * forward);
}
