// Copyright (c) 2023 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "Actor/ActorSpawnResult.h"
#include "Actor/SirvActorFactory.h"

#include "SensorFactory.generated.h"

/// Factory in charge of spawning sensors. This factory is able to spawn every
/// sensor registered in sirv::sensor::SensorRegistry.
UCLASS()
class SIRV_API ASensorFactory : public ASirvActorFactory
{
  GENERATED_BODY()

  /// Retrieve the definitions of all the sensors registered in the
  /// SensorRegistry. Sensors must implement a GetSensorDefinition() static
  /// method.
  TArray<FActorDefinition> GetDefinitions() final;

  FActorSpawnResult SpawnActor(
      const FTransform &SpawnAtTransform,
      const FActorDescription &ActorDescription) final;
};
