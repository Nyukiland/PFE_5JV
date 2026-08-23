#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PFPainter.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

USTRUCT(Blueprintable, BlueprintType)
struct FPFHismData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HISM Data")
	UHierarchicalInstancedStaticMeshComponent* HismPtr_ = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HISM Data")
	int Index = -1;
};

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
	void CreateNewHismModel(const TSubclassOf<AActor>& ActorClass);
	static void InitializeHism(const TSubclassOf<AActor>& ActorClass, UHierarchicalInstancedStaticMeshComponent* HismPtr_);
	
public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Painter")
	TArray<UClass*> HismToGenerate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Painter")
	TMap<UClass*, FPFHismData> ActiveHisms;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static APFPainter* GetPainter(UObject* WorldContext);

	UFUNCTION()
	void ReplaceActorsByHismByClass(TSubclassOf<AActor> ActorClass, FLinearColor ColorValue, TArray<FTransform> PlacedObjectTransforms);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PaintStuff(const TArray<FHitResult>& validHitResults, const TArray<float>& brushSizes);
	void PaintStuff_Implementation(const TArray<FHitResult>& validHitResults, const TArray<float>& brushSizes);
};