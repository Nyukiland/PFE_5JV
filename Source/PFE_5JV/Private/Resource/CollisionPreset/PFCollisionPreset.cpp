#include "Resource/CollisionPreset/PFCollisionPreset.h"

#include "Resource/PFCollisionResource.h"

void UPFCollisionPreset::InitPreset_Implementation(UPFCollisionResource* collisionResource)
{
	CollisionResourcePtr_ = collisionResource;
	OwnerPtr_ = CollisionResourcePtr_->Owner;
}

void UPFCollisionPreset::OnHitReaction_Implementation(const FHitResult& hit)
{
	
}

void UPFCollisionPreset::OnTickActions_Implementation(float deltaTime)
{
	
}
