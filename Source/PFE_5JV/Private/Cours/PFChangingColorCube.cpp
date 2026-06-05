#include "Cours/PFChangingColorCube.h"

APFChangingColorCube::APFChangingColorCube()
{
	PrimaryActorTick.bCanEverTick = true;

	RootMeshPtr_ = CreateDefaultSubobject<UStaticMeshComponent>("RootComponent");
	RootComponent = RootMeshPtr_;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(
		TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (CubeMeshAsset.Succeeded())
	{
		RootMeshPtr_->SetStaticMesh(CubeMeshAsset.Object);
	}
}

void APFChangingColorCube::BeginPlay()
{
	Super::BeginPlay();

	if (BaseMaterialPtr_ == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[ChangingColorCube] No valid base material"))
		return;
	}

	Timer_ = 0;
	RootMeshPtr_->SetMaterial(0, BaseMaterialPtr_);
}

void APFChangingColorCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SwitchMaterialPtr_ == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[ChangingColorCube] No valid switch material"))
		return;
	}

	Timer_ += DeltaTime;
	if (Timer_ >= TimeBeforeSwitch_)
	{
		RootMeshPtr_->SetMaterial(0, SwitchMaterialPtr_);
	}
}
