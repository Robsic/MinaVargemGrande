// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "Vehicle/WheelPhysicsControl.h"
#include "VehiclePhysicsControl.generated.h"

USTRUCT(BlueprintType)
struct FGearPhysicsControl
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(Category = "Gear Physics Control", EditAnywhere, BlueprintReadWrite)
  float Ratio = 1.0f;

  UPROPERTY(Category = "Gear Physics Control", EditAnywhere, BlueprintReadWrite)
  float DownRatio = 0.5f;

  UPROPERTY(Category = "Gear Physics Control", EditAnywhere, BlueprintReadWrite)
  float UpRatio = 0.65f;
};

USTRUCT(BlueprintType)
struct SIRV_API FVehiclePhysicsControl
{
  GENERATED_BODY()

  // MECHANICAL SETUP

  // Engine Setup
  FRichCurve TorqueCurve;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float MaxRPM = 0.0f;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float EngineRevUpMOI = 0.0f;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float DampingRateFullThrottle = 0.0f;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float DampingRateZeroThrottleClutchEngaged = 0.0f;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float DampingRateZeroThrottleClutchDisengaged = 0.0f;

  // // Transmission Setup
  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  bool bUseAutomaticGears = 0.0f;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float GearChangeTime = 0.0f;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float ClutchStrength = 0.0f;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float FinalRatio = 1.0f;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  TArray<FGearPhysicsControl> ForwardGears;

  // Vehicle Setup
  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float Mass = 0.0f;

  UPROPERTY(Category = "Vehicle Engine Physics Control", EditAnywhere, BlueprintReadWrite)
  float DragCoefficient = 0.0f;

  // Steering Setup
  FRichCurve SteeringCurve;

  // Center Of Mass
  UPROPERTY(Category = "Vehicle Center Of Mass", EditAnywhere, BlueprintReadWrite)
  FVector CenterOfMass;

  // Wheels Setup
  TArray<FWheelPhysicsControl> Wheels;

  UPROPERTY(Category = "Vehicle Wheels Configuration", EditAnywhere, BlueprintReadWrite)
  bool UseSweepWheelCollision = false;
};
