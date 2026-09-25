// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Vehicle/SirvWheeledVehicle.h"

#include "VehicleParameters.generated.h"

USTRUCT(BlueprintType)
struct SIRV_API FVehicleParameters
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Make;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Model;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<ASirvWheeledVehicle> Class;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 NumberOfWheels = 4;

  /// (OPTIONAL) Use for custom classification of vehicles.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString ObjectType;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FColor> RecommendedColors;

  /// List of IDs of the drivers (pedestrians) supported by this vehicle, leave
  /// empty if no driver is supported.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<int32> SupportedDrivers;
};
