#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFPainter.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class PFE_5JV_API APFPainter : public AActor
{
	GENERATED_BODY()
	
public:
	APFPainter();
	static APFPainter* Instance;
	static constexpr int MaxHISMIntances = 25;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static APFPainter* GetPainter(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PaintStuff(const TArray<FHitResult>& validHitResults, const TArray<float>& brushSizes);
	void PaintStuff_Implementation(const TArray<FHitResult>& validHitResults, const TArray<float>& brushSizes);
	// UHierarchicalInstancedStaticMeshComponent* CreateNewHISM(UHierarchicalInstancedStaticMeshComponent* HismToDuplicate);
};