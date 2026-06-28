#include "Resource/PFCollisionResource.h"

#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFCollisionResource::ChangeCollisionPreset(TSubclassOf<UPFCollisionPreset> collisionPreset)
{
	if (!collisionPreset)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Null collision preset when trying to change"))
		return;
	}

	CurrentPresetPtr_ = NewObject<UPFCollisionPreset>(this, collisionPreset);

	if (CurrentPresetPtr_)
	{
		CurrentPresetPtr_->InitPreset(this);
	}
}

int UPFCollisionResource::GetPriority() const
{
	return 100;
}

void UPFCollisionResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = Owner->GetStateComponent<UPFPhysicResource>();
	
	OwnerWorld = Owner->GetWorld();

	PhysicRoot->SetNotifyRigidBodyCollision(true);
	PhysicRoot->BodyInstance.bUseCCD = true;

	PhysicRoot->OnComponentHit.AddDynamic(
		this,
		&UPFCollisionResource::OnHit);

	ChangeCollisionPreset(DataPtr_->DefaultCollisionPreset);
}

void UPFCollisionResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	if (!CurrentPresetPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Null collision preset"))
		return;
	}

	CurrentPresetPtr_->OnTickActions(deltaTime);
	CheckFlank(deltaTime);
	RecordInfoForPlayTest();
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
	(PhysicResourcePtr_->CurrentForwardVelocity_.Length() * DataPtr_->FlankPredictionDist * ForwardRootPtr->
		GetForwardVector());

	FVector rightEnd = startPos + (ForwardRootPtr->GetRightVector() * flankTraceLength);
	FVector leftEnd = startPos + (-ForwardRootPtr->GetRightVector() * flankTraceLength);
	FVector upEnd = startPos + (ForwardRootPtr->GetUpVector() * flankTraceLength);
	FVector downEnd = startPos + (-ForwardRootPtr->GetUpVector() * flankTraceLength);

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

void UPFCollisionResource::RecordInfoForPlayTest()
{
	if (!DataPtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Bad set up on data"));
		return;
	}

	float forwardVelo = PhysicResourcePtr_->CurrentForwardVelocity_.Length();
	FVector position = PhysicRoot->GetComponentLocation();

	FStoredPlaytestInfo playtestInfo =
		FStoredPlaytestInfo(forwardVelo,
							position);

	GameInfoList_.Add(playtestInfo);
}

void UPFCollisionResource::OnHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp,
								FVector normalImpulse, const FHitResult& hit)
{
	if (!CurrentPresetPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CollisionResource] Null collision preset"))
		return;
	}

	CurrentPresetPtr_->OnHitReaction(hit);
}