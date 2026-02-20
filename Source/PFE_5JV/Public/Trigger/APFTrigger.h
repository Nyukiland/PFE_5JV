#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APFTrigger.generated.h"

class APFPlayerCharacter;
class UBoxComponent;

UCLASS()
class PFE_5JV_API AAPFTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AAPFTrigger();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	TObjectPtr<APFPlayerCharacter> PlayerInTriggerPtr;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	TObjectPtr<UBoxComponent> TriggerBoxPtr_;
	
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
