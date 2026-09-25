// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "WheelPhysicsControl.generated.h"

USTRUCT(BlueprintType)
struct SIRV_API FWheelPhysicsControl
{
  GENERATED_BODY()

  UPROPERTY(Category = "Wheel Tire Friction", EditAnywhere, BlueprintReadWrite)
  float TireFriction = 3.5f;

  UPROPERTY(Category = "Wheel Damping Rate", EditAnywhere, BlueprintReadWrite)
  float DampingRate = 1.0f;

  UPROPERTY(Category = "Wheel Max Steer Angle", EditAnywhere, BlueprintReadWrite)
  float MaxSteerAngle = 70.0f;

  UPROPERTY(Category = "Wheel Shape Radius", EditAnywhere, BlueprintReadWrite)
  float Radius = 30.0f;

  UPROPERTY(Category = "Wheel Max Brake Torque (Nm)", EditAnywhere, BlueprintReadWrite)
  float MaxBrakeTorque = 1500.0f;

  UPROPERTY(Category = "Wheel Max Handbrake Torque (Nm)", EditAnywhere, BlueprintReadWrite)
  float MaxHandBrakeTorque = 3000.0f;

  UPROPERTY(Category = "Wheel Position", EditAnywhere, BlueprintReadWrite)
  FVector Position = FVector::ZeroVector;
};
