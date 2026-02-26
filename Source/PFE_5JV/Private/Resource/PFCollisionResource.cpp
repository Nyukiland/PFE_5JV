#include "Resource/PFCollisionResource.h"

#include "StateMachine/PFPlayerCharacter.h"

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

	if (!bCanRecord_)
		return;

	float forwardVelo = PhysicResource_->CurrentForwardVelo_.Length();
	FVector position = PhysicRoot->GetComponentLocation();
	
	// Store info for the collision
	FStoredCollisionInfo CollisionInfo =
		FStoredCollisionInfo(forwardVelo,
			PhysicResource_->CurrentGlobalVelo_,
			position, PhysicRoot->GetComponentRotation(),
			DiveAbility_->CurrentMedianValue_);
	
	TimeSavedList_.Insert(OwnerWorld->GetTimeSeconds(), 0);
	StoredCollisionInfoList_.Insert(CollisionInfo, 0);

	//Test to remove the useless info
	if (OwnerWorld->GetTimeSeconds() - TimeSavedList_.Last() > DataPtr_->DurationOfRemember)
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
	
	float forwardVelo = PhysicResource_->CurrentForwardVelo_.Length();
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
	FVector playerDirection = PhysicResource_->GetCurrentVelocity();
	float dotCollision = FVector::DotProduct(normalOfCollider, playerDirection.GetSafeNormal());
	
	if (dotCollision > 0)
		return;

	if (FMath::Abs(dotCollision) > DataPtr_->ThresholdCollision)
	{
		OnHardCollision.Broadcast();
		return;
	}

	OnSoftCollision.Broadcast();
	
}
