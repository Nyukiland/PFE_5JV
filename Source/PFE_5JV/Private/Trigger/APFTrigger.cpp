#include "Trigger/APFTrigger.h"
#include "Components/BoxComponent.h"

AAPFTrigger::AAPFTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerBoxPtr_ = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBoxPtr_;

	TriggerBoxPtr_->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBoxPtr_->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBoxPtr_->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBoxPtr_->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerBoxPtr_->SetGenerateOverlapEvents(true);
}

void AAPFTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAPFTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAPFTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AAPFTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

