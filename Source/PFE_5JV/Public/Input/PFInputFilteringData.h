// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFInputFilteringData.generated.h"

/**
 * 
 */
UCLASS()
class PFE_5JV_API UPFInputFilteringData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WingBeat", meta=(ToolTip="Force à donner à la vitesse"))
    float SimultaneousTapTolerance;
	
};
