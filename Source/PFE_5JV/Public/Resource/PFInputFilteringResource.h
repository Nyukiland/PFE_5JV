#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFInputFilteringResource.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EAction : uint8
{
	Base,
	Dive,
	Roll,
	WingBeat
};

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFInputFilteringResource : public UPFResource
{
	GENERATED_BODY()

public:
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	float InputLeft_;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	float InputRight_;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	float DiveValue;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	EAction CurrentAction;
	
public:
	void ReceiveInputLeft(float value);
	void ReceiveInputRight(float value);

	virtual void ComponentTick_Implementation(float deltaTime) override;

private:
		
};
