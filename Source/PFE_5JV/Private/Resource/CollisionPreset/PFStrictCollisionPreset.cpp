#include "Resource/CollisionPreset/PFStrictCollisionPreset.h"

#include "Resource/PFCollisionResource.h"
#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFStrictCollisionPreset::InitPreset_Implementation(UPFCollisionResource* collisionResource)
{
	Super::InitPreset_Implementation(collisionResource);

	PhysicResourcePtr_ = OwnerPtr_->GetStateComponent<UPFPhysicResource>();
}

void UPFStrictCollisionPreset::OnHitReaction_Implementation(const FHitResult& hit)
{
	Super::OnHitReaction_Implementation(hit);

	if (!CollisionResourcePtr_ || !PhysicResourcePtr_ || !DataPtr_->CollisionOffset)
	{
		UE_LOG(LogTemp, Error, TEXT("[StrictCollisionPreset] Missing resource references."));
		return;
	}

	FVector currentVelocity = PhysicResourcePtr_->GetCurrentVelocity();

	// Offset
	FVector safeLocation = CollisionResourcePtr_->PhysicRoot->GetComponentLocation() + (hit.ImpactNormal * DataPtr_->CollisionOffset);
	CollisionResourcePtr_->PhysicRoot->SetWorldLocation(safeLocation, false, nullptr, ETeleportType::TeleportPhysics);

	// Redirection
	FVector reflectedVelocity = currentVelocity.MirrorByVector(hit.ImpactNormal);
	FRotator bounceRotation = reflectedVelocity.GetSafeNormal().Rotation();

	FRotator yawOnlyRotation = FRotator(0.f, bounceRotation.Yaw, 0.f);
	PhysicResourcePtr_->PhysicRoot->SetWorldRotation(yawOnlyRotation);
	
	PhysicResourcePtr_->HardSetPitchRotationVisual(bounceRotation.Pitch);

	// Slow
	PhysicResourcePtr_->AddForwardVelocity(DataPtr_->SlowOnCollision, true, true);
}
