#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFTrigger.generated.h"

class UPFTriggerBehavior;
class APFPlayerCharacter;
class UBoxComponent;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API APFTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	APFTrigger();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	TObjectPtr<APFPlayerCharacter> PlayerInTriggerPtr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	TObjectPtr<UBoxComponent> TriggerBoxPtr_;

protected:
	TArray<TObjectPtr<UPFTriggerBehavior>> BehaviorsPtr_;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
