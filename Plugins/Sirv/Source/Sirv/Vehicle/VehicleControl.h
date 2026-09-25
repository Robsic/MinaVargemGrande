// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "VehicleControl.generated.h"

USTRUCT(BlueprintType)
struct SIRV_API FVehicleControl
{
  GENERATED_BODY()

  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  float Throttle = 0.0f;

  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  float Steer = 0.0f;

  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  float Brake = 0.0f;

  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  bool bHandBrake = false;

  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  bool bReverse = false;

  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  bool bManualGearShift = false;

  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bManualGearShift))
  int32 Gear = 0; // (transmission) Range: [1,2,D,N,R] 
  
  //----------------------------------------------------------------------------------
  
  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  float emergencybrake = 0.0f;
  
  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  float brakeretarder = 0.0f;
  
  UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  int32 hoist = 0; // Control the tipper of the truck (Range: [F - floating,U - up, D - down, S - shake])
};
