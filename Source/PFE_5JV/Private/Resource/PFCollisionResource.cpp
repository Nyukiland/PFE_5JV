#include "Resource/PFCollisionResource.h"

#include "Ability/PFDiveAbility.h"
#include "Ability/PFTurnAbility.h"
#include "Ability/PFWingBeatAbility.h"
#include "Resource/PFPhysicResource.h"
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

	PhysicResourcePtr_->SetKinematic(true);
	PhysicResourcePtr_->StopAllMotion();

	FStoredCollisionInfo stored = StoredCollisionInfoList_.Last();
	float time = TimeSavedList_.Last();

	StoredCollisionInfoList_.Pop(false);
	TimeSavedList_.Pop(false);

	PhysicRoot->SetWorldLocationAndRotation(stored.Position, stored.PhysicRotation,
											false, nullptr, ETeleportType::TeleportPhysics);

	if (TimeSavedOnImpact_ - time > DataPtr_->DurationOfRewind || StoredCollisionInfoList_.Num() == 0)
	{
		PhysicResourcePtr_->SetKinematic(false);
		PhysicResourcePtr_->AddForwardVelocity(stored.ForwardForce * DataPtr_->SlowPercentageAfterSideCollision, false);
		PhysicResourcePtr_->AddVelocity(stored.GlobalForce * DataPtr_->SlowPercentageAfterSideCollision, false);
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

void UPFCollisionResource::ChangeHelperActive(bool newActive)
{
	bIsHelperActive_ = newActive;
}

bool UPFCollisionResource::GetHelperActive() const
{
	return bIsHelperActive_;
}

void UPFCollisionResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = Owner->GetStateComponent<UPFPhysicResource>();
	TurnAbilityPtr_ = Owner->GetStateComponent<UPFTurnAbility>();
	DiveAbilityPtr_ = Owner->GetStateComponent<UPFDiveAbility>();
	WingBeatAbilityPtr_ = Owner->GetStateComponent<UPFWingBeatAbility>();

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

	if (bIsHelperActive_)
	{
		FVector playerDesiredDir = GetPlayerIntendedDirection();
		CheckFlank(deltaTime);
		CheckPredictiveCollision(deltaTime, playerDesiredDir);
	}
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

	PhysicResourcePtr_->CurrentForwardVelocity_ *= DataPtr_->SlowPercentageAfterSideCollision;
}

FVector UPFCollisionResource::GetPlayerIntendedDirection() const
{
	if (!PhysicResourcePtr_ || !TurnAbilityPtr_ || !DiveAbilityPtr_ || !WingBeatAbilityPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] abilities or resources not fetch properly"));
		return ForwardRootPtr_->GetForwardVector();
	}
	
	float TargetPitchAxis = 0.0f;

		float TargetYawAxis = TurnAbilityPtr_->RotationValue_;

	if (WingBeatAbilityPtr_->IsCurrentlyGoingUp())
	{
		TargetPitchAxis = WingBeatAbilityPtr_->GetCurrentWingBeatPercentage();
	}
	else if (DiveAbilityPtr_->IsDiving())
	{
		TargetPitchAxis = -DiveAbilityPtr_->GetDivingValue();
	}

	if (FMath::IsNearlyZero(TargetYawAxis) && FMath::IsNearlyZero(TargetPitchAxis))
	{
		return ForwardRootPtr_->GetForwardVector();
	}

	const float MaxIntentYawAngle = 45.0f;
	const float MaxIntentPitchAngle = 45.0f;

	FRotator IntentLocalRotation = FRotator(-TargetPitchAxis * MaxIntentPitchAngle, TargetYawAxis * MaxIntentYawAngle, 0.0f);
	return IntentLocalRotation.RotateVector(ForwardRootPtr_->GetForwardVector()).GetSafeNormal();
}

