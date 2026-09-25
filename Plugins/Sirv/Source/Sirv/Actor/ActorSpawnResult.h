// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "ActorSpawnResult.generated.h"

/// List of valid types for actor attributes.
UENUM(BlueprintType)
enum class EActorSpawnResultStatus : uint8
{
  Success                      UMETA(DisplayName = "Success"),
  InvalidDescription           UMETA(DisplayName = "Invalid actor description"),
  Collision                    UMETA(DisplayName = "Failed because collision at spawn position"),
  UnknownError                 UMETA(DisplayName = "Unknown Error"),

  SIZE                         UMETA(Hidden)
};

/// Result of an actor spawn function.
USTRUCT(BlueprintType)
struct FActorSpawnResult
{
  GENERATED_BODY()

  FActorSpawnResult() = default;

  explicit FActorSpawnResult(AActor *InActor)
    : Actor(InActor),
      Status(Actor != nullptr ?
          EActorSpawnResultStatus::Success :
          EActorSpawnResultStatus::UnknownError) {}

  static FString StatusToString(EActorSpawnResultStatus Status);

  bool IsValid() const
  {
    return (Actor != nullptr) && (Status == EActorSpawnResultStatus::Success);
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  AActor *Actor = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EActorSpawnResultStatus Status = EActorSpawnResultStatus::UnknownError;
};
