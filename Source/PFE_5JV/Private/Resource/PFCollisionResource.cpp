#include "Resource/PFCollisionResource.h"

#include "State/PFAfterCollisionState.h"
#include "StateMachine/PFPlayerCharacter.h"

UPFCollisionResource::UPFCollisionResource()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

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

bool UPFCollisionResource::HasHitDirection(ERayDir Direction) const
{
	uint8 index = static_cast<uint8>(Direction);
    
	if (index < 9) 
	{
		return RayDirHits_[index];
	}
    
	UE_LOG(LogTemp, Warning, TEXT("[CollisionResource] Invalid Ray Direction requested!"));
	return false;
}

FString UPFCollisionResource::GetInfo_Implementation()
{
	FString text = TEXT("<hb>Collision:</>");
	text += TEXT("\n <b>Hit Right: </>") + FString::Printf(TEXT("%i"), bHitRight);
	text += TEXT("\n <b>Hit Left: </>") + FString::Printf(TEXT("%i"), bHitLeft);
	text += TEXT("\n <b>Hit Up: </>") + FString::Printf(TEXT("%i"), bHitUp);
	text += TEXT("\n <b>Hit Down: </>") + FString::Printf(TEXT("%i"), bHitDown);
	return text;
}

void UPFCollisionResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResource_ = Owner->GetStateComponent<UPFPhysicResource>();

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

