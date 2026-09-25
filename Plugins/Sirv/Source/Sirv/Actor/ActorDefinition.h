// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Sirv/Actor/ActorAttribute.h"

#include "GameFramework/Actor.h"

#include "ActorDefinition.generated.h"

/// A definition of a Sirv Actor with all the variation and attributes.
USTRUCT(BlueprintType)
struct FActorDefinition
{
  GENERATED_BODY()

  /// Uniquely identifies the definition (no need to fill it).
  uint32 UId = 0u;

  /// Display ID that identifies the actor.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Id;

  /// Class of the actor to be spawned (Optional).
  ///
  /// Note that this parameter is not exposed on the client-side, only used by
  /// the spawner itself.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<AActor> Class;

  /// A list of comma-separated tags.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Tags;

  /// Variations represent variables the user can modify to generate variations
  /// of the actor.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FActorVariation> Variations;

  /// Attributes represent non-modifiable properties of the actor that might
  /// help the user identifying and filtering actors.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FActorAttribute> Attributes;
};
