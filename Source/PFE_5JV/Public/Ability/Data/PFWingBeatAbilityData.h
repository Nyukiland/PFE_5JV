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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Temps écoulé avant de rafraichir l'enregistrement de l'input précédent"))
	float PreviousInputRegistrationTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Force à donner vers le haut"))
	float ForceToGiveInHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Force à donner à la vitesse"))
	float ForceToGiveInVelocity;
};
