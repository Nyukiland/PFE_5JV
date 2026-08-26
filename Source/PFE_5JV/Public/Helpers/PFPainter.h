#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Helpers/FlowerSpawner/PFFlowerSpawnerTypes.h"
#include "PFPainter.generated.h"

struct FPFStaticMeshModelData;
class UHierarchicalInstancedStaticMeshComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHismAmountInstanced, const TSubclassOf<AActor>& ActorClass);

UCLASS()
class PFE_5JV_API APFPainter : public AActor
{
	GENERATED_BODY()
	
public:
	APFPainter();
	static APFPainter* Instance;
	static constexpr int MaxHismIntances = 5000;

	FOnMaxHismAmountInstanced OnMaxHismAmountInstancedDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void CreateNewHismModel(const TSubclassOf<AActor>& ActorClass);
	static void InitializeHism(const TSubclassOf<AActor>& ActorClass, UHierarchicalInstancedStaticMeshComponent* HismPtr_);
	
public:	
	virtual void Tick(float DeltaTime) override;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Painter")
	// TArray<UClass*> HismToGenerate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Painter")
	TMap<UClass*, FPFStaticMeshModelData> StaticMeshModelsToSpawn;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static APFPainter* GetPainter(UObject* WorldContext);

	UFUNCTION()
	void ReplaceActorsByHismByClass(const TSubclassOf<AActor>& ActorClass, const FLinearColor ColorValue, const TArray<FTransform>& PlacedObjectTransforms);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PaintStuff(const TArray<FHitResult>& validHitResults, const TArray<float>& brushSizes);
	void PaintStuff_Implementation(const TArray<FHitResult>& validHitResults, const TArray<float>& brushSizes);
};