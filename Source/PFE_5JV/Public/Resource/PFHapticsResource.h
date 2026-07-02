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
	bool bAffectsLarge;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAffectsSmall;
	
public:
	FHapticsSettings() = default;
	
	FHapticsSettings(float InIntensity, float InDuration, 
		bool bInLarge, bool bInSmall)
	   : Intensity(InIntensity), Duration(InDuration), bAffectsLarge(bInLarge), 
		 bAffectsSmall(bInSmall){}
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
	bool bisMuted_ = false;
	float Intensity_ = 1.0f;
    
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController_;
    
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
    
	UFUNCTION(BlueprintCallable)
	void PlayHaptics(float intensity, float duration, FName uniqueID,
	   bool bAffectsLarge, bool bAffectsSmall);
    
	UFUNCTION(BlueprintCallable)
	void AskToPauseHaptics();
    
	UFUNCTION(BlueprintCallable)
	void AskToResumeHaptics();
	
	UFUNCTION(BlueprintCallable)
	void SetMuteHaptics(bool bMute);
	
	UFUNCTION(BlueprintCallable)
    void ToggleMutism();
	
	UFUNCTION(BlueprintCallable)
	void ChangeHapticsIntensity(float intensity);
	
private:
	void AddNewHaptics(FName uniqueID, const FHapticsSettings& settings);
	void UpdateHaptics(FActiveHapticData* existingData, const FHapticsSettings& settings);
    
	bool CheckValidity();
};