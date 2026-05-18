#include "Resource/PFProximityResource.h"

#include "Engine/OverlapResult.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFProximityResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	// Si aucune direction n'est configurée, on injecte les 6 directions cardinales par défaut
	if (DirectionalTraces.Num() == 0)
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
                  + (BrushForwardOffset * Owner->GetActorForwardVector());

    const FCollisionShape Sphere = FCollisionShape::MakeSphere(BrushSphereDistance);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);

    TArray<FHitResult> HitResults;
    GetWorld()->SweepMultiByChannel(
        HitResults,
        StartPosition,
        StartPosition + FVector(0.f, 0.f, 0.1f),
        FQuat::Identity,
        ProximitySweepConfig.CollisionChannel,
        Sphere,
        QueryParams
    );

    for (const FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor)
            continue;

        // Filtre 1 : doit être un Landscape
        // if (!HitActor->IsA<Landscape>())
        //     continue;

        // Filtre 2 : doit avoir le composant assigné dans l'éditeur (BP_PaintableSurface)
        if (!PaintableSurfaceClass || !HitActor->FindComponentByClass(PaintableSurfaceClass))
            continue;

        // Filtre 3 : AddUnique sur l'acteur
        bool bAlreadyAdded = ValidHitResults.ContainsByPredicate([&](const FHitResult& Existing)
        {
            return Existing.GetActor() == HitActor;
        });
        if (bAlreadyAdded)
            continue;

        // Calcul BrushSize
        float Distance = FVector::Dist(Hit.ImpactPoint, StartPosition);
        float BrushSize = FMath::GetMappedRangeValueClamped(
            FVector2D(BrushSphereDistance, 0.f),
            FVector2D(BrushSizesMinMax.X, BrushSizesMinMax.Y),
            Distance
        );

        ValidBrushSizes.Add(BrushSize);
        ValidHitResults.Add(Hit);
    }

    if (ProximitySweepConfig.bDrawDebug)
    {
        const FColor Color = ValidHitResults.Num() > 0 ? FColor::Red : FColor::Green;
        DrawDebugSphere(GetWorld(), StartPosition, BrushSphereDistance,
            16, Color, false, ProximitySweepConfig.DebugDrawDuration);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  2. OverlapAnyTestByChannel — distance exacte par shrink progressif du rayon
// ─────────────────────────────────────────────────────────────────────────────
float UPFProximityResource::OverlapAnyTestByChannel() const
{
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);

    float CurrentRadius = ClosestObstacleConfig.InitialRadius;

    while (CurrentRadius > ClosestObstacleConfig.MinRadius)
    {
        const FCollisionShape Sphere = FCollisionShape::MakeSphere(CurrentRadius);

        bool bHit = GetWorld()->OverlapAnyTestByChannel(
            StartPosition,
            FQuat::Identity,
            ClosestObstacleConfig.CollisionChannel,
            Sphere,
            QueryParams
        );

        if (ClosestObstacleConfig.bDrawDebug)
        {
            const FColor Color = bHit ? FColor::Red : FColor::Green;
            DrawDebugSphere(GetWorld(), StartPosition, CurrentRadius,
                16, Color, false, ClosestObstacleConfig.DebugDrawDuration);
        }

        if (!bHit)
            return CurrentRadius; // Le rayon au moment où on ne touche plus rien = distance exacte

        CurrentRadius -= ClosestObstacleConfig.RadiusStep;
    }

    // L'obstacle est extrêmement proche (en dessous de MinRadius)
    return ClosestObstacleConfig.MinRadius;
}

// ─────────────────────────────────────────────────────────
//  3. DetectAllDirections — raycast dans toutes les directions
// ─────────────────────────────────────────────────────────
TArray<FDirectionalTraceResult> UPFProximityResource::DetectAllDirections() const
{
    TArray<FDirectionalTraceResult> Results;
    Results.Reserve(DirectionalTraces.Num());

    for (const FDirectionalTraceConfig& Config : DirectionalTraces)
    {
        Results.Add(LineTraceSingleByChannel(Config));
    }

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
    const TArray<TPair<FString, FVector>> DefaultDirections = {
        { TEXT("Down"),     FVector( 0,  0, -1) },
        // { TEXT("Up"),       FVector( 0,  0,  1) },
        // { TEXT("Forward"),  FVector( 1,  0,  0) },
        // { TEXT("Backward"), FVector(-1,  0,  0) },
        // { TEXT("Right"),    FVector( 0,  1,  0) },
        // { TEXT("Left"),     FVector( 0, -1,  0) },
    };

    for (const auto& [Label, Dir] : DefaultDirections)
    {
        FDirectionalTraceConfig Config;
        Config.DirectionLabel    = Label;
        Config.LocalDirection    = Dir;
        Config.TraceLength       = 1000.f;
        Config.CollisionChannel  = ECC_WorldStatic;
        Config.bDrawDebug        = false;
        Config.DebugDrawDuration = 0.1f;

        DirectionalTraces.Add(Config);
    }
}