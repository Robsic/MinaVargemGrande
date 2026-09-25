// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Actor/ActorSpawnResult.h"
#include "Actor/SirvActorFactory.h"

#include "StaticMeshFactory.generated.h"

/// Factory in charge of spawning static meshes. This factory is able to spawn
/// any mesh in content.
UCLASS()
class SIRV_API AStaticMeshFactory : public ASirvActorFactory
{
  GENERATED_BODY()

  /// Retrieve the definitions of the static mesh actor
  TArray<FActorDefinition> GetDefinitions() final;

  FActorSpawnResult SpawnActor(
      const FTransform &SpawnAtTransform,
      const FActorDescription &ActorDescription) final;
};
