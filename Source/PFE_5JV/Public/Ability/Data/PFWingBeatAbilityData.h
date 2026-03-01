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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Velocité donné par rapport à l'input"))
	TObjectPtr<UCurveFloat> VelocityToGiveCurvePtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="La rotation max avec chaque clap"))
	float MaxRotationValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="La rotation basé par rapport à l'input"))
	TObjectPtr<UCurveFloat> RotationCurvePtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Le nombre de clap pour atteindre la rotation maximale"))
	int MaxClapCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Le nombre de clap pour atteindre la rotation maximale"))
	float TimerReset;
};
