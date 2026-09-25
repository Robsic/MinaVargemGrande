// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Engine/StaticMesh.h"

#include "Sirv/Settings/QualityLevelUE.h"

#include "SirvSettings.generated.h"

/// Global settings for SIRV.
///
/// Setting object used to hold both config settings and editable ones in one
/// place. To ensure the settings are saved to the specified config file make
/// sure to add props using the globalconfig or config meta.
UCLASS(BlueprintType, Blueprintable, config = Game, defaultconfig)
class SIRV_API USirvSettings : public UObject
{
  GENERATED_BODY()

public:

  /// Sets the new quality settings level and make changes in the game related
  /// to it.
  ///
  /// @note This will not apply the quality settings. Use ApplyQualitySettings
  /// functions instead
  /// @param InQualityLevel Store the new quality.
  UFUNCTION(BlueprintCallable, Category = "SIRV Settings")
  void SetQualityLevel(EQualityLevel InQualityLevel)
  {
    QualityLevel = InQualityLevel;
  }

  /// @return current quality settings level (could not have been applied yet).
  UFUNCTION(BlueprintCallable, Category = "SIRV Settings")
  EQualityLevel GetQualityLevel() const
  {
    return QualityLevel;
  }

  /// Load the settings based on the command-line arguments and the INI file if
  /// provided.
  void LoadSettings();

  /// Load the settings from the given string (formatted as INI). SirvServer
  /// section is ignored.
  void LoadSettingsFromString(const FString &INIFileContents);

  /// Log settings values.
  void LogSettings() const;

public:

  /// SIRV_ROAD name to tag road mesh actors.
  static const FName SIRV_ROAD_TAG;

  /// SIRV_SKY name to tag the sky sphere (BPS) actors in the scenes.
  static const FName SIRV_SKY_TAG;

private:

  void LoadSettingsFromFile(const FString &FilePath, bool bLogOnFailure);

  /// File name of the settings file used to load this settings. Empty if none
  /// used.
  UPROPERTY(Category = "SIRV Settings|Debug", VisibleAnywhere)
  FString CurrentFileName;

  // ===========================================================================
  /// @name SIRV Server
  // ===========================================================================
  /// @{

public:

  /// World port to listen for client connections.
  UPROPERTY(Category = "SIRV Server", VisibleAnywhere, meta = (EditCondition = bUseNetworking))
  uint32 RPCPort = 2000u;

  /// Optional setting for the secondary port.
  TOptional<uint32> StreamingPort;

  /// In synchronous mode, SIRV waits every tick until the control from the
  /// client is received.
  UPROPERTY(Category = "SIRV Server", VisibleAnywhere, meta = (EditCondition = bUseNetworking))
  bool bSynchronousMode = false;

  /// Enable or disable the viewport rendering of the world. Disabled by
  /// default.
  UPROPERTY(Category = "SIRV Server", VisibleAnywhere)
  bool bDisableRendering = false;


  // ===========================================================================
  /// @name Quality Settings
  // ===========================================================================
  /// @{

private:

  /// Quality Settings level.
  UPROPERTY(Category = "Quality Settings", VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
  EQualityLevel QualityLevel = EQualityLevel::Epic;

public:

  /// Low quality Road Materials. Uses slots name to set material for each part
  /// of the road for low quality.
  ///
  /// @todo Move Low quality vars to a generic map of structs with the quality
  /// level as key.
  UPROPERTY(Category = "Quality Settings/Low",
      BlueprintReadOnly,
      EditAnywhere,
      config,
      DisplayName = "Road Materials List for Low Quality")
  TArray<FStaticMaterial> LowRoadMaterials;

  /// Distance at which the light function should be completely faded to
  /// DisabledBrightness. This is useful for hiding aliasing from light
  /// functions applied in the distance.
  UPROPERTY(Category = "Quality Settings/Low", BlueprintReadOnly, EditAnywhere, config)
  float LowLightFadeDistance  = 1000.0f;

  /// Default low distance for all primitive components.
  UPROPERTY(Category = "Quality Settings/Low", BlueprintReadOnly, EditAnywhere, config,
      meta = (ClampMin = "5000.0", ClampMax = "20000.0", UIMin = "5000.0", UIMax = "20000.0"))
  float LowStaticMeshMaxDrawDistance = 10000.0f;

  /// Default low distance for roads meshes.
  UPROPERTY(Category = "Quality Settings/Low", BlueprintReadOnly, EditAnywhere, config,
      meta = (ClampMin = "5000.0", ClampMax = "20000.0", UIMin = "5000.0", UIMax = "20000.0"))
  float LowRoadPieceMeshMaxDrawDistance = 15000.0f;

  /// EPIC quality Road Materials. Uses slots name to set material for each part
  /// of the road for Epic quality.
  UPROPERTY(Category = "Quality Settings/Epic",
      BlueprintReadOnly,
      EditAnywhere,
      config,
      DisplayName = "Road Materials List for EPIC Quality")
  TArray<FStaticMaterial> EpicRoadMaterials;

  /// @}
};
