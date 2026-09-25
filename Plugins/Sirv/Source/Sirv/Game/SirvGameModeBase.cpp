// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#include "Game/SirvGameModeBase.h"
#include "Sirv.h"

#include "Game/SirvHUD.h"
#include "Engine/DecalActor.h"
#include "Engine/LevelStreaming.h"

#include <compiler/disable-ue4-macros.h>
//#include "sirv/opendrive/OpenDriveParser.h"
//#include "sirv/road/element/RoadInfoSignal.h"
#include <sirv/rpc/EnvironmentObject.h>
#include <sirv/rpc/WeatherParameters.h>
//#include <sirv/rpc/MapLayer.h>
//#include <memory>

#include <compiler/enable-ue4-macros.h>

#include "Async/ParallelFor.h"
#include "DynamicRHI.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"


//namespace cr = sirv::road;
//namespace crp = sirv::rpc;
//namespace cre = sirv::road::element;

ASirvGameModeBase::ASirvGameModeBase(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickGroup = TG_PrePhysics;
  bAllowTickBeforeBeginPlay = false;


  static int num=0;
  FString IntAsString = FString::FromInt(++num);
  
  
  Episode = CreateDefaultSubobject<USirvEpisode>(TEXT("Episode_{%d}"),num);
 
  //UE_LOG(LogSirv, Log, TEXT("Corrected NUM %d "), num);
  //UE_LOG(LogSirv, Log, TEXT(" GameBase EPISODE POINTER %p "), Episode);

  //Recorder = CreateDefaultSubobject<ASirvRecorder>(TEXT("Recorder"));
      
  ObjectRegister = CreateDefaultSubobject<UObjectRegister>(TEXT("ObjectRegister"));

  // HUD
  HUDClass = ASirvHUD::StaticClass();

  TaggerDelegate = CreateDefaultSubobject<UTaggerDelegate>(TEXT("TaggerDelegate"));
  SirvSettingsDelegate = CreateDefaultSubobject<USirvSettingsDelegate>(TEXT("SirvSettingsDelegate"));
}

void ASirvGameModeBase::InitGame(
    const FString &MapName,
    const FString &Options,
    FString &ErrorMessage)
{
  Super::InitGame(MapName, Options, ErrorMessage);


   checkf(
      Episode != nullptr,
      TEXT("Missing episode, can't continue without an episode!"));

#if WITH_EDITOR
    {
      // When playing in editor the map name gets an extra prefix, here we
      // remove it.
      FString CorrectedMapName = MapName;
      constexpr auto PIEPrefix = TEXT("UEDPIE_0_");
      CorrectedMapName.RemoveFromStart(PIEPrefix);
      UE_LOG(LogSirv, Log, TEXT("Corrected map name from %s to %s"), *MapName, *CorrectedMapName);
      Episode->MapName = CorrectedMapName;
    }
#else
  Episode->MapName = MapName;
#endif // WITH_EDITOR

  auto World = GetWorld();
  check(World != nullptr);

   GameInstance = Cast<USirvGameInstance>(GetGameInstance());
  checkf(
      GameInstance != nullptr,
      TEXT("GameInstance is not a USirvGameInstance, did you forget to set "
           "it in the project settings?"));

   if (TaggerDelegate != nullptr) {
    TaggerDelegate->RegisterSpawnHandler(World);
  } else {
    UE_LOG(LogSirv, Error, TEXT("Missing TaggerDelegate!"));
  }

  if(SirvSettingsDelegate != nullptr) {
    SirvSettingsDelegate->ApplyQualityLevelPostRestart();
    SirvSettingsDelegate->RegisterSpawnHandler(World);
  } else {
    UE_LOG(LogSirv, Error, TEXT("Missing SirvSettingsDelegate!"));
  }

    if (WeatherClass != nullptr) {
     Episode->Weather = World->SpawnActor<AWeather>(WeatherClass);
   } else {
     UE_LOG(LogSirv, Error, TEXT("Missing weather class!"));
   } 

   GameInstance->NotifyInitGame();

  OnEpisodeSettingsChangeHandle = FSirvStaticDelegates::OnEpisodeSettingsChange.AddUObject(
        this,
        &ASirvGameModeBase::OnEpisodeSettingsChanged); 
  

  SpawnActorFactories();

  // make connection between Episode and Recorder
  //Recorder->SetEpisode(Episode);
  //Episode->SetRecorder(Recorder);

  //ParseOpenDrive(MapName);
  
}

void ASirvGameModeBase::RestartPlayer(AController *NewPlayer)
{
  if (SirvSettingsDelegate != nullptr)
  {
    SirvSettingsDelegate->ApplyQualityLevelPreRestart();
  }

  Super::RestartPlayer(NewPlayer);
}

