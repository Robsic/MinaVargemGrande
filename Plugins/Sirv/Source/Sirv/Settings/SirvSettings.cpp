// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.




#include "Sirv/Settings/SirvSettings.h"
#include "Sirv.h"

#include "Sirv/Util/IniFile.h"

#include "Misc/CommandLine.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Engine.h"
#include "Engine/PointLight.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"
#include "UObject/Package.h"
#include "Math/UnrealMathUtility.h"

// INI file sections.
#define S_SIRV_SERVER          TEXT("SIRV/Server")
#define S_SIRV_QUALITYSETTINGS TEXT("SIRV/QualitySettings")

// =============================================================================
// -- Static variables & constants ---------------------------------------------
// =============================================================================

const FName USirvSettings::SIRV_ROAD_TAG = FName("SIRV_ROAD");
const FName USirvSettings::SIRV_SKY_TAG = FName("SIRV_SKY");

// =============================================================================
// -- Static methods -----------------------------------------------------------
// =============================================================================

static EQualityLevel QualityLevelFromString(
    const FString &SQualitySettingsLevel,
    const EQualityLevel Default = EQualityLevel::INVALID)
{
  if (SQualitySettingsLevel.Equals("Low"))
  {
    return EQualityLevel::Low;
  }
  if (SQualitySettingsLevel.Equals("Epic"))
  {
    return EQualityLevel::Epic;
  }
  return Default;
}

FString QualityLevelToString(EQualityLevel QualitySettingsLevel)
{
  const UEnum *ptr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EQualityLevel"), true);
  if (!ptr)
  {
    return FString("Invalid");
  }
  return ptr->GetNameStringByIndex(static_cast<int32>(QualitySettingsLevel));
}

static void LoadSettingsFromConfig(
    const FIniFile &ConfigFile,
    USirvSettings &Settings,
    const bool bLoadSirvServerSection)
{
  // SirvServer.
  if (bLoadSirvServerSection)
  {
    ConfigFile.GetInt(S_SIRV_SERVER, TEXT("WorldPort"), Settings.RPCPort);
    ConfigFile.GetInt(S_SIRV_SERVER, TEXT("RPCPort"), Settings.RPCPort);
  }
  ConfigFile.GetBool(S_SIRV_SERVER, TEXT("SynchronousMode"), Settings.bSynchronousMode);
  ConfigFile.GetBool(S_SIRV_SERVER, TEXT("DisableRendering"), Settings.bDisableRendering);
  // QualitySettings.
  FString sQualityLevel;
  ConfigFile.GetString(S_SIRV_QUALITYSETTINGS, TEXT("QualityLevel"), sQualityLevel);
  Settings.SetQualityLevel(QualityLevelFromString(sQualityLevel));
}

static bool GetSettingsFilePathFromCommandLine(FString &Value)
{
  if (FParse::Value(FCommandLine::Get(), TEXT("-sirv-settings="), Value))
  {
    if (FPaths::IsRelative(Value))
    {
      Value = FPaths::ConvertRelativePathToFull(FPaths::LaunchDir(), Value);
    }
    return true;
  }
  return false;
}

// =============================================================================
// -- USirvSettings -----------------------------------------------------------
// =============================================================================

void USirvSettings::LoadSettings()
{
  CurrentFileName = TEXT("");
  // Load settings from project Config folder if present.
  LoadSettingsFromFile(FPaths::Combine(FPaths::ProjectConfigDir(), TEXT("SirvSettings.ini")), false);
  // Load settings given by command-line arg if provided.
  {
    FString FilePath;
    if (GetSettingsFilePathFromCommandLine(FilePath))
    {
      LoadSettingsFromFile(FilePath, true);
    }
  }
  // Override settings from command-line.
  {
    uint32 Value;
    if (FParse::Value(FCommandLine::Get(), TEXT("-world-port="), Value) ||
        FParse::Value(FCommandLine::Get(), TEXT("-sirv-port="), Value) ||
        FParse::Value(FCommandLine::Get(), TEXT("-sirv-rpc-port="), Value) ||
        FParse::Value(FCommandLine::Get(), TEXT("-sirv-world-port="), Value))
    {
      RPCPort = Value;
    }
    if (FParse::Value(FCommandLine::Get(), TEXT("-sirv-streaming-port="), Value))
    {
      StreamingPort = Value;
    }
    FString StringQualityLevel;
    if (FParse::Value(FCommandLine::Get(), TEXT("-quality-level="), StringQualityLevel))
    {
      QualityLevel = QualityLevelFromString(StringQualityLevel, EQualityLevel::Epic);
    }
    if (FParse::Param(FCommandLine::Get(), TEXT("-no-rendering")))
    {
      bDisableRendering = true;
    }
    QualityLevel = EQualityLevel::Epic;
  }
}

void USirvSettings::LoadSettingsFromString(const FString &INIFileContents)
{
  UE_LOG(LogSirv, Log, TEXT("Loading SIRV settings from string"));
  FIniFile ConfigFile;
  ConfigFile.ProcessInputFileContents(INIFileContents);
  constexpr bool bLoadSirvServerSection = false;
  LoadSettingsFromConfig(ConfigFile, *this, bLoadSirvServerSection);
  CurrentFileName = TEXT("<string-provided-by-client>");
}

void USirvSettings::LogSettings() const
{
  auto EnabledDisabled = [](bool bValue) { return (bValue ? TEXT("Enabled") : TEXT("Disabled")); };
  UE_LOG(LogSirv, Log,
      TEXT("== SIRV Settings =============================================================="));
  UE_LOG(LogSirv, Log, TEXT("Last settings file loaded: %s"), *CurrentFileName);
  UE_LOG(LogSirv, Log, TEXT("[%s]"), S_SIRV_SERVER);
  UE_LOG(LogSirv, Log, TEXT("RPC Port = %d"), RPCPort);
  UE_LOG(LogSirv, Log, TEXT("Streaming Port = %d"), StreamingPort.Get(RPCPort + 1u));
  UE_LOG(LogSirv, Log, TEXT("Synchronous Mode = %s"), EnabledDisabled(bSynchronousMode));
  UE_LOG(LogSirv, Log, TEXT("Rendering = %s"), EnabledDisabled(!bDisableRendering));
  UE_LOG(LogSirv, Log, TEXT("[%s]"), S_SIRV_QUALITYSETTINGS);
  UE_LOG(LogSirv, Log, TEXT("Quality Level = %s"), *QualityLevelToString(QualityLevel));
  UE_LOG(LogSirv, Log,
      TEXT("================================================================================"));


}

#undef S_SIRV_QUALITYSETTINGS
#undef S_SIRV_SERVER

void USirvSettings::LoadSettingsFromFile(const FString &FilePath, const bool bLogOnFailure)
{
  if (FPaths::FileExists(FilePath))
  {
    UE_LOG(LogSirv, Log, TEXT("Loading SIRV settings from \"%s\""), *FilePath);
    const FIniFile ConfigFile(FilePath);
    constexpr bool bLoadSirvServerSection = true;
    LoadSettingsFromConfig(ConfigFile, *this, bLoadSirvServerSection);
    CurrentFileName = FilePath;
  }
  else if (bLogOnFailure)
  {
    UE_LOG(LogSirv, Error, TEXT("Unable to find settings file \"%s\""), *FilePath);
  }
}
