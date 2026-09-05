// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/FlowerSpawner/FlowerSpawnerHelper.h"

#include "Helpers/PFPainter.h"
#include "Helpers/FlowerSpawner/PFFlowerSpawnerTypes.h"
#include "Materials/MaterialParameterCollection.h"

bool UFlowerSpawnerHelper::TryGetFlowerColorFromEnum(const EPFFlowerColor FlowerColor, FLinearColor& ColorValue)
{
	bool bParameterFound = false;
	
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
	
	UMaterialParameterCollection* mpc = APFPainter::Instance->MaterialParameterCollectionPtr;
	ColorValue = mpc->GetVectorParameterDefaultValue(ColorName, bParameterFound);
	return bParameterFound;
}

FName UFlowerSpawnerHelper::GetFlowerEnvironmentNameFromEnum(const EPFFlowerEnvironment Environment)
{
	FName EnvironmentName;
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