void ASirvGameModeBase::BeginPlay()
{
  Super::BeginPlay();

  UE_LOG(LogSirv, Log, TEXT(" ::: ASirvGameModeBase::BeginPlay...  :::::::::::::::::::::::::::::::::::::::: "));

  if(GEngine)
	{
    UE_LOG(LogSirv, Log, TEXT(" ::: Changing the WINDOWS POSITION... "));
		UGameUserSettings* MyGameSettings = GEngine->GetGameUserSettings();
		MyGameSettings->SetScreenResolution(FIntPoint(1920,1080));
    MyGameSettings->SetWindowPosition(3840,0);
		MyGameSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);

		MyGameSettings->SetVSyncEnabled(true);
		MyGameSettings->ApplySettings(true);
    UE_LOG(LogSirv, Log, TEXT(" ::: Changed the WINDOWS POSITION... "));
	}

  //LoadMapLayer(GameInstance->GetCurrentMapLayer());
  ReadyToRegisterObjects = true;

  if (true) { /// @todo If semantic segmentation enabled.
    check(GetWorld() != nullptr);
    ATagger::TagActorsInLevel(*GetWorld(), true);
    TaggerDelegate->SetSemanticSegmentationEnabled();
  } 

  // HACK: fix transparency see-through issues
  // The problem: transparent objects are visible through walls.
  // This is due to a weird interaction between the SkyAtmosphere component,
  // the shadows of a directional light (the sun)
  // and the custom depth set to 3 used for semantic segmentation
  // The solution: Spawn a Decal.
  // It just works!
  /* GetWorld()->SpawnActor<ADecalActor>(
      FVector(0,0,-1000000), FRotator(0,0,0), FActorSpawnParameters()); */

 /*  ATrafficLightManager* Manager = GetTrafficLightManager();
  Manager->InitializeTrafficLights(); */

   Episode->InitializeAtBeginPlay();
  GameInstance->NotifyBeginEpisode(*Episode);

    if (Episode->Weather != nullptr)
   {
     Episode->Weather->ApplyWeather(sirv::rpc::WeatherParameters::Default);
   } 

  /// @todo Recorder should not tick here, FSirvEngine should do it.
  // check if replayer is waiting to autostart
  // if (Recorder)
  // {
  //   Recorder->GetReplayer()->CheckPlayAfterMapLoaded();
  // }

   if(ReadyToRegisterObjects && PendingLevelsToLoad == 0)
  {
    RegisterEnvironmentObjects();
  } 
}

void ASirvGameModeBase::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);

  /// @todo Recorder should not tick here, FSirvEngine should do it.
  // if (Recorder)
  // {
  //   Recorder->Tick(DeltaSeconds);
  // }
}

void ASirvGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  FSirvStaticDelegates::OnEpisodeSettingsChange.Remove(OnEpisodeSettingsChangeHandle);

   Episode->EndPlay();
  GameInstance->NotifyEndEpisode();

  Super::EndPlay(EndPlayReason);

  if ((SirvSettingsDelegate != nullptr) && (EndPlayReason != EEndPlayReason::EndPlayInEditor))
  {
    SirvSettingsDelegate->Reset();
  }
}


void ASirvGameModeBase::SpawnActorFactories()
{
  auto *World = GetWorld();
  check(World != nullptr);

  for (auto &FactoryClass : ActorFactories)
  {
    if (FactoryClass != nullptr)
    {
      auto *Factory = World->SpawnActor<ASirvActorFactory>(FactoryClass);
      if (Factory != nullptr)
      {
        Episode->RegisterActorFactory(*Factory);
        ActorFactoryInstances.Add(Factory);
      }
      else
      {
        UE_LOG(LogSirv, Error, TEXT("Failed to spawn actor spawner"));
      }
    }
  }
}
 
/* 
void ASirvGameModeBase::ParseOpenDrive(const FString &MapName)
{
  std::string opendrive_xml = sirv::rpc::FromLongFString(UOpenDrive::LoadXODR(MapName));
  Map = sirv::opendrive::OpenDriveParser::Load(opendrive_xml);
  if (!Map.has_value()) {
    UE_LOG(LogSirv, Error, TEXT("Invalid Map"));
  }
  else
  {
    Episode->MapGeoReference = Map->GetGeoReference();
  }
}
 */
