// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Game/SirvEpisode.h"
#include "Game/SirvGameInstance.h"
#include "Game/SirvGameModeBase.h"
#include "Settings/SirvSettings.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SirvStatics.generated.h"

// =============================================================================
// -- USirvStatics declaration ------------------------------------------------
// =============================================================================

UCLASS()
class SIRV_API USirvStatics : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:

  UFUNCTION(BlueprintPure, Category="SIRV", meta=(WorldContext="WorldContextObject"))
  static ASirvGameModeBase *GetGameMode(const UObject *WorldContextObject);

  UFUNCTION(BlueprintPure, Category="SIRV", meta=(WorldContext="WorldContextObject"))
  static USirvGameInstance *GetGameInstance(const UObject *WorldContextObject);

  UFUNCTION(BlueprintPure, Category="SIRV", meta=(WorldContext="WorldContextObject"))
  static USirvEpisode *GetCurrentEpisode(const UObject *WorldContextObject);

  UFUNCTION(BlueprintPure, Category="SIRV", meta=(WorldContext="WorldContextObject"))
  static USirvSettings *GetSirvSettings(const UObject *WorldContextObject);

  UFUNCTION(BlueprintPure, Category="SIRV")
  static TArray<FString> GetAllMapNames();

  //UFUNCTION(BlueprintPure, Category="SIRV", meta=(WorldContext="WorldContextObject"))
  //static ASirvRecorder* GetRecorder(const UObject *WorldContextObject);

  //static SirvReplayer* GetReplayer(const UObject *WorldContextObject);
};

// =============================================================================
// -- USirvStatics implementation ---------------------------------------------
// =============================================================================

inline ASirvGameModeBase *USirvStatics::GetGameMode(const UObject *WorldContext)
{
  return Cast<ASirvGameModeBase>(UGameplayStatics::GetGameMode(WorldContext));
}

inline USirvGameInstance *USirvStatics::GetGameInstance(const UObject *WorldContext)
{
  return Cast<USirvGameInstance>(UGameplayStatics::GetGameInstance(WorldContext));
}

inline USirvEpisode *USirvStatics::GetCurrentEpisode(const UObject *WorldContext)
{
  auto GameInstance = GetGameInstance(WorldContext);
  return GameInstance != nullptr ? GameInstance->GetSirvEpisode() : nullptr;
}

inline USirvSettings *USirvStatics::GetSirvSettings(const UObject *WorldContext)
{
  auto GameInstance = GetGameInstance(WorldContext);
  return GameInstance != nullptr ? GameInstance->GetSIRVSettings() : nullptr;
}

// inline ASirvRecorder* USirvStatics::GetRecorder(const UObject *WorldContextObject)
// {
//   auto* Episode = USirvStatics::GetCurrentEpisode(WorldContextObject);
//   if (Episode)
//   {
//     return Episode->GetRecorder();
//   }
//   return nullptr;
// }

// inline SirvReplayer* USirvStatics::GetReplayer(const UObject *WorldContextObject)
// {
//   auto* Episode = USirvStatics::GetCurrentEpisode(WorldContextObject);
//   if (Episode)
//   {
//     return Episode->GetReplayer();
//   }
//   return nullptr;
// }