void UPFCollisionResource::TickComponent(float deltaTime, enum ELevelTick tickType,
										FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	CheckFlank(deltaTime);
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

void UPFCollisionResource::CheckFlank(float deltaTime)
{
	if (!PhysicResource_ || !DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad Set up"))
		return;
	}

	float flankTraceLength = DataPtr_->FlankDetectionDist;
	FVector startPos = PhysicRoot->GetComponentLocation() +
	(PhysicResource_->CurrentForwardVelocity_.Length() * DataPtr_->FlankPredictionDist * ForwardRootPtr_->
		GetForwardVector());

	FVector rightEnd = startPos + (ForwardRootPtr_->GetRightVector() * flankTraceLength);
	FVector leftEnd = startPos + (-ForwardRootPtr_->GetRightVector() * flankTraceLength);
	FVector upEnd = startPos + (ForwardRootPtr_->GetUpVector() * flankTraceLength);
	FVector downEnd = startPos + (-ForwardRootPtr_->GetUpVector() * flankTraceLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	bHitRight = OwnerWorld->LineTraceTestByChannel(startPos, rightEnd, ECC_WorldStatic, QueryParams);
	bHitLeft = OwnerWorld->LineTraceTestByChannel(startPos, leftEnd, ECC_WorldStatic, QueryParams);
	bHitUp = OwnerWorld->LineTraceTestByChannel(startPos, upEnd, ECC_WorldStatic, QueryParams);
	bHitDown = OwnerWorld->LineTraceTestByChannel(startPos, downEnd, ECC_WorldStatic, QueryParams);

#if !UE_BUILD_SHIPPING
	if (DataPtr_->bShowPredictiveDebug && OwnerWorld)
	{
		DrawDebugSphere(OwnerWorld, startPos, 15.f, 8, FColor::Cyan, false, -1.f);

		DrawDebugLine(OwnerWorld, startPos, rightEnd, bHitRight ? FColor::Red : FColor::Green, false, -1.f, 0, 2.f);
		DrawDebugLine(OwnerWorld, startPos, leftEnd, bHitLeft ? FColor::Red : FColor::Green, false, -1.f, 0, 2.f);
		DrawDebugLine(OwnerWorld, startPos, upEnd, bHitUp ? FColor::Red : FColor::Green, false, -1.f, 0, 2.f);
		DrawDebugLine(OwnerWorld, startPos, downEnd, bHitDown ? FColor::Red : FColor::Green, false, -1.f, 0, 2.f);
	}
#endif
}

void UPFCollisionResource::CheckPredictiveCollision(float deltaTime)
{
	if (!PhysicResource_ || !DataPtr_
		|| !DataPtr_->AssistTurnCurve
		|| !DataPtr_->AssistForceCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad Set up"))
		return;
	}

	FVector velocity = PhysicResource_->GetCurrentVelocity();

	if (velocity.IsNearlyZero() || TimeSavedOnImpact_ != 0)
		return;

	float speedPercentage = PhysicResource_->GetForwardVelocityPercentage();
	float speedMultiplier = FMath::Lerp(DataPtr_->MinSpeedMultiplier, DataPtr_->MaxSpeedMultiplier, speedPercentage);

	float dynamicAssistDistance = DataPtr_->AssistDistance * speedMultiplier;
	float dynamicAssistDistanceSides = DataPtr_->AssistDistanceSides * speedMultiplier;
	float dynamicAssistDistanceDiagonal = DataPtr_->AssistDistanceDiagonal * speedMultiplier;
	float dynamicAvoidDistance = DataPtr_->AvoidDistance * speedMultiplier;

	FVector startPos = PhysicRoot->GetComponentLocation();
	FVector forwardDir = ForwardRootPtr_->GetForwardVector();
	FVector rightDir = ForwardRootPtr_->GetRightVector();
	FVector upDir = ForwardRootPtr_->GetUpVector();

	float angle = DataPtr_->ConeAngle;

	FVector rayDirs[9] =
	{
		forwardDir,
		forwardDir.RotateAngleAxis(angle, rightDir),
		forwardDir.RotateAngleAxis(-angle, rightDir),
		forwardDir.RotateAngleAxis(angle, upDir),
		forwardDir.RotateAngleAxis(-angle, upDir),
		forwardDir.RotateAngleAxis(angle, rightDir + upDir),
		forwardDir.RotateAngleAxis(angle, -rightDir + upDir),
		forwardDir.RotateAngleAxis(-angle, rightDir + upDir),
		forwardDir.RotateAngleAxis(-angle, -rightDir + upDir)
	};

	float rayDist[9] =
	{
		dynamicAssistDistance,
		dynamicAssistDistanceSides,
		dynamicAssistDistanceSides,
		dynamicAssistDistanceSides,
		dynamicAssistDistanceSides,
		dynamicAssistDistanceDiagonal,
		dynamicAssistDistanceDiagonal,
		dynamicAssistDistanceDiagonal,
		dynamicAssistDistanceDiagonal
	};

	FVector totalRepulsion = FVector::ZeroVector;
	FVector firstOpenDir = FVector::ZeroVector;

	float closestDistance = MAX_flt;
	bool bHitCenter = false;
	int openRayCount = 0;

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Owner);
	FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->PreshotSphereSize * 0.5f);

	for (int i = 0; i < 9; ++i)
	{
		FHitResult hit;
		float assistDistance = rayDist[i];
		FVector endPos = startPos + (rayDirs[i] * assistDistance);
		bool bHit = false;

		if (i == 0)
		{
			bHit = OwnerWorld->SweepSingleByChannel(hit, startPos, endPos, FQuat::Identity, ECC_WorldStatic, sweepShape,
													queryParams);
		}
		else
		{
			bHit = OwnerWorld->LineTraceSingleByChannel(hit, startPos, endPos, ECC_WorldStatic, queryParams);
		}

		DrawDebugWhiskerCone(startPos, endPos, bHit, hit);

		RayDirHits_[i] = bHit;
		
		if (!bHit)
		{
			if (openRayCount == 0) firstOpenDir = rayDirs[i];
			openRayCount++;
			continue;
		}

		if (i == 0) bHitCenter = true;
		if (hit.Distance < closestDistance) closestDistance = hit.Distance;

		float weight = 1.0f - (hit.Distance / assistDistance);
		totalRepulsion += hit.ImpactNormal * weight;
	}

	if (closestDistance == MAX_flt)
	{
		CurrentRepulsion_ = FMath::VInterpTo(CurrentRepulsion_, FVector::ZeroVector, deltaTime,
											DataPtr_->SmoothingTurn);
		bIsInHardAvoid_ = false;
		return;
	}

	FVector combinedSteering = forwardDir + totalRepulsion;

	if (bHitCenter && totalRepulsion.Length() < 0.2f)
	{
		if (openRayCount > 0)
		{
			combinedSteering = firstOpenDir;
		}
		else
		{
			if (!bHitRight) combinedSteering = rightDir;
			else if (!bHitUp) combinedSteering = upDir;
			else if (!bHitLeft) combinedSteering = -rightDir;
			else combinedSteering = -upDir;
		}
	}

	combinedSteering.Normalize();

	CurrentRepulsion_ = FMath::VInterpTo(CurrentRepulsion_, combinedSteering, deltaTime, DataPtr_->SmoothingTurn);
	float avoidExitThreshold = dynamicAvoidDistance * 1.15f;

	if (closestDistance < DataPtr_->AvoidDistance)
	{
		bIsInHardAvoid_ = true;
	}
	else if (closestDistance > avoidExitThreshold)
	{
		bIsInHardAvoid_ = false;
	}

	if (bIsInHardAvoid_)
	{
		PhysicResource_->AddForwardVelocity(-DataPtr_->SlowForceAvoid * deltaTime, false);

		PhysicResource_->AddVelocity(CurrentRepulsion_ * DataPtr_->AvoidForce);

		float targetPitchOffset = FMath::FindDeltaAngleDegrees(PhysicRoot->GetComponentRotation().Pitch,
																CurrentRepulsion_.Rotation().Pitch);
		PhysicResource_->AddAssistPitch(targetPitchOffset * DataPtr_->AvoidForceRot);

		float targetYawOffset = FMath::FindDeltaAngleDegrees(PhysicRoot->GetComponentRotation().Yaw,
															CurrentRepulsion_.Rotation().Yaw);
		PhysicResource_->SetYawRotationVelocity(targetYawOffset * DataPtr_->AvoidForceRot);
		return;
	}

	// Assist Logic
	float distanceRatio = (closestDistance - DataPtr_->AvoidDistance) / (DataPtr_->AssistDistance - DataPtr_->
		AvoidDistance);
	float intensity = FMath::Clamp(1.0f - distanceRatio, 0.0f, 1.0f);

	PhysicResource_->AddForwardVelocity(-DataPtr_->SlowForceAssist * deltaTime, false);

	float forceMultiplier = DataPtr_->AssistForceCurve->GetFloatValue(intensity);
	PhysicResource_->AddVelocity(CurrentRepulsion_ * (DataPtr_->AssistForce * forceMultiplier * speedMultiplier));

	float turnMultiplier = DataPtr_->AssistTurnCurve->GetFloatValue(intensity);

	float targetPitchOffset = FMath::FindDeltaAngleDegrees(PhysicRoot->GetComponentRotation().Pitch,
															CurrentRepulsion_.Rotation().Pitch);
	float pitchNudge = targetPitchOffset * turnMultiplier * DataPtr_->AssistTurnSpeed * speedMultiplier;
	PhysicResource_->AddAssistPitch(pitchNudge);

	float targetYawOffset = FMath::FindDeltaAngleDegrees(PhysicRoot->GetComponentRotation().Yaw,
														CurrentRepulsion_.Rotation().Yaw);
	PhysicResource_->SetYawRotationVelocity(
		targetYawOffset * turnMultiplier * DataPtr_->AssistTurnSpeed * speedMultiplier);
}

void UPFCollisionResource::DrawDebugWhiskerCone(const FVector& StartPos, const FVector& EndPos,
												bool bHit, const FHitResult& HitResult)
{
#if !UE_BUILD_SHIPPING
	if (!DataPtr_ || !DataPtr_->bShowPredictiveDebug || !OwnerWorld)
		return;

	FColor RayColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(OwnerWorld, StartPos, EndPos, RayColor, false, -1.f, 0, 2.f);

	if (bHit)
	{
		DrawDebugPoint(OwnerWorld, HitResult.ImpactPoint, 10.f, FColor::Red, false, -1.f);
		DrawDebugLine(OwnerWorld, HitResult.ImpactPoint, HitResult.ImpactPoint + (HitResult.ImpactNormal * 150.f),
					FColor::Yellow, false, -1.f, 0, 3.f);
	}
#endif
}

void UPFCollisionResource::RecordInfoForRollBack(float deltaTime)
{
	if (!DataPtr_ || !PhysicResource_)
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
							position, PhysicRoot->GetComponentRotation());

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
