// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFPoolable.h"
#include "GameFramework/Actor.h"
#include "PFFlower.generated.h"

class UPoolSubsystem;

UCLASS()
class APFFlower : public AActor, public IPFPoolable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APFFlower();
	TObjectPtr<UMaterialInstanceDynamic> GetDynamicMaterial() { return DynamicMaterial;}
	TObjectPtr<UStaticMeshComponent> GetFlowerMesh() { return FlowerMeshComponent;}
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flower)
	TObjectPtr<UStaticMeshComponent> FlowerMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Flower)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial = nullptr;
	
	UFUNCTION(BlueprintCallable, Category = "Poolable")
	virtual void OnSpawnFromPool_Implementation() override;
	
	UFUNCTION(BlueprintCallable, Category = "Poolable")
	virtual void OnReturnToPool_Implementation() override;
};