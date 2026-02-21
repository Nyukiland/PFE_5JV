#include "Trigger/PFTrigger.h"
#include "Components/BoxComponent.h"
#include "StateMachine/PFPlayerCharacter.h"
#include "Trigger/PFTriggerBehavior.h"

APFTrigger::APFTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerBoxPtr_ = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBoxPtr_;

	TriggerBoxPtr_->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBoxPtr_->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBoxPtr_->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBoxPtr_->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBoxPtr_->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

	TriggerBoxPtr_->SetGenerateOverlapEvents(true);

	TriggerBoxPtr_->OnComponentBeginOverlap.AddDynamic(this, &APFTrigger::OnOverlapBegin);
	TriggerBoxPtr_->OnComponentEndOverlap.AddDynamic(this, &APFTrigger::OnOverlapEnd);
}

void APFTrigger::BeginPlay()
{
	Super::BeginPlay();

	GetComponents(BehaviorsPtr_);

	for (UPFTriggerBehavior* behavior : BehaviorsPtr_)
		behavior->Init(this);
}

void APFTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerInTriggerPtr)
		return;
		
	for (UPFTriggerBehavior* behavior : BehaviorsPtr_)
		behavior->OnTriggerTick(DeltaTime);
}

void APFTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerInTriggerPtr)
		return;

	APFPlayerCharacter* player = Cast<APFPlayerCharacter>(OtherActor);
	
	if (!player)
		return;

	PlayerInTriggerPtr = player;
	
	for (UPFTriggerBehavior* behavior : BehaviorsPtr_)
		behavior->OnTriggerEnter();
}

void APFTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!PlayerInTriggerPtr)
		return;

	APFPlayerCharacter* player = Cast<APFPlayerCharacter>(OtherActor);
	
	if (!player)
		return;
	
	for (UPFTriggerBehavior* behavior : BehaviorsPtr_)
		behavior->OnTriggerExit();

	PlayerInTriggerPtr = nullptr;
}

