#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFWingBeatAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFWingBeatAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Courbe pour l'accélération en fonction de la moyenne des différences d'inputs avec l'enregistrement de valeur précédent"))
	TObjectPtr<UCurveFloat> WingBeatAccelerationBasedOnAverageInputValueCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Intervalle de temps entre deux enregistrements d'input (en seconde)"))
	float DelayBetweenInputRegistrations;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Force à donner vers le haut"))
	float ForceToGiveInHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Force à donner vers le haut Pour un temps X"))
	float ForceToGiveInHeightDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Force à donner à la vitesse"))
	float ForceToGiveInVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Intervalle maximum de temps entre 2 battements d'aile permettant d'obtenir un super battement d'aile (en seconde)"))
	float SuperWingBeatTiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Multiplicateur à appliquer aux forces du battement d'aile pour le super battement d'aile"))
	float SuperWingBeatMultiplier;
};
