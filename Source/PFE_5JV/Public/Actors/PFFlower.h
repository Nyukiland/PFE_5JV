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
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flower)
	TObjectPtr<UStaticMeshComponent> FlowerMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Flower)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial = nullptr;

	// TObjectPtr<UPoolSubsystem> PoolSubsystemPtr_;
	//
	// UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Poolable")
	// void OnSpawnFromPool_Implementation();
	//
	// UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Poolable")
	// void OnReturnToPool_Implementation();
};
