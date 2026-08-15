#include "Resource/PFProximityResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFProximityResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	OwnerWorldPtr_ = Owner->GetWorld();

	CachedQueryParams.AddIgnoredActor(Owner);
	CachedQueryParams.bTraceComplex = true;
	CachedQueryParams.bReturnFaceIndex = true;

	CachedObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	CachedObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel4);
}

void UPFProximityResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[ProximityResource] DataPtr_ is null!"));
		return;
	}
	
	CheckCollisionInFront();
	CheckClosestHit();
	CheckBelowHit();
}

float UPFProximityResource::GetClosestDistancePercentage() const
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[ProximityResource] DataPtr_ is null!"));
		return 0;
	}

	if (!ClosestHitResult.bBlockingHit)
	{
		return 0;
	}

	float dist = ClosestHitResult.Distance;
	dist -= DataPtr_->SmallestClosestSphereSize;
	float max = DataPtr_->BaseClosestSphereSize - DataPtr_->SmallestClosestSphereSize;

	return 1 - FMath::Clamp(dist / max, 0.0f, 1.0f);
}

void UPFProximityResource::CheckCollisionInFront()
{
	FVector startPosition = Owner->GetActorLocation() + (DataPtr_->ForwardSphereDistance * PhysicRoot->
		GetForwardVector());
	const FCollisionShape sphere = FCollisionShape::MakeSphere(DataPtr_->ForwardSphereSize);
	
	ValidHitResults.Reset();

	OwnerWorldPtr_->SweepMultiByObjectType(ValidHitResults, startPosition, startPosition + FVector(0.f, 0.f, 0.1f),
										FQuat::Identity, CachedObjectQueryParams, sphere, CachedQueryParams);

#if !UE_BUILD_SHIPPING

	if (!DataPtr_->bisDrawDebugForward)
	{
		return;
	}
	
	DrawDebugSphere(OwnerWorldPtr_, startPosition, DataPtr_->ForwardSphereSize, 16, FColor::Cyan, false, -1.f);

	for (const FHitResult& hit : ValidHitResults)
	{
		if (hit.bBlockingHit || hit.bStartPenetrating)
		{
			DrawDebugLine(OwnerWorldPtr_, startPosition, hit.ImpactPoint, FColor::Green, false, -1.f, 0, 2.f);
			DrawDebugPoint(OwnerWorldPtr_, hit.ImpactPoint, 10.f, FColor::Red, false, -1.f);
			DrawDebugLine(OwnerWorldPtr_, hit.ImpactPoint, hit.ImpactPoint + (hit.ImpactNormal * 150.f), FColor::Yellow,
						false, -1.f, 0, 3.f);
		}
	}
#endif
}

void UPFProximityResource::CheckClosestHit()
{
    FVector startPosition = Owner->GetActorLocation(); 
    float currentRadius = DataPtr_->BaseClosestSphereSize;
    float stepSize = DataPtr_->ClosestSphereDetectionStep;
    float minRadius = DataPtr_->SmallestClosestSphereSize;
	
    FHitResult bestHit;
    bestHit.TraceStart = startPosition;
    bestHit.TraceEnd = startPosition;
    bestHit.Location = startPosition;
    bestHit.bBlockingHit = false;
    bestHit.Distance = currentRadius; 

    bool bHasAnyHit = false;

    while (currentRadius >= minRadius)
    {
        TArray<FHitResult> currentHits;
        FCollisionShape sphere = FCollisionShape::MakeSphere(currentRadius);

        bool bHit = OwnerWorldPtr_->SweepMultiByObjectType(currentHits, startPosition, startPosition + FVector(0.f, 0.f, 0.1f), 
            FQuat::Identity, CachedObjectQueryParams, sphere, CachedQueryParams);

        if (bHit)
        {
            bHasAnyHit = true;

            for (const FHitResult& hit : currentHits)
            {
            	bool bIsOurTrigger = hit.Component.IsValid() && (hit.Component->GetCollisionObjectType() == ECC_GameTraceChannel4);
            	
                if (hit.bBlockingHit || hit.bStartPenetrating || bIsOurTrigger)
                {
                    bestHit = hit;
                    break; 
                }
            }

            currentRadius -= stepSize;
        }
        else
        {
            break; 
        }
    }

    if (bHasAnyHit)
    {
        if (bestHit.bStartPenetrating && bestHit.ImpactPoint.IsNearlyZero())
        {
            bestHit.ImpactPoint = bestHit.Location; 
            bestHit.ImpactNormal = (startPosition - bestHit.ImpactPoint).GetSafeNormal(); 
        }

        bestHit.Distance = FVector::Dist(startPosition, bestHit.ImpactPoint);
    }

    ClosestHitResult = bestHit;

#if !UE_BUILD_SHIPPING

    if (!DataPtr_->bisDrawDebugClosest)
    {
       return;
    }
    
    FColor sphereColor = ClosestHitResult.bBlockingHit ? FColor::Orange : FColor::Red;
    
    float debugRadius = ClosestHitResult.bBlockingHit ? (currentRadius + stepSize) : DataPtr_->BaseClosestSphereSize;
    DrawDebugSphere(OwnerWorldPtr_, startPosition, debugRadius, 16, sphereColor, false, -1.f);

    if (ClosestHitResult.bBlockingHit)
    {
        DrawDebugPoint(OwnerWorldPtr_, ClosestHitResult.ImpactPoint, 12.f, FColor::Magenta, false, -1.f);
        DrawDebugLine(OwnerWorldPtr_, startPosition, ClosestHitResult.ImpactPoint, FColor::Orange, false, -1.f, 0, 1.5f);
        DrawDebugLine(OwnerWorldPtr_, ClosestHitResult.ImpactPoint, ClosestHitResult.ImpactPoint + (ClosestHitResult.ImpactNormal * 50.f), FColor::Cyan, false, -1.f, 0, 2.f);
    }
#endif
}

void UPFProximityResource::CheckBelowHit()
{
	FVector startPos = PhysicRoot->GetComponentLocation();
	FVector endPos = startPos + (ForwardRootPtr->GetUpVector() * -DataPtr_->BelowRayDistance);
	
	bool bHit = OwnerWorldPtr_->
		LineTraceSingleByObjectType(HitBelowResult, startPos, endPos, CachedObjectQueryParams, CachedQueryParams);

#if !UE_BUILD_SHIPPING

	if (!DataPtr_->bisDrawDebugBelow)
	{
		return;
	}
	
	FColor RayColor = bHit ? FColor::Red : FColor::Green;

	DrawDebugLine(OwnerWorldPtr_, startPos, bHit ? HitBelowResult.ImpactPoint : endPos, RayColor, false, -1.f, 0, 2.f);

	if (bHit)
	{
		DrawDebugPoint(OwnerWorldPtr_, HitBelowResult.ImpactPoint, 10.f, FColor::Red, false, -1.f);
		DrawDebugLine(OwnerWorldPtr_, HitBelowResult.ImpactPoint,
					HitBelowResult.ImpactPoint + (HitBelowResult.ImpactNormal * 50.f), FColor::Yellow, false, -1.f, 0,
					2.f);
	}
#endif
}

float UPFProximityResource::GetMaxClosestDistance() const
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[ProximityResource] DataPtr_ is null!"));
		return 0;
	}

	return DataPtr_->BaseClosestSphereSize;
}

float UPFProximityResource::GetMaxDistanceWithBelow() const
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[ProximityResource] DataPtr_ is null!"));
		return 0;
	}

	return DataPtr_->BelowRayDistance;
}
