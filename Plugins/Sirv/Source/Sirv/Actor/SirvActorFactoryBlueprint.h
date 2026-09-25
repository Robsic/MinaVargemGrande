// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Actor/ActorSpawnResult.h"
#include "Actor/SirvActorFactory.h"

#include "GameFramework/Actor.h"

#include "SirvActorFactoryBlueprint.generated.h"

/// Base class for Blueprints implementing ASirvActorFactory interface.
///
/// Blueprints deriving from this class are expected to override
/// GetDefinitions and SpawnActor functions.
UCLASS(Abstract, BlueprintType, Blueprintable)
class SIRV_API ASirvActorFactoryBlueprint : public ASirvActorFactory
{
  GENERATED_BODY()

public:

  TArray<FActorDefinition> GetDefinitions() final
  {
    return GenerateDefinitions();
  }

  FActorSpawnResult SpawnActor(
      const FTransform &SpawnAtTransform,
      const FActorDescription &ActorDescription) final
  {
    FActorSpawnResult Result;
    SpawnActor(SpawnAtTransform, ActorDescription, Result);
    return Result;
  }

protected:

  UFUNCTION(BlueprintImplementableEvent)
  TArray<FActorDefinition> GenerateDefinitions();

  UFUNCTION(BlueprintImplementableEvent)
  void SpawnActor(
      const FTransform &SpawnAtTransform,
      const FActorDescription &ActorDescription,
      FActorSpawnResult &SpawnResult);
};
