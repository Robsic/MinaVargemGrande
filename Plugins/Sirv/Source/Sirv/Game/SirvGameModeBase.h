// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include <compiler/disable-ue4-macros.h>
#include <boost/optional.hpp>


//#include <sirv_ros/sirv_ros_subscription_handler.hpp>


#include <compiler/enable-ue4-macros.h>

#include "Engine/Engine.h"
 	

#include "GameFramework/GameUserSettings.h"

#include "Sirv/Actor/SirvActorFactory.h"
#include "Sirv/Game/SirvEpisode.h"
#include "Sirv/Game/SirvGameInstance.h"
#include "Sirv/Game/TaggerDelegate.h"
//#include "Sirv/OpenDrive/OpenDrive.h"
//#include "Sirv/Recorder/SirvRecorder.h"
//#include "Sirv/Sensor/SceneCaptureSensor.h"
#include "Sirv/Settings/SirvSettingsDelegate.h"
//#include "Sirv/Traffic/TrafficLightManager.h"
#include "Sirv/Util/ObjectRegister.h"
#include "Sirv/Weather/Weather.h"

#include "Kismet/GameplayStatics.h"

#include "SirvGameModeBase.generated.h"

/// Base class for the SIRV Game Mode.
UCLASS(HideCategories=(ActorTick))
class SIRV_API ASirvGameModeBase : public AGameModeBase
{
  GENERATED_BODY()

public:

  ASirvGameModeBase(const FObjectInitializer& ObjectInitializer);

   const USirvEpisode &GetSirvEpisode() const
   {
     check(Episode != nullptr);
     return *Episode;
   }

  /* const boost::optional<sirv::road::Map>& GetMap() const {
    return Map;
  } */

  /* UFUNCTION(Exec, Category = "SIRV Game Mode")
  void DebugShowSignals(bool enable); */

  /* UFUNCTION(BlueprintCallable, Category = "SIRV Game Mode")
  ATrafficLightManager* GetTrafficLightManager(); */

   UFUNCTION(Category = "Sirv Game Mode", BlueprintCallable, CallInEditor, Exec)
  TArray<FBoundingBox> GetAllBBsOfLevel(uint8 TagQueried = 0xFF) const;
 
  UFUNCTION(Category = "Sirv Game Mode", BlueprintCallable, CallInEditor, Exec)
  TArray<FEnvironmentObject> GetEnvironmentObjects(uint8 QueriedTag = 0xFF) const
  {
    return ObjectRegister->GetEnvironmentObjects(QueriedTag);
  }

   void EnableEnvironmentObjects(const TSet<uint64>& EnvObjectIds, bool Enable);
 
   UFUNCTION(Category = "Sirv Game Mode", BlueprintCallable, CallInEditor, Exec)
  void LoadMapLayer(int32 MapLayers);

  /* UFUNCTION(Category = "Sirv Game Mode", BlueprintCallable, CallInEditor, Exec)
  void UnLoadMapLayer(int32 MapLayers); */

  /* UFUNCTION(Category = "Sirv Game Mode")
  ULevel* GetULevelFromName(FString LevelName); */

   UFUNCTION(BlueprintCallable, Category = "Sirv Game Mode")
  void OnLoadStreamLevel(); 

   UFUNCTION(BlueprintCallable, Category = "Sirv Game Mode")
  void OnUnloadStreamLevel();

protected:

  void InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage) override;

  void RestartPlayer(AController *NewPlayer) override;

  void BeginPlay() override;

  void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  void Tick(float DeltaSeconds) override;

private:

  void SpawnActorFactories();

  //void ParseOpenDrive(const FString &MapName);

  void RegisterEnvironmentObjects();

  //void ConvertMapLayerMaskToMapNames(int32 MapLayer, TArray<FName>& OutLevelNames);

  void OnEpisodeSettingsChanged(const FEpisodeSettings &Settings);

   UPROPERTY()
  USirvGameInstance *GameInstance = nullptr;

   UPROPERTY()
  UTaggerDelegate *TaggerDelegate = nullptr;

  UPROPERTY()
  USirvSettingsDelegate *SirvSettingsDelegate = nullptr;

  UPROPERTY()
  USirvEpisode *Episode = nullptr;

  //UPROPERTY()
  //ASirvRecorder *Recorder = nullptr;

   UPROPERTY()
  UObjectRegister* ObjectRegister = nullptr;

  /// The class of Weather to spawn.
    UPROPERTY(Category = "SIRV Game Mode", EditAnywhere)
   TSubclassOf<AWeather> WeatherClass;

  //std::shared_ptr<sirv_ros::SirvRosHandler> ros;
  //TSharedPtr<sirv_ros::SirvRosHandler> ros;
  //TSharedPtr<rclcpp::Node> node;

  //sirv_ros::SirvRosHandler ros;
  //std::shared_ptr<rclcpp::Node> node;

  /// List of actor spawners that will be used to define and spawn the actors
  /// available in game.
  UPROPERTY(Category = "SIRV Game Mode", EditAnywhere)
  TSet<TSubclassOf<ASirvActorFactory>> ActorFactories;

   UPROPERTY()
  TArray<ASirvActorFactory *> ActorFactoryInstances;

  /* UPROPERTY()
  ATrafficLightManager* TrafficLightManager = nullptr; */

  FDelegateHandle OnEpisodeSettingsChangeHandle;

  /* boost::optional<sirv::road::Map> Map; */

  int PendingLevelsToLoad = 0;
  int PendingLevelsToUnLoad = 0;

  bool ReadyToRegisterObjects = false;

};
