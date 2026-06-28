#pragma once

#include "CoreMinimal.h"
#include "StateMachine/PFPlayerCharacter.h"
#include "UObject/NoExportTypes.h"
#include "PFCollisionPreset.generated.h"

class UPFCollisionResource;

UCLASS(Abstract, BlueprintType, Blueprintable)
class PFE_5JV_API UPFCollisionPreset : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPFCollisionResource> CollisionResourcePtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<APFPlayerCharacter> OwnerPtr_;
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision Preset")
	void InitPreset(UPFCollisionResource* collisionResource);
	virtual void InitPreset_Implementation(UPFCollisionResource* collisionResource);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision Preset")
	void OnHitReaction(const FHitResult& hit);
	virtual void OnHitReaction_Implementation(const FHitResult& hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision Preset")
	void OnTickActions(float deltaTime);
	virtual void OnTickActions_Implementation(float deltaTime);
};
