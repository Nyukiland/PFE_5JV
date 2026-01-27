#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFPhysicResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFPhysicResourceData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Speed",
        meta = (ToolTip = "Maximum speed of the player"))
    float MaxSpeed = 10;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Speed",
        meta = (ToolTip = "Maximum speed even with boost"))
    float MaxAboveSpeed = 10;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gravity",
        meta = (ToolTip = "Downward force applied to the player"))
    float Gravity = 2;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gravity",
        meta = (ToolTip = "How long before the gravity is active"))
    float TimerMaxGravity = 1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gravity",
        meta = (ToolTip = "How long before the gravity is at max force"))
    float GravityLerpTime = 1;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction",
        meta = (ToolTip = "The friction applied to the player when going horizontal"))
    float BaseAirFriction;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction",
        meta = (ToolTip = "How long before the friction reach it's full capacity"))
    float TimerMaxFriction;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction",
        meta = (ToolTip = "Use the Timer Max Friction to evaluate the curve and use a percentage of the friction"))
    TObjectPtr<UCurveFloat> FrictionTimerControlCurvePtr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Above",
        meta = (ToolTip = "The friction applied to the player when going above the max speed"))
    float AboveSpeedFriction;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Above",
        meta = (ToolTip = "The curve used for the friction when going above speed, 0 is max speed, 1 is above max speed"))
    TObjectPtr<UCurveFloat> AboveSpeedFrictionCurvePtr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Friction Up",
        meta = (ToolTip = "The friction applied to the player when going up"))
    float AirFrictionGoingUp;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Friction Up",
        meta = (ToolTip = "The curve used for the friction based on the rotation"))
    TObjectPtr<UCurveFloat> AirFrictionCurveUpPtr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Friction Down",
        meta = (ToolTip = "The friction applied to the player when going horizontal"))
    float AirFrictionGoingDown;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Friction/Friction Down",
        meta = (ToolTip = "The curve used for the friction based on the rotation"))
    TObjectPtr<UCurveFloat> AirFrictionCurveDownPtr;
};
