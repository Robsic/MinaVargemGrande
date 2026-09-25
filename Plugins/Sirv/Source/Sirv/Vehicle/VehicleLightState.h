// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "VehicleLightState.generated.h"

USTRUCT(BlueprintType)
struct SIRV_API FVehicleLightState
{
  GENERATED_BODY()

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool Position = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool LowBeam = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool HighBeam = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool Brake = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool RightBlinker = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool LeftBlinker = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool Reverse = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool Fog = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool Interior = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool Special1 = false;

  UPROPERTY(Category = "Vehicle Lights", EditAnywhere, BlueprintReadWrite)
  bool Special2 = false;

};
