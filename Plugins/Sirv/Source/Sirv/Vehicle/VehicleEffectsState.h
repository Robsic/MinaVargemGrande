// Copyright (c) 2022 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "VehicleEffectsState.generated.h"

USTRUCT(BlueprintType)
struct SIRV_API FVehicleEffectsState
{
  GENERATED_BODY()

  UPROPERTY(Category = "Vehicle Effects", EditAnywhere, BlueprintReadWrite)
  int Wiper = 0;

  UPROPERTY(Category = "Vehicle Effects", EditAnywhere, BlueprintReadWrite)
  bool FireEngine = false;
  
  UPROPERTY(Category = "Vehicle Effects", EditAnywhere, BlueprintReadWrite)
  bool FireWheel = false;

};