void UPFCollisionResource::CheckFlank(float deltaTime)
{
	if (!PhysicResourcePtr_ || !DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad Set up"))
		return;
	}

	float flankTraceLength = DataPtr_->FlankDetectionDist;
	FVector startPos = PhysicRoot->GetComponentLocation() +
	(PhysicResourcePtr_->CurrentForwardVelocity_.Length() * DataPtr_->FlankPredictionDist * ForwardRootPtr_->
		GetForwardVector());

	FVector rightEnd = startPos + (ForwardRootPtr_->GetRightVector() * flankTraceLength);
	FVector leftEnd = startPos + (-ForwardRootPtr_->GetRightVector() * flankTraceLength);
	FVector upEnd = startPos + (ForwardRootPtr_->GetUpVector() * flankTraceLength);
	FVector downEnd = startPos + (-ForwardRootPtr_->GetUpVector() * flankTraceLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	bHitRight = OwnerWorld->LineTraceTestByChannel(startPos, rightEnd, ECC_Visibility, QueryParams);
	bHitLeft = OwnerWorld->LineTraceTestByChannel(startPos, leftEnd, ECC_Visibility, QueryParams);
	bHitUp = OwnerWorld->LineTraceTestByChannel(startPos, upEnd, ECC_Visibility, QueryParams);
	bHitDown = OwnerWorld->LineTraceTestByChannel(startPos, downEnd, ECC_Visibility, QueryParams);

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

void UPFCollisionResource::CheckPredictiveCollision(float deltaTime, const FVector& playerDesiredDir)
{
	if (!PhysicResourcePtr_ || !DataPtr_ || !DataPtr_->AssistTurnCurve
		|| !DataPtr_->AssistForceCurve || !DataPtr_->AssistSpeedMultiplierCurve) return;

	FVector velocity = PhysicResourcePtr_->GetCurrentVelocity();

	if (velocity.IsNearlyZero() || !FMath::IsNearlyZero(TimeSavedOnImpact_))
		return;

	float distanceThisFrame = velocity.Length() * deltaTime;
	float speedPercentage = PhysicResourcePtr_->GetForwardVelocityPercentage();
	speedPercentage = DataPtr_->AssistSpeedMultiplierCurve->GetFloatValue(speedPercentage);
	float speedMultiplier = FMath::Lerp(DataPtr_->MinSpeedMultiplier, DataPtr_->MaxSpeedMultiplier, speedPercentage);

	float dynamicAssistDistance = distanceThisFrame + (DataPtr_->AssistDistance * speedMultiplier);
	float dynamicAssistDistanceSides = distanceThisFrame + (DataPtr_->AssistDistanceSides * speedMultiplier);
	float dynamicAssistDistanceDiagonal = distanceThisFrame + (DataPtr_->AssistDistanceDiagonal * speedMultiplier);
	float dynamicAvoidDistance = distanceThisFrame + (DataPtr_->AvoidDistance * speedMultiplier);

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

	FVector totalRepulsion;
	FVector bestEscapeDir;
	float closestDistance;
	bool bHitCenter;
	int openRayCount;

	FirePredictiveRays(startPos, rayDirs, rayDist, playerDesiredDir, totalRepulsion, bestEscapeDir, closestDistance, bHitCenter, openRayCount);

	if (closestDistance == MAX_flt)
	{
		CurrentRepulsion_ = FMath::VInterpTo(CurrentRepulsion_, FVector::ZeroVector, deltaTime, DataPtr_->SmoothingTurn);
		bIsInHardAvoid_ = false;
		return;
	}

	UpdateSteeringRepulsion(deltaTime, forwardDir, totalRepulsion, bestEscapeDir, bHitCenter);
	ApplyPredictiveForces(deltaTime, closestDistance, dynamicAvoidDistance, speedMultiplier);
}

void UPFCollisionResource::FirePredictiveRays(const FVector& startPos, const FVector* rayDirs, const float* rayDist,
											  const FVector& playerDesiredDir, FVector& outTotalRepulsion, 
											  FVector& OutBestDir, float& outClosestDistance, 
											  bool& bOutHitCenter, int& outOpenRayCount)
{
	outTotalRepulsion = FVector::ZeroVector;
	OutBestDir = rayDirs[0]; 
	outClosestDistance = MAX_flt;
	bOutHitCenter = false;
	outOpenRayCount = 0;

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Owner);
	FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->PreshotSphereSize * 0.5f);

	float bestRayScore = -MAX_flt;

	for (int i = 0; i < 9; ++i)
	{
		FHitResult hit;
		float assistDistance = rayDist[i];
		FVector endPos = startPos + (rayDirs[i] * assistDistance);
		bool bHit = false;

		if (i == 0)
		{
			bHit = OwnerWorld->SweepSingleByChannel(hit, startPos, endPos, FQuat::Identity, ECC_Visibility, sweepShape, queryParams);
			if (bHit) bOutHitCenter = true;
		}
		else
		{
			bHit = OwnerWorld->LineTraceSingleByChannel(hit, startPos, endPos, ECC_Visibility, queryParams);
		}

		DrawDebugWhiskerCone(startPos, endPos, bHit, hit);
		RayDirHits_[i] = bHit;

		// Context Scoring
		float safetyScore = bHit ? (hit.Distance / assistDistance) : 1.0f;
		float intentScore = FVector::DotProduct(rayDirs[i], playerDesiredDir);
		
		// Adjust weights here or expose them to your Data Asset
		float totalScore = (safetyScore * 2.0f) + (intentScore * 1.0f); 

		if (!bHit)
		{
			outOpenRayCount++;
		}
		else
		{
			if (hit.Distance < outClosestDistance) outClosestDistance = hit.Distance;
			
			float weight = 1.0f - safetyScore;
			outTotalRepulsion += hit.ImpactNormal * weight;
		}

		// Evaluate peripheral rays for escape
		if (i > 0 && totalScore > bestRayScore)
		{
			bestRayScore = totalScore;
			OutBestDir = rayDirs[i];
		}
	}
}

