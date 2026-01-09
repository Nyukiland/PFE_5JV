#include "Helpers/PFMathHelper.h"

float& UPFMathHelper::IncrementFloat(float& value, float amount)
{
	value += amount;
	return value;
}

int& UPFMathHelper::IncrementInt(int& value, int amount)
{
	value += amount;
	return value;
}

float& UPFMathHelper::DecrementFloat(float& value, float amount)
{
	value -= amount;
	return value;
}

int& UPFMathHelper::DecrementInt(int& value, int amount)
{
	value -= amount;
	return value;
}

float UPFMathHelper::GetAngleBetweenVectors(const FVector vectorA, const FVector vectorB)
{
	float dotProduct = FVector::DotProduct(vectorA.GetSafeNormal(), vectorB.GetSafeNormal());
	return FMath::Acos(dotProduct) * (180.0f / PI);
}

FVector UPFMathHelper::GetClosestPoint(const FVector firstVector, const TArray<FVector> vectors)
{
	if (vectors.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[UFFMathBlueprintHelper::GetClosestPoint] List Is Empty"));
		return FVector(0, 0, 0);
	}

	FVector closest = vectors[0];
	float distClosest = FVector::Dist(firstVector, vectors[0]);

	for(const FVector vector : vectors)
	{
		float curDist = FVector::Dist(firstVector, vector);
		if (curDist < distClosest)
		{
			closest = vector;
			distClosest = curDist;
		}
	}

	return closest;
}

AActor* UPFMathHelper::GetClosestActor(const AActor* baseActor, const TArray<AActor*> actors)
{
	if (!baseActor || actors.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[UFFMathBlueprintHelper::GetClosestPoint] List Is Empty or Self is not set"));
		return nullptr;
	}

	AActor* closest = actors[0];
	float distClosest = FVector::Dist(baseActor->GetActorLocation(), actors[0]->GetActorLocation());

	for (AActor* Actor : actors)
	{
		float curDist = FVector::Dist(baseActor->GetActorLocation(), Actor->GetActorLocation());
		if (curDist < distClosest)
		{
			closest = Actor;
			distClosest = curDist;
		}
	}

	return closest;
}

FVector2D UPFMathHelper::RandomPointInCircle(float radius)
{
	float randomX = FMath::RandRange(0, 1);
	float randomY = FMath::RandRange(0, 1);
	return FVector2D(randomX, randomY).GetSafeNormal() * radius;
}

FVector UPFMathHelper::RandomPointInSphere(float radius)
{
	float randomX = FMath::RandRange(0, 1);
	float randomY = FMath::RandRange(0, 1);
	float randomZ = FMath::RandRange(0, 1);
	return FVector(randomX, randomY, randomZ).GetSafeNormal() * radius;
}

void UPFMathHelper::RandomOutput(ERandomPinOut& outputPins)
{
	bool rand = FMath::RandBool();
	if (rand) outputPins = ERandomPinOut::Random1;
	else outputPins = ERandomPinOut::Random2;
}

float& UPFMathHelper::ClampFloatByRef(float& value, const float min, const float max)
{
	value = FMath::Clamp(value, min, max);
	return value;
}

int& UPFMathHelper::ClampIntByRef(int& value, const int min, const int max)
{
	value = FMath::Clamp(value, min, max);
	return value;
}

FVector UPFMathHelper::GetOffsetBasedOnTransform(const FVector offset, const USceneComponent* toUseTransform)
{
	if (toUseTransform == nullptr) return FVector::ZeroVector;
	FVector temp = FVector(0, 0, 0);

	temp = toUseTransform->GetForwardVector() * offset.X;
	temp += toUseTransform->GetRightVector() * offset.Y;
	temp += toUseTransform->GetUpVector() * offset.Z;

	temp += toUseTransform->GetComponentLocation();
	temp = FVector(FMath::RoundToInt(temp.X), FMath::RoundToInt(temp.Y), FMath::RoundToInt(temp.Z));

	return temp;
}

FVector UPFMathHelper::RandomVector(const float intensity)
{
	float t = FMath::RandRange(0.0f, intensity);
	FVector random = FVector(t, t, t);

	return random.GetClampedToMaxSize(intensity);
}

float UPFMathHelper::Get2dDistance(FVector a, FVector b)
{
	a.Z = 0;
	b.Z = 0;

	return FVector::Dist(a, b);
}
