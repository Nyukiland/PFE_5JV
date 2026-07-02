#include "Resource/PFHapticsResource.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFHapticsResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);
	PlayerController_ = Cast<APlayerController>(ownerObj->GetController());
	CheckValidity();
}

void UPFHapticsResource::PlayHaptics(float intensity, float duration, FName uniqueID,
									bool bAffectsLarge, bool bAffectsSmall)
{
	if (!CheckValidity()) return;
	if (bisMuted_) return;

	intensity = FMath::Clamp(intensity, 0.f, 1.f);
	FHapticsSettings settings = FHapticsSettings(intensity, duration,
												bAffectsLarge, bAffectsSmall);

	if (bIsPaused_)
	{
		ActiveHapticsMap_.Add(uniqueID, FActiveHapticData(FDynamicForceFeedbackHandle(), settings));
		return;
	}

	if (FActiveHapticData* existingData = ActiveHapticsMap_.Find(uniqueID))
	{
		UpdateHaptics(existingData, settings);
	}
	else
	{
		AddNewHaptics(uniqueID, settings);
	}
}

void UPFHapticsResource::AskToPauseHaptics()
{
	if (!CheckValidity() || bIsPaused_)
		return;

	bIsPaused_ = true;

	for (auto& Pair : ActiveHapticsMap_)
	{
		FActiveHapticData& Data = Pair.Value;

		PlayerController_->PlayDynamicForceFeedback(
			0.f, 0.f, false, false, false, false,
			EDynamicForceFeedbackAction::Stop,
			Data.EngineHandle
		);

		Data.EngineHandle = FDynamicForceFeedbackHandle();
	}
}

void UPFHapticsResource::AskToResumeHaptics()
{
	if (!CheckValidity() || !bIsPaused_)
		return;

	bIsPaused_ = false;

	for (auto& Pair : ActiveHapticsMap_)
	{
		FActiveHapticData& Data = Pair.Value;

		Data.EngineHandle = PlayerController_->PlayDynamicForceFeedback(
			Data.Settings.Intensity * Intensity_, Data.Settings.Duration,
			Data.Settings.bAffectsLarge, Data.Settings.bAffectsSmall,
			Data.Settings.bAffectsLarge, Data.Settings.bAffectsSmall,
			EDynamicForceFeedbackAction::Start,
			FDynamicForceFeedbackHandle()
		);
	}
}

void UPFHapticsResource::SetMuteHaptics(bool bMute)
{
	if (bMute == bisMuted_)
		return;
	
	bisMuted_ = bMute;
	
	if (bisMuted_)
		return;
	
	for (auto& Pair : ActiveHapticsMap_)
	{
		FActiveHapticData& Data = Pair.Value;

		PlayerController_->PlayDynamicForceFeedback(
			0.f, 0.f, false, false, false, false,
			EDynamicForceFeedbackAction::Stop,
			Data.EngineHandle
		);
	}
	
	ActiveHapticsMap_.Empty();
}

void UPFHapticsResource::ToggleMutism()
{
	SetMuteHaptics(!bisMuted_);
}

void UPFHapticsResource::ChangeHapticsIntensity(float intensity)
{
	for (auto& Pair : ActiveHapticsMap_)
	{
		FActiveHapticData& Data = Pair.Value;

		float intensityHaptics = Data.Settings.Intensity;
		intensityHaptics /= Intensity_;
		intensityHaptics *= intensity;
		
		Data.EngineHandle = PlayerController_->PlayDynamicForceFeedback(
			intensityHaptics, Data.Settings.Duration,
			Data.Settings.bAffectsLarge, Data.Settings.bAffectsSmall,
			Data.Settings.bAffectsLarge, Data.Settings.bAffectsSmall,
			EDynamicForceFeedbackAction::Start,
			FDynamicForceFeedbackHandle()
		);
	}
	
	Intensity_ = intensity;
}

void UPFHapticsResource::AddNewHaptics(FName uniqueID, const FHapticsSettings& settings)
{
	FDynamicForceFeedbackHandle NewHandle = PlayerController_->PlayDynamicForceFeedback(
		settings.Intensity * Intensity_, settings.Duration,
		settings.bAffectsLarge, settings.bAffectsSmall,
		settings.bAffectsLarge, settings.bAffectsSmall,
		EDynamicForceFeedbackAction::Start,
		FDynamicForceFeedbackHandle()
	);

	ActiveHapticsMap_.Add(uniqueID, FActiveHapticData(NewHandle, settings));
}

void UPFHapticsResource::UpdateHaptics(FActiveHapticData* existingData, const FHapticsSettings& settings)
{
	existingData->Settings = settings;

	if (settings.Duration == -1)
	{
		PlayerController_->PlayDynamicForceFeedback(
			settings.Intensity * Intensity_, settings.Duration,
			settings.bAffectsLarge, settings.bAffectsSmall,
			settings.bAffectsLarge, settings.bAffectsSmall,
			EDynamicForceFeedbackAction::Update,
			existingData->EngineHandle
		);
		
		return;
	}
	
	PlayerController_->PlayDynamicForceFeedback(
		0.f, 0.f, false, false, false, false, 
		EDynamicForceFeedbackAction::Stop, 
		existingData->EngineHandle
	);

	existingData->Settings = settings;
        
	existingData->EngineHandle = PlayerController_->PlayDynamicForceFeedback(
		settings.Intensity * Intensity_, settings.Duration, 
		settings.bAffectsLarge, settings.bAffectsSmall, 
		settings.bAffectsLarge, settings.bAffectsSmall,
		EDynamicForceFeedbackAction::Start, 
		FDynamicForceFeedbackHandle()
	);
}

bool UPFHapticsResource::CheckValidity()
{
	if (!PlayerController_)
	{
		UE_LOG(LogTemp, Error, TEXT("[HapticsResource] Failed to get the PlayerController"))
		return false;
	}
	return true;
}
