// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Sirv/Actor/ActorSpawnResult.h"
#include "Sirv.h"

FString FActorSpawnResult::StatusToString(EActorSpawnResultStatus InStatus)
{
  static_assert(
      static_cast<uint8>(EActorSpawnResultStatus::SIZE) == 4u,
      "If you add a new status, please update this function.");

  switch (InStatus)
  {
    case EActorSpawnResultStatus::Success:
      return TEXT("Success");
    case EActorSpawnResultStatus::InvalidDescription:
      return TEXT("Spawn failed because of invalid actor description");
    case EActorSpawnResultStatus::Collision:
      return TEXT("Spawn failed because of collision at spawn position");
    case EActorSpawnResultStatus::UnknownError:
    default:
      return TEXT("Unknown error while trying to spawn actor");
  }
}