void UPFCollisionResource::UpdateSteeringRepulsion(float deltaTime, const FVector& forwardDir, 
												   const FVector& totalRepulsion, const FVector& bestDir, 
												   bool bHitCenter)
{
	FVector combinedSteering = forwardDir;

	if (bHitCenter)
	{
		combinedSteering = bestDir;
	}
	else if (totalRepulsion.Length() > 0.1f)
	{
		combinedSteering = forwardDir + totalRepulsion;
	}

	combinedSteering.Normalize();
	CurrentRepulsion_ = FMath::VInterpTo(CurrentRepulsion_, combinedSteering, deltaTime, DataPtr_->SmoothingTurn);
}

void UPFCollisionResource::ApplyPredictiveForces(float DeltaTime, float ClosestDistance, float DynamicAvoidDistance, float SpeedMultiplier)
{
	float avoidExitThreshold = DynamicAvoidDistance * 1.15f;

	if (ClosestDistance < DataPtr_->AvoidDistance)
	{
		bIsInHardAvoid_ = true;
	}
	else if (ClosestDistance > avoidExitThreshold)
	{
		bIsInHardAvoid_ = false;
	}

	if (bIsInHardAvoid_)
	{
		PhysicResourcePtr_->AddForwardVelocity(-DataPtr_->SlowForceAvoid * DeltaTime, false);
		PhysicResourcePtr_->AddVelocity(CurrentRepulsion_ * DataPtr_->AvoidForce);

		float targetPitchOffset = FMath::FindDeltaAngleDegrees(PhysicRoot->GetComponentRotation().Pitch, CurrentRepulsion_.Rotation().Pitch);
		PhysicResourcePtr_->AddAssistPitch(targetPitchOffset * DataPtr_->AvoidForceRot);

		float targetYawOffset = FMath::FindDeltaAngleDegrees(PhysicRoot->GetComponentRotation().Yaw, CurrentRepulsion_.Rotation().Yaw);
		PhysicResourcePtr_->SetYawRotationVelocity(targetYawOffset * DataPtr_->AvoidForceRot);
		return;
	}

	float distanceRatio = (ClosestDistance - DataPtr_->AvoidDistance) / (DataPtr_->AssistDistance - DataPtr_->AvoidDistance);
	float intensity = FMath::Clamp(1.0f - distanceRatio, 0.0f, 1.0f);

	PhysicResourcePtr_->AddForwardVelocity(-DataPtr_->SlowForceAssist * DeltaTime, false);

	float forceMultiplier = DataPtr_->AssistForceCurve->GetFloatValue(intensity);
	PhysicResourcePtr_->AddVelocity(CurrentRepulsion_ * (DataPtr_->AssistForce * forceMultiplier * SpeedMultiplier));

	float turnMultiplier = DataPtr_->AssistTurnCurve->GetFloatValue(intensity);

	float targetPitchOffset = FMath::FindDeltaAngleDegrees(PhysicRoot->GetComponentRotation().Pitch, CurrentRepulsion_.Rotation().Pitch);
	float pitchNudge = targetPitchOffset * turnMultiplier * DataPtr_->AssistTurnSpeed * SpeedMultiplier;
	PhysicResourcePtr_->AddAssistPitch(pitchNudge);

	float targetYawOffset = FMath::FindDeltaAngleDegrees(PhysicRoot->GetComponentRotation().Yaw, CurrentRepulsion_.Rotation().Yaw);
	PhysicResourcePtr_->SetYawRotationVelocity(targetYawOffset * turnMultiplier * DataPtr_->AssistTurnSpeed * SpeedMultiplier);
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
	if (!DataPtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}

	if (!DataPtr_->bUseRollBackOnFrontalCollision)
		return;

	if (!bCanRecord_ || TimeSavedOnImpact_ != 0)
		return;

	float forwardVelo = PhysicResourcePtr_->CurrentForwardVelocity_.Length();
	FVector position = PhysicRoot->GetComponentLocation();

	FStoredCollisionInfo collisionInfo =
		FStoredCollisionInfo(forwardVelo,
							PhysicResourcePtr_->CurrentGlobalVelocity_,
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
	if (!DataPtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}

	if (!bCanRecord_)
		return;

	float forwardVelo = PhysicResourcePtr_->CurrentForwardVelocity_.Length();
	FVector position = PhysicRoot->GetComponentLocation();

	FStoredPlaytestInfo playtestInfo =
		FStoredPlaytestInfo(forwardVelo,
							position);

	GameInfoList_.Add(playtestInfo);
}

void UPFCollisionResource::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								FVector NormalImpulse, const FHitResult& Hit)
{
	if (!DataPtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}

	FVector Velocity = PhysicResourcePtr_->GetCurrentVelocity();

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
