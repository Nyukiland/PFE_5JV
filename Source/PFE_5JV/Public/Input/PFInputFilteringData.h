#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFInputFilteringData.generated.h"

UCLASS()
class PFE_5JV_API UPFInputFilteringData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input",
	meta=(ToolTip="Valeur pour la gachette en bas", ClampMin = 0, UIMin = 0,
		ClampMax = 1, UIMax = 1))
	float InputPressedValueThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input",
		meta=(ToolTip="Valeur pour la gachette en haut", ClampMin = 0, UIMin = 0,
			ClampMax = 1, UIMax = 1))
	float InputReleaseValueThreshold;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat",
		meta=(ToolTip="Force à donner à la vitesse"))
    float SimultaneousTapTolerance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat",
	meta=(ToolTip="Valeur pour la gachette en haut", ClampMin = 0))
	float DelayInputRegisterWingBeat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive",
		meta=(ToolTip="Le temps d'attente entre chaque enregistement input", ClampMin = 0))
	float DelayInputRegisterDiveRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive",
		meta=(ToolTip="Le delta nessessaire pour declencher le dive roll", ClampMin = 0))
	float InputChangeRequiredDiveRoll;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive",
		meta=(ToolTip="Le temps d'attente avant de partir en dive", ClampMin = 0))
	float DelayBeforeGoingInDive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn",
		meta=(ToolTip="La difference entre les deux input pour commencer a turn", ClampMin = 0))
	float ToleranceRotBetweenInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn",
		meta=(ToolTip="The time between it start turning"))
	float TurnDelay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll",
		meta=(ToolTip="Le temps Max entre deux appuies sur l'input de roll", ClampMin = 0))
	float RollMaxBetweenInputTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll",
		meta=(ToolTip="Le temps d'attente apres un input valid", ClampMin = 0))
	float RollDelayCall;
};
