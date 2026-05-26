#include "Resource/PFProximityResource.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFProximityResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	// Si aucune direction n'est configurée, on injecte les 6 directions cardinales par défaut
	if (!DownTrace.IsInit)
	{
		InitDefaultDirectionalTraces();
	}
}

void UPFProximityResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

    // OLD METHOD :
	// CheckCollisions();

	OverlapMultiByChannel();
	// float ClosestDist = OverlapAnyTestByChannel();
	// TArray<FDirectionalTraceResult> DirectionResults = DetectAllDirections();
}

void UPFProximityResource::CheckCollisions_Implementation(){}

// ─────────────────────────────────────────────────────────
//  1. OverlapMultiByChannel — détection sphérique globale
// ─────────────────────────────────────────────────────────
void UPFProximityResource::OverlapMultiByChannel()
{
    ValidHitResults.Empty();
    ValidBrushSizes.Empty();

    StartPosition = Owner->GetActorLocation()
                  + (DataPtr_->BrushForwardOffset * Owner->GetActorForwardVector());

    const FCollisionShape Sphere = FCollisionShape::MakeSphere(DataPtr_->BrushSphereDistance);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);

    TArray<FHitResult> HitResults;
    GetWorld()->SweepMultiByChannel(
        HitResults,
        StartPosition,
        StartPosition + FVector(0.f, 0.f, 0.1f),
        FQuat::Identity,
        DataPtr_->ProximitySweepConfig.CollisionChannel,
        Sphere,
        QueryParams
    );

    for (const FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor)
            continue;

        // Filtre : AddUnique sur l'acteur
        bool bAlreadyAdded = ValidHitResults.ContainsByPredicate([&](const FHitResult& Existing)
        {
            return Existing.GetActor() == HitActor;
        });
        if (bAlreadyAdded)
            continue;

        // Calcul BrushSize
        float Distance = FVector::Dist(Hit.ImpactPoint, StartPosition);
        float BrushSize = FMath::GetMappedRangeValueClamped(
            FVector2D(DataPtr_->BrushSphereDistance, 0.f),
            FVector2D(DataPtr_->BrushSizesMinMax.X, DataPtr_->BrushSizesMinMax.Y),
            Distance
        );

        ValidBrushSizes.Add(BrushSize);
        ValidHitResults.Add(Hit);
    }

    if (DataPtr_->ProximitySweepConfig.bDrawDebug)
    {
        const FColor Color = ValidHitResults.Num() > 0 ? FColor::Red : FColor::Green;
        DrawDebugSphere(GetWorld(), StartPosition, DataPtr_->BrushSphereDistance,
            16, Color, false, DataPtr_->ProximitySweepConfig.DebugDrawDuration);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  2. OverlapAnyTestByChannel — distance exacte par shrink progressif du rayon
// ─────────────────────────────────────────────────────────────────────────────
float UPFProximityResource::OverlapAnyTestByChannel() const
{
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);

    float CurrentRadius = DataPtr_->ClosestObstacleConfig.InitialRadius;

    while (CurrentRadius > DataPtr_->ClosestObstacleConfig.MinRadius)
    {
        const FCollisionShape Sphere = FCollisionShape::MakeSphere(CurrentRadius);

        bool bHit = GetWorld()->OverlapAnyTestByChannel(
            StartPosition,
            FQuat::Identity,
            DataPtr_->ClosestObstacleConfig.CollisionChannel,
            Sphere,
            QueryParams
        );

        if (DataPtr_->ClosestObstacleConfig.bDrawDebug)
        {
            const FColor Color = bHit ? FColor::Red : FColor::Green;
            DrawDebugSphere(GetWorld(), StartPosition, CurrentRadius,
                16, Color, false, DataPtr_->ClosestObstacleConfig.DebugDrawDuration);
        }

        if (!bHit)
            return CurrentRadius; // Le rayon au moment où on ne touche plus rien = distance exacte

        CurrentRadius -= DataPtr_->ClosestObstacleConfig.RadiusStep;
    }

    // L'obstacle est extrêmement proche (en dessous de MinRadius)
    return DataPtr_->ClosestObstacleConfig.MinRadius;
}

// ─────────────────────────────────────────────────────────
//  3. DetectAllDirections — raycast dans toutes les directions
// ─────────────────────────────────────────────────────────
FDirectionalTraceResult UPFProximityResource::DetectBottom() const
{
    FDirectionalTraceResult Results = LineTraceSingleByChannel(DownTrace);
    return Results;
}

// ─────────────────────────────────────────────────────────
//  4. LineTraceSingleByChannel — raycast dans une direction
// ─────────────────────────────────────────────────────────
FDirectionalTraceResult UPFProximityResource::LineTraceSingleByChannel(const FDirectionalTraceConfig& Config) const
{
    FDirectionalTraceResult Result;

    if (!Owner || !GetWorld())
        return Result;
    
    const FVector TraceStart = Owner->GetActorLocation();
    const FVector TraceEnd   = TraceStart + FVector(0.f, 0.f, -1.f) * Config.TraceLength;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);

    FHitResult HitResult;
    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        Config.CollisionChannel,
        QueryParams
    );

    if (bHit)
    {
        Result.bHit        = true;
        Result.Distance    = HitResult.Distance;
        Result.ImpactPoint = HitResult.ImpactPoint;
        Result.ImpactNormal= HitResult.ImpactNormal;
        Result.HitActor    = HitResult.GetActor();
    }

    if (Config.bDrawDebug)
    {
        const FColor Color = bHit ? FColor::Red : FColor::Green;
        DrawDebugLine(GetWorld(), TraceStart, bHit ? HitResult.ImpactPoint : TraceEnd,
            Color, false, Config.DebugDrawDuration, 0, 2.f);

        if (bHit)
            DrawDebugSphere(GetWorld(), HitResult.ImpactPoint,
                10.f, 8, FColor::Yellow, false, Config.DebugDrawDuration);
    }

    return Result;
}

// ─────────────────────────────────────────────────────────
//  5. InitDefaultDirectionalTraces — initialisation des directions
// ─────────────────────────────────────────────────────────
void UPFProximityResource::InitDefaultDirectionalTraces()
{
    FDirectionalTraceConfig Config;
    Config.DirectionLabel    = TEXT("Down");
    Config.LocalDirection    = FVector( 0,  0, -1);
    Config.TraceLength       = 1000.f;
    Config.CollisionChannel  = ECC_WorldStatic;
    Config.bDrawDebug        = false;
    Config.DebugDrawDuration = 0.1f;
    Config.IsInit = true;
    DownTrace = Config;
}