// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#include "VehicleVelocityControl.h"
#include "Kismet/KismetMathLibrary.h"




UVehicleVelocityControl::UVehicleVelocityControl()
{
  PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleVelocityControl::BeginPlay()
{
  Super::BeginPlay();

  SetComponentTickEnabled(false);
  SetTickGroup(ETickingGroup::TG_PrePhysics);

  OwnerVehicle = GetOwner();
  PrimitiveComponent = Cast<UPrimitiveComponent>(OwnerVehicle->GetRootComponent());
}

void UVehicleVelocityControl::Activate(bool bReset)
{
  Super::Activate(bReset);

  TargetVelocity = FVector();
  SetComponentTickEnabled(true);
}

void UVehicleVelocityControl::Activate(FVector Velocity, bool bReset)
{
  Super::Activate(bReset);

  TargetVelocity = Velocity;
  SetComponentTickEnabled(true);
}

void UVehicleVelocityControl::Deactivate()
{
  SetComponentTickEnabled(false);
  Super::Deactivate();
}

void UVehicleVelocityControl::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
  FTransform Transf = OwnerVehicle->GetActorTransform();
  const FVector LocVel = Transf.TransformVector(TargetVelocity);
  PrimitiveComponent->SetPhysicsLinearVelocity(LocVel, false, "None");
}
