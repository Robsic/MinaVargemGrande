// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "Engine/GameInstance.h"

#include "Game/SirvEngine.h"
//#include "Sirv/Recorder/SirvRecorder.h"
//#include "Sirv/Server/SirvServer.h"

#include <compiler/disable-ue4-macros.h>
#include <sirv/rpc/MapLayer.h>
#include <sirv/rpc/OpendriveGenerationParameters.h>
#include <compiler/enable-ue4-macros.h>

#include "SirvGameInstance.generated.h"

class USirvSettings;

/// The game instance contains elements that must be kept alive in between
/// levels. It is instantiate once per game.
UCLASS()
class SIRV_API USirvGameInstance : public UGameInstance
{
  GENERATED_BODY()

public:

  USirvGameInstance();

  ~USirvGameInstance();

  USirvSettings &GetSirvSettings()
  {
    check(SirvSettings != nullptr);
    return *SirvSettings;
  }

  const USirvSettings &GetSirvSettings() const
  {
    check(SirvSettings != nullptr);
    return *SirvSettings;
  }

  // Extra overload just for blueprints.
  UFUNCTION(BlueprintCallable)
  USirvSettings *GetSIRVSettings()
  {
    return SirvSettings;
  }

  UFUNCTION(BlueprintCallable)
  USirvEpisode *GetSirvEpisode()
  {
    check(SirvEngine != nullptr);
    return SirvEngine->GetCurrentEpisode();
  }

  void NotifyInitGame()
  {
    check(SirvEngine != nullptr);
    SirvEngine->NotifyInitGame(GetSirvSettings());
  }

  void NotifyBeginEpisode(USirvEpisode &Episode)
  {
    check(SirvEngine != nullptr);
    SirvEngine->NotifyBeginEpisode(Episode);
  }

  void NotifyEndEpisode()
  {
    check(SirvEngine != nullptr);
    SirvEngine->NotifyEndEpisode();
  }

  const USirvRosBinder *GetRosNode() const
  {
    check(SirvEngine != nullptr);
    return SirvEngine->GetRosNode();
  }

  void SetOpendriveGenerationParameters(
      const sirv::rpc::OpendriveGenerationParameters & Parameters)
  {
    GenerationParameters = Parameters;
  }

  const sirv::rpc::OpendriveGenerationParameters&
      GetOpendriveGenerationParameters() const
  {
    return GenerationParameters;
  }

  UFUNCTION(Category = "Sirv Game Instance", BlueprintCallable)
  void SetMapLayer(int32 MapLayer)
  {
    CurrentMapLayer = MapLayer;
  }

  UFUNCTION(Category = "Sirv Game Instance", BlueprintCallable)
  int32 GetCurrentMapLayer() const
  {
    return CurrentMapLayer;
  }

private:

  UPROPERTY(Category = "SIRV Settings", EditAnywhere)
  USirvSettings *SirvSettings = nullptr;

  UPROPERTY()
  USirvEngine *SirvEngine;

  //UPROPERTY()
  //ASirvRecorder *Recorder = nullptr;

  sirv::rpc::OpendriveGenerationParameters GenerationParameters;

  UPROPERTY(Category = "SIRV Game Instance", EditAnywhere)
  int32 CurrentMapLayer = static_cast<int32>(sirv::rpc::MapLayer::All);

};
