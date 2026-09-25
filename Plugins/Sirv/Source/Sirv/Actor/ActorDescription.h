// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Sirv/Actor/ActorAttribute.h"

#include "ActorDescription.generated.h"

/// A description of a Sirv Actor with all its variation.
USTRUCT(BlueprintType)
struct FActorDescription
{
  GENERATED_BODY()

  /// UId of the definition in which this description was based.
  uint32 UId = 0u;

  /// Display ID that identifies the actor.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Id;

  /// Class of the actor to be spawned.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<AActor> Class;

  /// User selected variations of the actor. Note that at this point are
  /// represented by non-modifiable attributes.
  ///
  ///   Key: Id of the attribute.
  ///   Value: The attribute.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TMap<FString, FActorAttribute> Variations;
};
