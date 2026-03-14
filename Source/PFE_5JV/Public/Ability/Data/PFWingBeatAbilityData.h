#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFWingBeatAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFWingBeatAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Velocité à donner à la vitesse"))
	float MaxVelocityWingBeatVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="La rotation max avec chaque clap"))
	float MaxRotationValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="La rotation basé par rapport à l'input"))
	TObjectPtr<UCurveFloat> RotationCurvePtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Le nombre de clap pour atteindre la rotation maximale"))
	float MaxClapCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Le nombre de clap pour atteindre la rotation maximale"))
	float RotationAddedIfUnclamped = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Le temps avant le début du reset de la rotation"))
	float TimerReset;

	// Force
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat/Force", meta=(ToolTip="La force donné à chaque clap"))
	float WingBeatForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat/Force", meta=(ToolTip="Velocité donné par rapport à l'input"))
	TObjectPtr<UCurveFloat> WingBeatAddForceBasedOnTimeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat/Force", meta=(ToolTip="Le temps pour donné la force"))
	float WingBeatTimeForceGiven;
};
