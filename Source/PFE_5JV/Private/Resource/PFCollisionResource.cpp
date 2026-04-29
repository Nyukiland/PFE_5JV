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

	CheckFlank();
	CheckPredictiveCollision(deltaTime);
	RecordInfoForRollBack(deltaTime);
	RecordInfoForPlayTest();
}

bool UPFCollisionResource::IsHardCollision(const FVector& impactNormal, const FVector& currentVelocity) const
{
	if (!DataPtr_) 
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return false;
	}

	float impactSeverity = FVector::DotProduct(currentVelocity.GetSafeNormal(), -impactNormal);
	return impactSeverity > DataPtr_->ThresholdHorizontalCollision;
}

void UPFCollisionResource::HandleSoftCollision(const FVector& impactNormal, const FVector& currentVelocity)
{
	if (!DataPtr_) 
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}
	
	OnSoftCollision.Broadcast();

	FVector projectedMovement = currentVelocity.MirrorByVector(impactNormal);
	if (!projectedMovement.IsNearlyZero())
	{
		FRotator lookAtRotation = projectedMovement.GetSafeNormal().Rotation();
		FRotator yawOnlyRotation(0.f, lookAtRotation.Yaw, 0.f);
		PhysicRoot->SetWorldRotation(yawOnlyRotation);
	}

	PhysicResource_->CurrentForwardVelocity_ *= DataPtr_->SlowPercentageAfterSideCollision;
}

void UPFCollisionResource::CheckFlank()
{
	if (!PhysicResource_ || !DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad Set up"))
		return;
	}

	float flankTraceLength = DataPtr_->PreshotSphereSize + 50.f; 
	FVector startPos = PhysicRoot->GetComponentLocation();
    
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	bool bHitRight = OwnerWorld->LineTraceTestByChannel(startPos, startPos + (PhysicRoot->GetRightVector() * flankTraceLength), ECC_WorldStatic, QueryParams);
	bool bHitLeft  = OwnerWorld->LineTraceTestByChannel(startPos, startPos + (-PhysicRoot->GetRightVector() * flankTraceLength), ECC_WorldStatic, QueryParams);
	bool bHitUp    = OwnerWorld->LineTraceTestByChannel(startPos, startPos + (PhysicRoot->GetUpVector() * flankTraceLength), ECC_WorldStatic, QueryParams);
	bool bHitDown  = OwnerWorld->LineTraceTestByChannel(startPos, startPos + (-PhysicRoot->GetUpVector() * flankTraceLength), ECC_WorldStatic, QueryParams);

	PhysicResource_->SetDirectionalBlocks(bHitRight, bHitLeft, bHitUp, bHitDown);
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
	float collisionDist = velocity.Length() * deltaTime * DataPtr_->SweepAnticipation;
	float avoidanceDist = velocity.Length() * deltaTime * DataPtr_->AvoidanceAnticipationMultiplier;
	FVector endPos = startPos + (velocity.GetSafeNormal() * avoidanceDist);

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Owner);
	FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->PreshotSphereSize);

	if (OwnerWorld->SweepSingleByChannel(hitResult,	startPos, endPos, FQuat::Identity, 
		ECC_WorldStatic, sweepShape, queryParams))
	{
		PhysicResource_->SetAvoidanceNormal(hitResult.ImpactNormal);

		if (hitResult.Distance <= collisionDist)
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
		else
		{
			float distanceFactor = FMath::Clamp(1.0f - (hitResult.Distance / avoidanceDist), 0.0f, 1.0f);
			ApplyProgressiveSteering(hitResult, distanceFactor, deltaTime);
		}
	}
	else
	{
		PhysicResource_->SetAvoidanceNormal(FVector::ZeroVector);
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

	FStoredPlaytestInfo playtestInfo =
		FStoredPlaytestInfo(forwardVelo,
							position);

	GameInfoList_.Add(playtestInfo);
}

