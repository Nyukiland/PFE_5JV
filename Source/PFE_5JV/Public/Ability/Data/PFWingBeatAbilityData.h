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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="La rotation a chaque clap"))
	float RotationPerClap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="La rotation maximum pour le wingbeat"))
	float MaxWingBeatRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Le temps avant le début du reset de la rotation"))
	float TimerReset;

	// Force
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat/Force", meta=(ToolTip="La force donné à chaque clap"))
	float WingBeatForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat/Force", meta=(ToolTip="Velocité donné par rapport à l'input"))
	TObjectPtr<UCurveFloat> WingBeatAddForceBasedOnTimeCurvePtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat/Force", meta=(ToolTip="Le temps pour donné la force"))
	float WingBeatTimeForceGiven;
};
