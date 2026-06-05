#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFChangingColorCube.generated.h"

UCLASS()
class PFE_5JV_API APFChangingColorCube : public AActor
{
	GENERATED_BODY()
	
public:	
	APFChangingColorCube();

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> RootMeshPtr_;
	
	float Timer_;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> BaseMaterialPtr_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> SwitchMaterialPtr_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeBeforeSwitch_;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
