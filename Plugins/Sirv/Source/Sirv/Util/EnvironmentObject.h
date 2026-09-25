// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once


#include <cstdint>

#include "Util/BoundingBox.h"

//#include "Util/rpc/ObjectLabel.h"

#include <compiler/disable-ue4-macros.h>
#include <sirv/rpc/ObjectLabel.h>
#include <compiler/enable-ue4-macros.h>

#include "EnvironmentObject.generated.h"

namespace crp = sirv::rpc;

enum EnvironmentObjectType
{
  Invalid,
  Vehicle,
  Character,
  TrafficLight,
  ISMComp,
  SMComp,
  SKMComp
};

// Name is under discussion
USTRUCT(BlueprintType)
struct SIRV_API FEnvironmentObject
{
  GENERATED_BODY()

  AActor* Actor = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Name;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString IdStr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FTransform Transform;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FBoundingBox BoundingBox;

  uint64 Id = 0;

  EnvironmentObjectType Type = EnvironmentObjectType::Invalid;

  crp::CityObjectLabel ObjectLabel;

  bool CanTick = false;
};
