// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Game/Tagger.h"

#include "Engine/World.h"

#include "TaggerDelegate.generated.h"

/// Used to tag every actor that is spawned into the world.
UCLASS()
class SIRV_API UTaggerDelegate : public UObject
{
  GENERATED_BODY()

public:

  UTaggerDelegate();

  void RegisterSpawnHandler(UWorld *World);

  void SetSemanticSegmentationEnabled(bool Enable = true)
  {
    bSemanticSegmentationEnabled = Enable;
  }

  void OnActorSpawned(AActor *Actor);

private:

  FOnActorSpawned::FDelegate ActorSpawnedDelegate;

  bool bSemanticSegmentationEnabled = false;
};
