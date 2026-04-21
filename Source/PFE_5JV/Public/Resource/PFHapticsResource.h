#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFHapticsResource.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FHapticsSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, 
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float Intensity;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Duration;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAffectsLeftLarge;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAffectsLeftSmall;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAffectsRightLarge;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAffectsRightSmall;
	
public:
	FHapticsSettings() = default;
	
	FHapticsSettings(float InIntensity, float InDuration, 
		bool bInLeftLarge, bool bInLeftSmall, bool bInRightLarge, bool bInRightSmall)
	   : Intensity(InIntensity), Duration(InDuration), bAffectsLeftLarge(bInLeftLarge), 
		 bAffectsLeftSmall(bInLeftSmall), bAffectsRightLarge(bInRightLarge), bAffectsRightSmall(bInRightSmall){}
};

struct FActiveHapticData
{
	FDynamicForceFeedbackHandle EngineHandle;
	FHapticsSettings Settings;
	
public:
	FActiveHapticData() = default;
	
	FActiveHapticData(const FDynamicForceFeedbackHandle& InEngineHandle, const FHapticsSettings& InSettings) 
		: EngineHandle(InEngineHandle), Settings(InSettings) {}
};

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFHapticsResource : public UPFResource
{
	GENERATED_BODY()
	
private:
	TMap<FName, FActiveHapticData> ActiveHapticsMap_;
    
	bool bIsPaused_ = false;
    
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController_;
    
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
    
	UFUNCTION(BlueprintCallable)
	void PlayHaptics(float intensity, float duration, FName uniqueID,
	   bool bAffectsLeftLarge, bool bAffectsLeftSmall, bool bAffectsRightLarge, bool bAffectsRightSmall);
    
	UFUNCTION(BlueprintCallable)
	void AskToPauseHaptics();
    
	UFUNCTION(BlueprintCallable)
	void AskToResumeHaptics();
    
private:
	void AddNewHaptics(FName uniqueID, const FHapticsSettings& settings);
	void UpdateHaptics(FActiveHapticData* existingData, const FHapticsSettings& settings);
    
	bool CheckValidity();
};