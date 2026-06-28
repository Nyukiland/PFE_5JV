#pragma once

#include "CoreMinimal.h"
#include "PFStrictCollisionData.h"
#include "Resource/PFPhysicResource.h"
#include "Resource/CollisionPreset/PFCollisionPreset.h"
#include "PFStrictCollisionPreset.generated.h"

UCLASS()
class PFE_5JV_API UPFStrictCollisionPreset : public UPFCollisionPreset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPFStrictCollisionData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;
	
public:
	virtual void InitPreset_Implementation(UPFCollisionResource* collisionResource) override;
    
	virtual void OnHitReaction_Implementation(const FHitResult& hit) override;
};
