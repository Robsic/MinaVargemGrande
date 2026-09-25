// Copyright (c) 2022 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "VehicleSoundState.generated.h"


USTRUCT(BlueprintType)
struct SIRV_API FVehicleSoundState
{
  GENERATED_BODY()

  UPROPERTY(Category = "Vehicle Sounds", EditAnywhere, BlueprintReadWrite)
  bool Horn = false;

  UPROPERTY(Category = "Vehicle Sounds", EditAnywhere, BlueprintReadWrite)
  bool Reverse = false;

  UPROPERTY(Category = "Vehicle Sounds", EditAnywhere, BlueprintReadWrite)
  bool Enginekeycrank = false;

  UPROPERTY(Category = "Vehicle Sounds", EditAnywhere, BlueprintReadWrite)
  bool Enginekeyon = false;
  
  UPROPERTY(Category = "Vehicle Sounds", EditAnywhere, BlueprintReadWrite)
  bool Dashboard2salert = false;

};
