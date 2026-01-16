#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFPhysicResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFPhysicResourceData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Speed")
    float MaxSpeed = 10;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Speed")
    float MaxAboveSpeed = 10;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gravity")
    float Gravity = 2;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gravity")
    float TimerMaxGravity = 1;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction")
    float BaseAirFriction;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Above")
    float AboveSpeedFriction;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Above")
    TObjectPtr<UCurveFloat> AboveSpeedFrictionCurve;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Friction Up")
    float AirFrictionGoingUp;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Friction Up")
    TObjectPtr<UCurveFloat> AirFrictionCurveUp;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Friction Down")
    float AirFrictionGoingDown;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Friction Down")
    TObjectPtr<UCurveFloat> AirFrictionCurveDown;
};
