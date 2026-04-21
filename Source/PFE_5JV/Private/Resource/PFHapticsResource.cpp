#include "Resource/PFHapticsResource.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFHapticsResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
    Super::ComponentInit_Implementation(ownerObj);
    PlayerController_ = Cast<APlayerController>(ownerObj->GetController());
    CheckValidity();
}

void UPFHapticsResource::PlayHaptics(float intensity, float duration, FName uniqueID,
        bool bAffectsLeftLarge, bool bAffectsLeftSmall, bool bAffectsRightLarge, bool bAffectsRightSmall)
{
    if (!CheckValidity()) return;
    
    intensity = FMath::Clamp(intensity, 0.f, 1.f);
    FHapticsSettings settings = FHapticsSettings(intensity, duration, 
            bAffectsLeftLarge, bAffectsLeftSmall, bAffectsRightLarge, bAffectsRightSmall);
    
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
            Data.Settings.Intensity, Data.Settings.Duration, 
            Data.Settings.bAffectsLeftLarge, Data.Settings.bAffectsLeftSmall, 
            Data.Settings.bAffectsRightLarge, Data.Settings.bAffectsRightSmall,
            EDynamicForceFeedbackAction::Start, 
            FDynamicForceFeedbackHandle() 
        );
    }
}

void UPFHapticsResource::AddNewHaptics(FName uniqueID, const FHapticsSettings& settings)
{
    FDynamicForceFeedbackHandle NewHandle = PlayerController_->PlayDynamicForceFeedback(
            settings.Intensity, settings.Duration, 
            settings.bAffectsLeftLarge, settings.bAffectsLeftSmall, 
            settings.bAffectsRightLarge, settings.bAffectsRightSmall,
            EDynamicForceFeedbackAction::Start, 
            FDynamicForceFeedbackHandle()
    );

    ActiveHapticsMap_.Add(uniqueID, FActiveHapticData(NewHandle, settings));
}

void UPFHapticsResource::UpdateHaptics(FActiveHapticData* existingData, const FHapticsSettings& settings)
{
    existingData->Settings = settings;
        
    PlayerController_->PlayDynamicForceFeedback(
        settings.Intensity, settings.Duration, 
        settings.bAffectsLeftLarge, settings.bAffectsLeftSmall, 
        settings.bAffectsRightLarge, settings.bAffectsRightSmall,
        EDynamicForceFeedbackAction::Update, 
        existingData->EngineHandle
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