/* 
ATrafficLightManager* ASirvGameModeBase::GetTrafficLightManager()
{
  if (!TrafficLightManager)
  {
    UWorld* World = GetWorld();
    AActor* TrafficLightManagerActor = UGameplayStatics::GetActorOfClass(World, ATrafficLightManager::StaticClass());
    if(TrafficLightManagerActor == nullptr)
    {
      FActorSpawnParameters SpawnParams;
      SpawnParams.OverrideLevel = GetULevelFromName("TrafficLights");
      TrafficLightManager = World->SpawnActor<ATrafficLightManager>(SpawnParams);
    }
    else
    {
      TrafficLightManager = Cast<ATrafficLightManager>(TrafficLightManagerActor);
    }
  }
  return TrafficLightManager;
}
 */

/* 
void ASirvGameModeBase::DebugShowSignals(bool enable)
{

  auto World = GetWorld();
  check(World != nullptr);

  if(!Map)
  {
    return;
  }

  if(!enable)
  {
    UKismetSystemLibrary::FlushDebugStrings(World);
    UKismetSystemLibrary::FlushPersistentDebugLines(World);
    return;
  }

  //const std::unordered_map<sirv::road::SignId, std::unique_ptr<sirv::road::Signal>>
  const auto& Signals = Map->GetSignals();
  const auto& Controllers = Map->GetControllers();

  for(const auto& Signal : Signals) {
    const auto& ODSignal = Signal.second;
    const FTransform Transform = ODSignal->GetTransform();
    const FVector Location = Transform.GetLocation();
    const FQuat Rotation = Transform.GetRotation();
    const FVector Up = Rotation.GetUpVector();
    DrawDebugSphere(
      World,
      Location,
      50.0f,
      10,
      FColor(0, 255, 0),
      true
    );
  }

  TArray<const cre::RoadInfoSignal*> References;
  auto waypoints = Map->GenerateWaypointsOnRoadEntries();
  std::unordered_set<cr::RoadId> ExploredRoads;
  for (auto & waypoint : waypoints)
  {
    // Check if we already explored this road
    if (ExploredRoads.count(waypoint.road_id) > 0)
    {
      continue;
    }
    ExploredRoads.insert(waypoint.road_id);

    // Multiple times for same road (performance impact, not in behavior)
    auto SignalReferences = Map->GetLane(waypoint).
        GetRoad()->GetInfos<cre::RoadInfoSignal>();
    for (auto *SignalReference : SignalReferences)
    {
      References.Add(SignalReference);
    }
  }
  for (auto& Reference : References)
  {
    auto RoadId = Reference->GetRoadId();
    const auto* SignalReference = Reference;
    const FTransform SignalTransform = SignalReference->GetSignal()->GetTransform();
    for(auto &validity : SignalReference->GetValidities())
    {
      for(auto lane : sirv::geom::Math::GenerateRange(validity._from_lane, validity._to_lane))
      {
        if(lane == 0)
          continue;

        auto signal_waypoint = Map->GetWaypoint(
            RoadId, lane, SignalReference->GetS()).get();

        if(Map->GetLane(signal_waypoint).GetType() != cr::Lane::LaneType::Driving)
          continue;

        FTransform ReferenceTransform = Map->ComputeTransform(signal_waypoint);

        DrawDebugSphere(
            World,
            ReferenceTransform.GetLocation(),
            50.0f,
            10,
            FColor(0, 0, 255),
            true
        );

        DrawDebugLine(
            World,
            ReferenceTransform.GetLocation(),
            SignalTransform.GetLocation(),
            FColor(0, 0, 255),
            true
        );
      }
    }
  }

}
 */

TArray<FBoundingBox> ASirvGameModeBase::GetAllBBsOfLevel(uint8 TagQueried) const
{
  UWorld* World = GetWorld();

  // Get all actors of the level
  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), FoundActors);

  TArray<FBoundingBox> BoundingBoxes;
  BoundingBoxes = UBoundingBoxCalculator::GetBoundingBoxOfActors(FoundActors, TagQueried);

  return BoundingBoxes;
}

void ASirvGameModeBase::RegisterEnvironmentObjects()
{
  // Get all actors of the level
  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
  ObjectRegister->RegisterObjects(FoundActors);
}
 

void ASirvGameModeBase::EnableEnvironmentObjects(
  const TSet<uint64>& EnvObjectIds,
  bool Enable)
{
  ObjectRegister->EnableEnvironmentObjects(EnvObjectIds, Enable);
}
 
 
void ASirvGameModeBase::LoadMapLayer(int32 MapLayers)
{
  const UWorld* World = GetWorld();

  TArray<FName> LevelsToLoad;
  //ConvertMapLayerMaskToMapNames(MapLayers, LevelsToLoad);

  FLatentActionInfo LatentInfo;
  LatentInfo.CallbackTarget = this;
  LatentInfo.ExecutionFunction = "OnLoadStreamLevel";
  LatentInfo.Linkage = 0;
  LatentInfo.UUID = 1;

  PendingLevelsToLoad = LevelsToLoad.Num();

  for(FName& LevelName : LevelsToLoad)
  {
    UGameplayStatics::LoadStreamLevel(World, LevelName, true, true, LatentInfo);
    LatentInfo.UUID++;
  }

}
 
