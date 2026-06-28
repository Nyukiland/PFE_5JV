#pragma once

#include "CoreMinimal.h"
#include "CollisionPreset/PFCollisionPreset.h"
#include "Data/PFCollisionResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFCollisionResource.generated.h"

class UPFPhysicResource;


USTRUCT(BlueprintType)
struct FStoredPlaytestInfo
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	float ForwardForce;
	UPROPERTY(BlueprintReadWrite)
	FVector Position;

public:
	FStoredPlaytestInfo()
	{
		ForwardForce = 0;
		Position = FVector::ZeroVector;
	}
	
	FStoredPlaytestInfo(float forward, FVector position)
	{
		ForwardForce = forward;
		Position = position;
	}
};

USTRUCT(BlueprintType)
struct FPlaytestSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FStoredPlaytestInfo> PlaytestData;
};

UENUM()
enum class ERayDir : uint8
{
	Forward,
	Right,
	Left,
	Top,
	Bottom,
	TopRight,
	TopLeft,
	BottomRight,
	BottomLeft,
};

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFCollisionResource : public UPFResource
{
	GENERATED_BODY()

public:
	bool bHitRight;
	bool bHitLeft;
	bool bHitUp; 
	bool bHitDown;  

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UPFCollisionResourceData> DataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bRecordPlaytest = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UPFCollisionPreset> CurrentPresetPtr_;
	
	UPROPERTY()
	TObjectPtr<UWorld> OwnerWorld;

	UPROPERTY()
	TArray<FStoredPlaytestInfo> GameInfoList_;
	
public:
	UFUNCTION(BlueprintCallable)
	void ChangeCollisionPreset(TSubclassOf<UPFCollisionPreset> collisionPreset);

	UFUNCTION(BlueprintCallable, Category = "Playtest")
	void StartRecordingPlaytestData();
	
	UFUNCTION(BlueprintCallable, Category = "Playtest")
	bool ExportPlaytestDataToJson(const FString& FileName);

	virtual int GetPriority() const override;
	
protected:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;
	
	void CheckFlank(float deltaTime);
	void RecordInfoForPlayTest();
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* hitComp, AActor* otherActor,
		UPrimitiveComponent* otherComp,	FVector normalImpulse, const FHitResult& hit);
};
