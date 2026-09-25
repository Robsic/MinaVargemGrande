// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include <compiler/disable-ue4-macros.h>
#include "sirv/rpc/ObjectLabel.h"
#include "sirv/rpc/LabelledPoint.h"
#include <compiler/enable-ue4-macros.h>

#include <vector>
#include <utility>

#include "RayTracer.generated.h"

class AActor;

UCLASS()
class SIRV_API URayTracer : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:

  static std::vector<sirv::rpc::LabelledPoint> CastRay(
      FVector StartLocation, FVector EndLocation, UWorld * World);

  static std::pair<bool, sirv::rpc::LabelledPoint> ProjectPoint(
      FVector StartLocation, FVector Direction, float MaxDistance, UWorld * World);

};