/* 
void ASirvGameModeBase::UnLoadMapLayer(int32 MapLayers)
{
  const UWorld* World = GetWorld();

  TArray<FName> LevelsToUnLoad;
  ConvertMapLayerMaskToMapNames(MapLayers, LevelsToUnLoad);

  FLatentActionInfo LatentInfo;
  LatentInfo.CallbackTarget = this;
  LatentInfo.ExecutionFunction = "OnUnloadStreamLevel";
  LatentInfo.UUID = 1;
  LatentInfo.Linkage = 0;

  PendingLevelsToUnLoad = LevelsToUnLoad.Num();

  for(FName& LevelName : LevelsToUnLoad)
  {
    UGameplayStatics::UnloadStreamLevel(World, LevelName, LatentInfo, false);
    LatentInfo.UUID++;
  }

}
 */

// void ASirvGameModeBase::ConvertMapLayerMaskToMapNames(int32 MapLayer, TArray<FName>& OutLevelNames)
// {
//   UWorld* World = GetWorld();
//   const TArray <ULevelStreaming*> Levels = World->GetStreamingLevels();
//   TArray<FString> LayersToLoad;

//   // Get all the requested layers
//   int32 LayerMask = 1;
//   int32 AllLayersMask = static_cast<crp::MapLayerType>(crp::MapLayer::All);

//   while(LayerMask > 0)
//   {
//     // Convert enum to FString
//     FString LayerName = UTF8_TO_TCHAR(MapLayerToString(static_cast<crp::MapLayer>(LayerMask)).c_str());
//     bool included = static_cast<crp::MapLayerType>(MapLayer) & LayerMask;
//     if(included)
//     {
//       LayersToLoad.Emplace(LayerName);
//     }
//     LayerMask = (LayerMask << 1) & AllLayersMask;
//   }

//   // Get all the requested level maps
//   for(ULevelStreaming* Level : Levels)
//   {
//     TArray<FString> StringArray;
//     FString FullSubMapName = Level->GetWorldAssetPackageFName().ToString();
//     // Discard full path, we just need the umap name
//     FullSubMapName.ParseIntoArray(StringArray, TEXT("/"), false);
//     FString SubMapName = StringArray[StringArray.Num() - 1];
//     for(FString LayerName : LayersToLoad)
//     {
//       if(SubMapName.Contains(LayerName))
//       {
//         OutLevelNames.Emplace(FName(*SubMapName));
//         break;
//       }
//     }
//   }

// }
/* 
ULevel* ASirvGameModeBase::GetULevelFromName(FString LevelName)
{
  ULevel* OutLevel = nullptr;
  UWorld* World = GetWorld();
  const TArray <ULevelStreaming*> Levels = World->GetStreamingLevels();

  for(ULevelStreaming* Level : Levels)
  {
    FString FullSubMapName = Level->GetWorldAssetPackageFName().ToString();
    if(FullSubMapName.Contains(LevelName))
    {
      OutLevel = Level->GetLoadedLevel();
      if(!OutLevel)
      {
        UE_LOG(LogSirv, Warning, TEXT("%s has not been loaded"), *LevelName);
      }
      break;
    }
  }

  return OutLevel;
}
 */

void ASirvGameModeBase::OnLoadStreamLevel()
{
  PendingLevelsToLoad--;

  // Register new actors and tag them
  if(ReadyToRegisterObjects && PendingLevelsToLoad == 0)
  {
    RegisterEnvironmentObjects();
    ATagger::TagActorsInLevel(*GetWorld(), true);
  }
}


void ASirvGameModeBase::OnUnloadStreamLevel()
{
  PendingLevelsToUnLoad--;
  // Update stored registered objects (discarding the deleted objects)
  if(ReadyToRegisterObjects && PendingLevelsToUnLoad == 0)
  {
    RegisterEnvironmentObjects();
  }
}
 

void ASirvGameModeBase::OnEpisodeSettingsChanged(const FEpisodeSettings &Settings)
{
  SirvSettingsDelegate->SetAllActorsDrawDistance(GetWorld(), Settings.MaxCullingDistance);
} 
