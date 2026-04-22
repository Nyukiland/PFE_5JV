#include "Resource/PFCollisionResource.h"

#include "State/PFAfterCollisionState.h"
#include "StateMachine/PFPlayerCharacter.h"

bool UPFCollisionResource::RewindAfterCollision(float deltaTime)
{
	if (!DataPtr_) 
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return false;
	}
	
	if (StoredCollisionInfoList_.Num() == 0) 
		return false;

	PhysicResource_->SetKinematic(true);
	PhysicResource_->StopAllMotion();

	FStoredCollisionInfo stored = StoredCollisionInfoList_.Last();
	float time = TimeSavedList_.Last();
    
	StoredCollisionInfoList_.Pop(false);
	TimeSavedList_.Pop(false);
    
	PhysicRoot->SetWorldLocationAndRotation(stored.Position, stored.PhysicRotation,
	   false, nullptr, ETeleportType::TeleportPhysics);
	PhysicResource_->HardSetPitchRotationVisual(stored.Pitch);

	if (TimeSavedOnImpact_ - time > DataPtr_->DurationOfRewind || StoredCollisionInfoList_.Num() == 0)
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
	PhysicRoot->BodyInstance.bUseCCD = true;
	
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

	CheckPredictiveCollision(deltaTime);

	RecordInfoForRollBack(deltaTime);
	RecordInfoForPlayTest();
}

bool UPFCollisionResource::IsHardCollision(const FVector& ImpactNormal, const FVector& CurrentVelocity) const
{
	if (!DataPtr_) 
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return false;
	}

	float ImpactSeverity = FVector::DotProduct(CurrentVelocity.GetSafeNormal(), -ImpactNormal);

	return ImpactSeverity > DataPtr_->ThresholdHorizontalCollision;
}

void UPFCollisionResource::HandleSoftCollision(const FVector& ImpactNormal, const FVector& CurrentVelocity)
{
	if (!DataPtr_) 
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}
	
	OnSoftCollision.Broadcast();

	FVector ProjectedMovement = FVector::VectorPlaneProject(CurrentVelocity, ImpactNormal);

	if (!ProjectedMovement.IsNearlyZero())
	{
		FRotator LookAtRotation = ProjectedMovement.GetSafeNormal().Rotation();
        
		FRotator YawOnlyRotation(0.f, LookAtRotation.Yaw, 0.f);
		PhysicRoot->SetWorldRotation(YawOnlyRotation);
	}

	PhysicResource_->CurrentForwardVelocity_ *= DataPtr_->SlowPercentageAfterSideCollision;
}

void UPFCollisionResource::CheckPredictiveCollision(float deltaTime)
{
	if (!PhysicResource_ || !DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad Set up"))
		return;
	}

	FVector velocity = PhysicResource_->GetCurrentVelocity();
	
	if (velocity.IsNearlyZero() || TimeSavedOnImpact_ != 0)
		return;

	FVector startPos = PhysicRoot->GetComponentLocation();
	FVector endPos = startPos + (velocity * deltaTime * DataPtr_->SweepAnticipation); 

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Owner);
	FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->PreshotSphereSize);

	if (OwnerWorld->SweepSingleByChannel(hitResult,	startPos, endPos, FQuat::Identity, 
		ECC_WorldStatic, sweepShape, queryParams))
	{
		if (IsHardCollision(hitResult.ImpactNormal, velocity))
		{
			TimeSavedOnImpact_ = TimeSavedList_.Last();
			OnHardCollision.Broadcast();
		}
		else
		{
			HandleSoftCollision(hitResult.ImpactNormal, velocity);
		}
	}
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
    
	if (!bCanRecord_ || TimeSavedOnImpact_ != 0) 
		return;

	float forwardVelo = PhysicResource_->CurrentForwardVelocity_.Length();
	FVector position = PhysicRoot->GetComponentLocation();

	FStoredCollisionInfo collisionInfo =
	   FStoredCollisionInfo(forwardVelo,
					  PhysicResource_->CurrentGlobalVelocity_,
					  position, PhysicRoot->GetComponentRotation(),
					  DiveAbility_->CurrentMedianValue_);

	float time = TimeSavedList_.Num() == 0 ? 0.f : TimeSavedList_.Last() + deltaTime;
	TimeSavedList_.Add(time);
	StoredCollisionInfoList_.Add(collisionInfo);

	while (TimeSavedList_.Num() > 0 && (time - TimeSavedList_[0] > DataPtr_->DurationOfRemember))
	{
		TimeSavedList_.RemoveAt(0, 1, false);
		StoredCollisionInfoList_.RemoveAt(0, 1, false);
	}
}

void UPFCollisionResource::RecordInfoForPlayTest()
{
	if (!DataPtr_ || !PhysicResource_)
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
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}

	FVector Velocity = PhysicResource_->GetCurrentVelocity();

	if (IsHardCollision(Hit.ImpactNormal, Velocity))
	{
		TimeSavedOnImpact_ = TimeSavedList_.Last();
		OnHardCollision.Broadcast();
	}
	else
	{
		HandleSoftCollision(Hit.ImpactNormal, Velocity);
	}
}

void UPFCollisionResource::OnHardCollisionEventCalled()
{
	if (!DataPtr_ || !DataPtr_->AfterCollisionState)
	{
		return;
	}
	
	Owner->ChangeState(DataPtr_->AfterCollisionState);
}
