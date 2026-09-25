// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "EpisodeSettings.generated.h"

USTRUCT(BlueprintType)
struct SIRV_API FEpisodeSettings
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bSynchronousMode = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bNoRenderingMode = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bSubstepping = true;

  TOptional<double> FixedDeltaSeconds;

  double MaxSubstepDeltaTime = 0.01;

  int MaxSubsteps = 10;

  float MaxCullingDistance = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bDeterministicRagdolls = true;

};
