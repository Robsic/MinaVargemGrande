// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Vehicle/VehicleControl.h"

#include "VehicleControlCAT793F.generated.h"




USTRUCT(BlueprintType)
struct SIRV_API FCAT793FVehicleControl : public FVehicleControl
{
  GENERATED_BODY()
  
//---CAT793F----------------------------------------------------------------------------------------
  
  // UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  // float emergencybrake = 0.0f;
  
  // UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  // float brakeretarder = 0.0f;
  
  // UPROPERTY(Category = "Vehicle Control", EditAnywhere, BlueprintReadWrite)
  // int32 hoist = 0; // Control the tipper of the truck (Range: [F - floating,U - up, D - down, S - shake])
  
  int test;
};
