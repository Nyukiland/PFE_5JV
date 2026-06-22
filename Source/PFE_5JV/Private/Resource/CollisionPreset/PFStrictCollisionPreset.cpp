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

	if (!CollisionResourcePtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[StrictCollisionPreset] Missing resource references."));
		return;
	}

	FVector currentVelocity = PhysicResourcePtr_->GetCurrentVelocity();

	if (currentVelocity.IsNearlyZero(10.f))
	{
		return;
	}

	FVector reflectedVelocity = currentVelocity.MirrorByVector(hit.ImpactNormal);

	FRotator bounceRotation = reflectedVelocity.GetSafeNormal().Rotation();

	FRotator yawOnlyRotation = FRotator(0.f, bounceRotation.Yaw, 0.f);
	PhysicResourcePtr_->PhysicRoot->SetWorldRotation(yawOnlyRotation);

	FRotator pitchOnlyRotation = FRotator(bounceRotation.Pitch, 0.f, 0.f);
	PhysicResourcePtr_->ForwardRootPtr->SetRelativeRotation(pitchOnlyRotation);

	PhysicResourcePtr_->HardSetPitchRotationVisual(bounceRotation.Pitch);
}
