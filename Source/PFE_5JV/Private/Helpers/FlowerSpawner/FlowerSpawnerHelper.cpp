// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/FlowerSpawner/FlowerSpawnerHelper.h"
#include "Helpers/FlowerSpawner/PFFlowerSpawnerTypes.h"
#include "Materials/MaterialParameterCollection.h"

bool UFlowerSpawnerHelper::TryGetFlowerColorFromEnum(const EPFFlowerColor FlowerColor, FLinearColor& ColorValue)
{
	bool bParameterFound = false;
	
	static UMaterialParameterCollection* CachedMPCPtr_ = Cast<UMaterialParameterCollection>(
		StaticLoadObject(UMaterialParameterCollection::StaticClass(), nullptr, TEXT("/Game/Materials/MPC/MPC_flowers.MPC_flowers"))
		);
	if(!CachedMPCPtr_)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UFlowerSpawnerHelper] TryGetFlowerColorFromEnum : Material parameter collection not found "));
		return bParameterFound;
	}
	
	FName ColorName;
	switch (FlowerColor)
	{
	case EPFFlowerColor::EPFFC_Blue:
		ColorName = "S_Blue";
		break;
	case EPFFlowerColor::EPFFC_Red:
		ColorName = "S_Red";
		break;
	case EPFFlowerColor::EPFFC_Yellow:
		ColorName = "S_Yellow";
		break;
	case EPFFlowerColor::EPFFC_Purple:
		ColorName = "S_Purple";
		break;
	case EPFFlowerColor::EPFFC_None:
	default:
		return bParameterFound;
	}
	
	ColorValue = CachedMPCPtr_->GetVectorParameterDefaultValue(ColorName, bParameterFound);
	return bParameterFound;
}

FString UFlowerSpawnerHelper::GetFlowerEnvironmentNameFromEnum(const EPFFlowerEnvironment Environment)
{
	FString EnvironmentName;
	switch (Environment)
	{
		case EPFFlowerEnvironment::EPFFS_Cliff :
			EnvironmentName = TEXT("Cliff");
			break;
		case EPFFlowerEnvironment::EPFFS_Water :
			EnvironmentName = TEXT("Water");
			break;
		case EPFFlowerEnvironment::EPFFS_Landscape :
		default:
			EnvironmentName = TEXT("Landscape");
			break;
	}
	
	return EnvironmentName;
}
