#include "Resource/PFPhysicResource.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFPhysicResource::AddForce(FVector force, bool bShouldResetForce, bool bShouldAddAtTheEnd, float duration, UCurveFloat* curve)
{
    FForceToAdd forceToAdd(force, bShouldResetForce, bShouldAddAtTheEnd, duration, curve);
    AllForces_.Add(forceToAdd);
}

FVector UPFPhysicResource::GetCurrentVelocity() const
{
    return Root->GetPhysicsLinearVelocity();
}

float UPFPhysicResource::GetAlignmentWithUp() const
{
    FVector forward = Root->GetForwardVector();

    return FVector::DotProduct(forward, GetCurrentVelocity());
}

float UPFPhysicResource::GetCurrentAirFriction() const
{
    if (!DataPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
        return 0;
    }

    float alignment = GetAlignmentWithUp();
    
    if (alignment > 0 && DataPtr_->AirFrictionCurveUp)
    {
        return FMath::Lerp(DataPtr_->BaseAirFriction,
            DataPtr_->AirFrictionGoingUp,
            DataPtr_->AirFrictionCurveUp->GetFloatValue(alignment));
    }
    if (alignment < 0 && DataPtr_->AirFrictionCurveDown)
    {
        alignment = FMath::Abs(alignment);
        
        return FMath::Lerp(DataPtr_->BaseAirFriction,
            DataPtr_->AirFrictionGoingDown,
            DataPtr_->AirFrictionCurveDown->GetFloatValue(alignment));
    }

    return DataPtr_->BaseAirFriction;
}

void UPFPhysicResource::ProcessAirFriction(const float deltaTime)
{
    FVector dir = Velocity_.GetSafeNormal();
    dir *= -1;

    Velocity_ -= deltaTime * GetCurrentAirFriction() * dir;
}

void UPFPhysicResource::ProcessVelocity(const float deltaTime)
{
    FVector velocity = Velocity_;
    
    for (int i = AllForces_.Num() - 1; i >= 0; i--)
    {
        FForceToAdd* forceToAdd = &AllForces_[i];

        if (!forceToAdd)
        {
            AllForces_.RemoveAt(i);
            continue;
        }

        FVector toAdd = forceToAdd->Force;
        
        if (forceToAdd->CurvePtr)
        {
            toAdd = forceToAdd->Force * forceToAdd->CurvePtr->GetFloatValue(forceToAdd->Duration);
        }

        forceToAdd->Duration -= deltaTime;

        if (!forceToAdd->bShouldReset)
        {
            Velocity_ += toAdd;
        }
        
        if (forceToAdd->Duration <= 0)
        {
            AllForces_.RemoveAt(i);
            if (forceToAdd->bShoudEndAdded)
            {
                Velocity_ += toAdd;
            }
        }
        
        velocity += toAdd;
    }

    Root->SetPhysicsLinearVelocity(velocity);
}

void UPFPhysicResource::ProcessMaxSpeed(const float deltaTime)
{
    if (!DataPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
        return;
    }
    
    FVector velocity = GetCurrentVelocity();
    velocity = velocity.GetClampedToMaxSize(DataPtr_->MaxAboveSpeed);

    if (velocity.Length() < DataPtr_->MaxSpeed)
        return;;

    FVector dir = velocity.GetSafeNormal();
    dir *= -1;

    velocity -= deltaTime * GetCurrentAirFriction() * dir;

    Root->SetPhysicsLinearVelocity(velocity);
}

void UPFPhysicResource::SetYawRotationForce(float rotation)
{
    AngularVelocity.Z = rotation;
}

void UPFPhysicResource::SetPitchRotationForce(float rotation)
{
    AngularVelocity.X = rotation;
}

void UPFPhysicResource::ProcessAngularVelocity()
{
    Root->SetPhysicsAngularVelocityInDegrees(AngularVelocity);
}

void UPFPhysicResource::DoGravity(const float deltaTime)
{
    AllForces_.Add(FForceToAdd(DataPtr_->Gravity * deltaTime * FVector::UpVector));
}
