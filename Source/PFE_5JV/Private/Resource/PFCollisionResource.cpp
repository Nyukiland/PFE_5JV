#include "Resource/PFCollisionResource.h"

#include "State/PFAfterCollisionState.h"
#include "StateMachine/PFPlayerCharacter.h"

bool UPFCollisionResource::RewindAfterCollision(float deltaTime)
{
	PhysicResource_->SetKinematic(true);
	PhysicResource_->StopAllMotion();

	FStoredCollisionInfo stored = StoredCollisionInfoList_[0];
	float time = TimeSavedList_[0];
	StoredCollisionInfoList_.RemoveAt(0);
	TimeSavedList_.RemoveAt(0);
	
	PhysicRoot->SetWorldLocationAndRotation(stored.Position, stored.PhysicRotation,
		false, nullptr, ETeleportType::TeleportPhysics);
	PhysicResource_->HardSetPitchRotationVisual(stored.Pitch);

	if (TimeSavedOnImpact_ - time > DataPtr_->DurationOfRewind
		|| StoredCollisionInfoList_.Num() == 0)
	{
		PhysicResource_->SetKinematic(false);
		PhysicResource_->AddForwardVelocity(stored.ForwardForce * DataPtr_->SlowPercentageAfterSideCollision, false);
		PhysicResource_->AddVelocity(stored.GlobalForce * DataPtr_->SlowPercentageAfterSideCollision, false);
		TimeSavedOnImpact_ = 0;
		return true;
	}

	return false;
}

void UPFCollisionResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResource_ = Owner->GetStateComponent<UPFPhysicResource>();
	DiveAbility_ = Owner->GetStateComponent<UPFDiveAbility>();

	OwnerWorld = Owner->GetWorld();

	PhysicRoot->SetNotifyRigidBodyCollision(true);

	PhysicRoot->OnComponentHit.AddDynamic(
		this,
		&UPFCollisionResource::OnHit);

	if (DataPtr_ && DataPtr_->bUseRollBackOnFrontalCollision)
	{
		OnHardCollision.AddDynamic(
			this,
			&UPFCollisionResource::OnHardCollisionEventCalled);
	}
}

void UPFCollisionResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	RecordInfoForRollBack(deltaTime);
	RecordInfoForPlayTest();
}

void UPFCollisionResource::RecordInfoForRollBack(float deltaTime)
{
	if (!DataPtr_ || !PhysicResource_ || !DiveAbility_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}

	if (!DataPtr_->bUseRollBackOnFrontalCollision)
		return;

	if (!bCanRecord_ ||
		TimeSavedOnImpact_ != 0)
		return;

	float forwardVelo = PhysicResource_->CurrentForwardVelocity_.Length();
	FVector position = PhysicRoot->GetComponentLocation();

	// Store info for the collision
	FStoredCollisionInfo collisionInfo =
		FStoredCollisionInfo(forwardVelo,
							PhysicResource_->CurrentGlobalVelocity_,
							position, PhysicRoot->GetComponentRotation(),
							DiveAbility_->CurrentMedianValue_);

	float time = TimeSavedList_.Num() == 0 ? 0 : TimeSavedList_[0] + deltaTime;
	TimeSavedList_.Insert(time, 0);
	StoredCollisionInfoList_.Insert(collisionInfo, 0);

	//Test to remove the useless info
	if (time - TimeSavedList_.Last() > DataPtr_->DurationOfRemember)
	{
		TimeSavedList_.RemoveAt(TimeSavedList_.Num() - 1);
		StoredCollisionInfoList_.RemoveAt(StoredCollisionInfoList_.Num() - 1);
	}
}

void UPFCollisionResource::RecordInfoForPlayTest()
{
	if (!DataPtr_ || !PhysicResource_ || !DiveAbility_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}

	if (!bCanRecord_)
		return;

	float forwardVelo = PhysicResource_->CurrentForwardVelocity_.Length();
	FVector position = PhysicRoot->GetComponentLocation();

	FStoredPlaytestInfo PlaytestInfo =
		FStoredPlaytestInfo(forwardVelo,
							position);

	GameInfoList_.Add(PlaytestInfo);
}

void UPFCollisionResource::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								FVector NormalImpulse, const FHitResult& Hit)
{
	if (!DataPtr_ || !PhysicResource_)
		return;

	FVector normalOfCollider = Hit.ImpactNormal;
	FVector playerMovement = PhysicResource_->GetCurrentVelocity();
	float dotCollision = FVector::DotProduct(normalOfCollider, playerMovement.GetSafeNormal());

	if (FMath::Abs(dotCollision) > DataPtr_->ThresholdCollision)
	{
		TimeSavedOnImpact_ = OwnerWorld->GetTimeSeconds();
		OnHardCollision.Broadcast();
		return;
	}

	OnSoftCollision.Broadcast();

	FVector mirroredMovement = playerMovement.MirrorByVector(normalOfCollider);
	FRotator LookAtRotation = mirroredMovement.GetSafeNormal().Rotation();

	FRotator YawOnlyRotation(0.f, LookAtRotation.Yaw, 0.f);
	PhysicRoot->SetWorldRotation(YawOnlyRotation);

	PhysicResource_->CurrentForwardVelocity_ *= DataPtr_->SlowPercentageAfterSideCollision;
}

void UPFCollisionResource::OnHardCollisionEventCalled()
{
	Owner->ChangeState(UPFAfterCollisionState::StaticClass());
}
