// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Sirv/Actor/ActorDefinition.h"
#include "Sirv/Actor/ActorDescription.h"
#include "Sirv/Actor/ActorSpawnResult.h"

#include "Containers/Array.h"
#include "GameFramework/Actor.h"

#include "SirvActorFactory.generated.h"

/// Base class for Sirv actor factories.
UCLASS(Abstract)
class SIRV_API ASirvActorFactory : public AActor
{
  GENERATED_BODY()

public:

  ASirvActorFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
  {
    PrimaryActorTick.bCanEverTick = false;
  }

  /// Retrieve the list of actor definitions that this class is able to spawn.
  virtual TArray<FActorDefinition> GetDefinitions() {
    unimplemented();
    return {};
  }

  /// Spawn an actor based on @a ActorDescription and @a Transform.
  ///
  /// @pre ActorDescription is expected to be derived from one of the
  /// definitions retrieved with GetDefinitions.
  virtual FActorSpawnResult SpawnActor(
      const FTransform &SpawnAtTransform,
      const FActorDescription &ActorDescription) {
    unimplemented();
    return {};
  }
};