void UPFCollisionResource::ApplyProgressiveSteering(const FHitResult& hit, float distanceFactor, float deltaTime)
{
	if (!DataPtr_ || !PhysicResource_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}

	FVector velocity = PhysicResource_->GetCurrentVelocity();
	FVector velNormal = velocity.GetSafeNormal();
    
	float impactSeverity = FMath::Max(0.f, FVector::DotProduct(velNormal, -hit.ImpactNormal));
	float speedFactor = FMath::Clamp(velocity.Length() / PhysicResource_->GetMaxBoostVelocity(), 0.1f, 1.0f);

	FVector escapeDir;
	if (impactSeverity > 0.9f)
	{
		FVector rightDir = PhysicRoot->GetRightVector();
		float yawBias = FVector::DotProduct(velNormal, rightDir);
		escapeDir = (yawBias >= 0.f) ? rightDir : -rightDir;
	}
	else
	{
		escapeDir = FindBestEvasionDirection(PhysicRoot->GetComponentLocation(), velNormal, hit.ImpactNormal);
	}

	float turnRate = DataPtr_->BaseAvoidanceTurnRate * speedFactor * distanceFactor * (1.0f + impactSeverity);
    
	FRotator CurrentRot = PhysicRoot->GetComponentRotation();
	FRotator NewRot = FMath::RInterpTo(CurrentRot, escapeDir.Rotation(), deltaTime, turnRate);
    
	PhysicRoot->SetWorldRotation(NewRot);
	PhysicResource_->HardSetPitchRotationVisual(NewRot.Pitch);

	float currentSlowdown = FMath::Lerp(1.0f, DataPtr_->SlowPercentageDuringAvoidance, distanceFactor);
	PhysicResource_->CurrentForwardVelocity_ *= currentSlowdown;
}

FVector UPFCollisionResource::FindBestEvasionDirection(const FVector& startPos, const FVector& currentVelocityNormal,
	const FVector& impactNormal)
{
	TArray<FVector> testDirections = {
		currentVelocityNormal.RotateAngleAxis(45.f, PhysicRoot->GetRightVector()),   
		currentVelocityNormal.RotateAngleAxis(-45.f, PhysicRoot->GetRightVector()),  
		currentVelocityNormal.RotateAngleAxis(45.f, PhysicRoot->GetUpVector()),      
		currentVelocityNormal.RotateAngleAxis(-45.f, PhysicRoot->GetUpVector()),     
		currentVelocityNormal.RotateAngleAxis(45.f, PhysicRoot->GetRightVector() + PhysicRoot->GetUpVector()) 
	};

	FVector bestDirection = FVector::ZeroVector;
	float bestScore = -MAX_flt;

	float avoidanceDistance = DataPtr_->AvoidanceAnticipationMultiplier * PhysicResource_->GetCurrentVelocity().Length() * 0.1f;
	avoidanceDistance = FMath::Max(avoidanceDistance, 1000.f); 

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Owner);
	FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->PreshotSphereSize * 0.8f);

	for (const FVector& dir : testDirections)
	{
		FVector endPos = startPos + (dir * avoidanceDistance);
		FHitResult hit;
        
		bool bHit = OwnerWorld->SweepSingleByChannel(hit, startPos, endPos, FQuat::Identity,
			ECC_WorldStatic, sweepShape, queryParams);

		if (!bHit)
		{
			float flowScore = FVector::DotProduct(dir, currentVelocityNormal);
			float escapeScore = FVector::DotProduct(dir, impactNormal);
			float totalScore = flowScore + escapeScore;

			if (totalScore > bestScore)
			{
				bestScore = totalScore;
				bestDirection = dir;
			}
		}
	}

	if (bestDirection.IsNearlyZero())
	{
		bestDirection = currentVelocityNormal.MirrorByVector(impactNormal);
	}

	return bestDirection.GetSafeNormal();
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
