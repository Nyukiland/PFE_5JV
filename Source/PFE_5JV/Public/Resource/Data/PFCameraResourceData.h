#pragma once

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCameraResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFCameraResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Camera|DefaultMovements", meta=(ToolTip="Distance en arrière entre la caméra et le player"))
	float BackDistance = 420.f;

	UPROPERTY(EditAnywhere, Category="Camera|DefaultMovements", meta=(ToolTip="Distance en hauteur entre la caméra et le player"))
	float UpDistance = 120.f;

	UPROPERTY(EditAnywhere, Category="Camera|DefaultMovements", meta=(ToolTip="Vitesse que met la caméra pour suivre la position du player"))
	float PositionInterpSpeed = 3.5f;

	UPROPERTY(EditAnywhere, Category="Camera|DefaultMovements", meta=(ToolTip="Vitesse que met la caméra pour suivre la rotation du player"))
	float RotationInterpSpeed = 6.f;

	UPROPERTY(EditAnywhere, Category="Camera|DefaultMovements", meta=(ToolTip="Décalage max de rotation entre le player et la caméra en Pitch"))
	float MaxPitchWhenRotating = 5.0f;

	UPROPERTY(EditAnywhere, Category="Camera|DefaultMovements", meta=(ToolTip="Décalage max de rotation entre le player et la caméra en Yaw"))
	float MaxYawWhenRotating = 20.0f;

	UPROPERTY(EditAnywhere, Category="Camera|Diving", meta=(ToolTip="Distance prise par la caméra lorsque le player fait un piqué"))
	float DivingDistance = 480.0f;

	UPROPERTY(EditAnywhere, Category="Camera|Diving", meta=(ToolTip="Rotation appliquée à la caméra lorsque le player fait un piqué"))
	FRotator DivingRotation = FRotator(-25.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category="Camera|Diving", meta=(ToolTip="Durée de la transition caméra pour passer de l'état normal au piqué"))
	float DivingStartInterpDuration = 1.0f;
	
	UPROPERTY(EditAnywhere, Category="Camera|Diving", meta=(ToolTip="Durée de la transition caméra pour passer du piqué à l'état normal"))
	float DivingEndInterpDuration = 4.0f;
	
	UPROPERTY(EditAnywhere, Category="Camera|Diving", meta=(ToolTip=""))
	float FlapDistanceOffset = 40.0f;
	
	UPROPERTY(EditAnywhere, Category="Camera|Diving", meta=(ToolTip="Mouvement vers le bas de la caméra lors du flap pour donner un effet de hauteur"))
	float FlapRotationOffsetPitch = -5.0f;
	
	UPROPERTY(EditAnywhere, Category="Camera|Diving", meta=(ToolTip="Durée de la transition caméra pour passer de l'état normal au flap"))
	float FlapInterpDuration = 0.2f;
};
