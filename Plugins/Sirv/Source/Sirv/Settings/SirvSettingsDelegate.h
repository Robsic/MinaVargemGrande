// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "Sirv/Settings/QualityLevelUE.h"

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"

#include "SirvSettingsDelegate.generated.h"

class USirvSettings;

/// Used to set settings for every actor that is spawned into the world.
UCLASS(BlueprintType)
class SIRV_API USirvSettingsDelegate : public UObject
{
  GENERATED_BODY()

public:

  USirvSettingsDelegate();

  /// Reset settings to default.
  void Reset();

  /// Create the event trigger handler for all the newly spawned actors to be
  /// processed with a custom function here.
  void RegisterSpawnHandler(UWorld *World);

  /// After loading a level, apply the current settings.
  UFUNCTION(BlueprintCallable, Category = "SIRV Settings", meta = (HidePin = "InWorld"))
  void ApplyQualityLevelPostRestart();

  /// Before loading a level, apply the current settings.
  UFUNCTION(BlueprintCallable, Category = "SIRV Settings", meta = (HidePin = "InWorld"))
  void ApplyQualityLevelPreRestart();

  void SetAllActorsDrawDistance(UWorld *world, float max_draw_distance) const;

private:

  UWorld *GetLocalWorld();

  /// Function to apply to the actor that is being spawned to apply the current
  /// settings.
  void OnActorSpawned(AActor *Actor);

  /// Check that the world, instance and settings are valid and save the
  /// SirvSettings instance.
  ///
  /// @param world used to get the instance of SirvSettings.
  void CheckSirvSettings(UWorld *world);

  /// Execute engine commands to apply the low quality level to the world.
  void LaunchLowQualityCommands(UWorld *world) const;

  void SetAllRoads(
      UWorld *world,
      float max_draw_distance,
      const TArray<FStaticMaterial> &road_pieces_materials) const;

  void SetActorComponentsDrawDistance(AActor *actor, float max_draw_distance) const;

  void SetPostProcessEffectsEnabled(UWorld *world, bool enabled) const;

  /// Execute engine commands to apply the epic quality level to the world.
  void LaunchEpicQualityCommands(UWorld *world) const;

  void SetAllLights(
      UWorld *world,
      float max_distance_fade,
      bool cast_shadows,
      bool hide_non_directional) const;

private:

  /// Currently applied quality level after level is restarted.
  static EQualityLevel AppliedLowPostResetQualityLevel;

  USirvSettings *SirvSettings = nullptr;

  FOnActorSpawned::FDelegate ActorSpawnedDelegate;
};
