// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "Actor/ActorDefinition.h"
#include "Actor/ActorDescription.h"
#include "Actor/ActorRegistry.h"
#include "Actor/ActorSpawnResult.h"

#include "Containers/Array.h"
#include "Templates/Function.h"

#include "ActorDispatcher.generated.h"

class ASirvActorFactory;

/// Object in charge of binding ActorDefinitions to spawn functions, as well as
/// keeping the registry of all the actors spawned.
UCLASS()
class SIRV_API UActorDispatcher : public UObject
{
  GENERATED_BODY()

public:

  using SpawnFunctionType = TFunction<FActorSpawnResult(const FTransform &, const FActorDescription &)>;

  /// Bind a definition to a spawn function. When SpawnActor is called with a
  /// matching description @a Functor is called.
  ///
  /// @warning Invalid definitions are ignored.
  void Bind(FActorDefinition Definition, SpawnFunctionType SpawnFunction);

  /// Bind all the definitions of @a ActorFactory to its spawn function.
  ///
  /// @warning Invalid definitions are ignored.
  void Bind(ASirvActorFactory &ActorFactory);

  /// Spawns an actor based on @a ActorDescription at @a Transform. To properly
  /// despawn an actor created with this function call DestroyActor.
  ///
  /// @return A pair containing the result of the spawn function and a view over
  /// the actor and its properties. If the status is different of Success the
  /// view is invalid.
  TPair<EActorSpawnResultStatus, FActorView> SpawnActor(
      const FTransform &Transform,
      FActorDescription ActorDescription,
      FActorView::IdType DesiredId = 0);

  /// Destroys an actor, properly removing it from the registry.
  ///
  /// Return true if the @a Actor is destroyed or already marked for
  /// destruction, false if indestructible or nullptr.
  bool DestroyActor(AActor *Actor);

  /// Register an actor that was not created using "SpawnActor" function but
  /// that should be kept in the registry.
  FActorView RegisterActor(AActor &Actor, FActorDescription ActorDescription, FActorRegistry::IdType DesiredId = 0);

  const TArray<FActorDefinition> &GetActorDefinitions() const
  {
    return Definitions;
  }

  const FActorRegistry &GetActorRegistry() const
  {
    
    return Registry;
  }

private:

  UFUNCTION()
  void OnActorDestroyed(AActor *Actor)
  {
    Registry.Deregister(Actor);
  }

  TArray<FActorDefinition> Definitions;

  TArray<SpawnFunctionType> SpawnFunctions;

  TArray<TSubclassOf<AActor>> Classes;

  FActorRegistry Registry;

};
