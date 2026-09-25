// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#include "Game/SirvEngine.h"
#include "Sirv.h"


#include "Game/SirvStaticDelegates.h"
//#include "Lights/SirvLightSubsystem.h"
//#include "Sirv/Recorder/SirvRecorder.h"
#include "Settings/SirvSettings.h"
#include "Settings/EpisodeSettings.h"

#include "Runtime/Core/Public/Misc/App.h"
#include "PhysicsEngine/PhysicsSettings.h"

#include <thread>

// =============================================================================
// -- Static local methods -----------------------------------------------------
// =============================================================================

// init static frame counter
uint64_t USirvEngine::FrameCounter = 0;

static uint32 USirvEngine_GetNumberOfThreadsForRPCServer()
{
  return std::max(std::thread::hardware_concurrency(), 4u) - 2u;
}

static TOptional<double> USirvEngine_GetFixedDeltaSeconds()
{
  return FApp::IsBenchmarking() ? FApp::GetFixedDeltaTime() : TOptional<double>{};
}

static void USirvEngine_SetFixedDeltaSeconds(TOptional<double> FixedDeltaSeconds)
{
  FApp::SetBenchmarking(FixedDeltaSeconds.IsSet());
  FApp::SetFixedDeltaTime(FixedDeltaSeconds.Get(0.0));
}

// =============================================================================
// -- USirvEngine -------------------------------------------------------------
// =============================================================================
USirvEngine::USirvEngine(const FObjectInitializer &ObjectInitializer)
: Super(ObjectInitializer)
{
    Ros = CreateDefaultSubobject<USirvRosBinder>(TEXT("Ros"));
    logic = MakeShareable(new FSirvEngineLogic(this));
}

USirvEngine::~USirvEngine()
{
  if (bIsRunning)
  {
    FWorldDelegates::OnWorldTickStart.Remove(OnPreTickHandle);
    FWorldDelegates::OnWorldPostActorTick.Remove(OnPostTickHandle);
    FSirvStaticDelegates::OnEpisodeSettingsChange.Remove(OnEpisodeSettingsChangeHandle);
  }
}

void USirvEngine::NotifyInitGame(const USirvSettings &Settings)
{

  
  if (!bIsRunning)
  {
    //const auto StreamingPort = Settings.StreamingPort.Get(Settings.RPCPort + 1u);
    //auto BroadcastStream = Server.Start(Settings.RPCPort, StreamingPort);
    //Server.AsyncRun(USirvEngine_GetNumberOfThreadsForRPCServer());

    check(Ros != nullptr);

    UE_LOG(LogSirv, Log, TEXT("NotifyInitGame  THIS OBJECT POINTER %p "), Ros);
    Ros->Start();


    //WorldObserver.SetStream(BroadcastStream);

    OnPreTickHandle = FWorldDelegates::OnWorldTickStart.AddRaw(
         logic.Get(),
         &FSirvEngineLogic::OnPreTick);

    // OnPreTickHandle = FWorldDelegates::OnWorldTickStart.AddRaw(
    //     this,
    //     &USirvEngine::OnPreTick);
    
     OnPostTickHandle = FWorldDelegates::OnWorldPostActorTick.AddRaw(
         logic.Get(),
         &FSirvEngineLogic::OnPostTick);
    
    // OnPostTickHandle = FWorldDelegates::OnWorldPostActorTick.AddRaw(
    //     this,
    //     &USirvEngine::OnPostTick);
    

    OnEpisodeSettingsChangeHandle = FSirvStaticDelegates::OnEpisodeSettingsChange.AddRaw(
         logic.Get(),
         &FSirvEngineLogic::OnEpisodeSettingsChanged);
    
    // OnEpisodeSettingsChangeHandle = FSirvStaticDelegates::OnEpisodeSettingsChange.AddRaw(
    //     this,
    //     &USirvEngine::OnEpisodeSettingsChanged);

    bIsRunning = true;
  }

  bMapChanged = true;

}

void USirvEngine::NotifyBeginEpisode(USirvEpisode &Episode)
{
  Episode.EpisodeSettings.FixedDeltaSeconds = USirvEngine_GetFixedDeltaSeconds();
  CurrentEpisode = &Episode;

  CurrentEpisode->ApplySettings(CurrentSettings);

  ResetFrameCounter();

  // make connection between Episode and Recorder
  // if (Recorder)
  // {
  //   Recorder->SetEpisode(&Episode);
  //   Episode.SetRecorder(Recorder);
  //   Recorder->GetReplayer()->CheckPlayAfterMapLoaded();
  // }

  //Server.NotifyBeginEpisode(Episode);
  check(Ros != nullptr);

  //UE_LOG(LogSirv, Log, TEXT("NotifyInitGame  THIS OBJECT POINTER %p "), Ros);

  Ros->NotifyBeginEpisode(Episode);
}

void USirvEngine::NotifyEndEpisode()
{
  //Server.NotifyEndEpisode();
  check(Ros != nullptr);
  Ros->NotifyEndEpisode();
  
  CurrentEpisode = nullptr;
}

void USirvEngine::ResetSimulationState()
{
  bMapChanged = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   FSirvEngineLogic Implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////

FSirvEngineLogic::FSirvEngineLogic(USirvEngine *_engine): engine(_engine)
{
}

FSirvEngineLogic::~FSirvEngineLogic() {};



void FSirvEngineLogic::OnPreTick(UWorld *World, ELevelTick TickType, float DeltaSeconds)
{
   //UE_LOG(LogSirv, Log, TEXT(" OnPreTick TickType %d  Paused [%d]"),TickType,World->IsPaused());

   if (TickType == ELevelTick::LEVELTICK_All && !World->IsPaused())
   {
     // update frame counter
     engine->UpdateFrameCounter();
      //Ros.NotifyBeginEpisode(*CurrentEpisode);
      //UE_LOG(LogSirv, Log, TEXT(" OnPreTick EPISODE POINTER %p "), engine->CurrentEpisode);
  
     // process RPC commands
     // do
     // {
     //   Server.RunSome(10u);
     // }
     // while (bSynchronousMode && !Server.TickCueReceived());

     if (engine->CurrentEpisode != nullptr)
     {
       
       engine->CurrentEpisode->TickTimers(DeltaSeconds);
       //UE_LOG(LogSirv, Log, TEXT("DENTRO PRETICK delta %f total %f "), DeltaSeconds, engine->CurrentEpisode->GetElapsedGameTime());
       engine->Ros->publish_timestamp(engine->CurrentEpisode->GetElapsedGameTime());
     }
  }
}

void FSirvEngineLogic::OnPostTick(UWorld *World, ELevelTick TickType, float DeltaSeconds)
{
  // tick the recorder/replayer system
  if (engine->GetCurrentEpisode())
  {
    // auto* EpisodeRecorder = GetCurrentEpisode()->GetRecorder();
    // if (EpisodeRecorder)
    // {
    //   EpisodeRecorder->Ticking(DeltaSeconds);
    // }
  }

  if ((TickType == ELevelTick::LEVELTICK_All) && (engine->CurrentEpisode != nullptr))
  {
    // Look for lightsubsystem
    bool LightUpdatePending = false;
    if (World)
    {
      /* USirvLightSubsystem* SirvLightSubsystem = World->GetSubsystem<USirvLightSubsystem>();
      if (SirvLightSubsystem)
      {
        LightUpdatePending = SirvLightSubsystem->IsUpdatePending();
      } */
    }

    // send the worldsnapshot
    //WorldObserver.BroadcastTick(*CurrentEpisode, DeltaSeconds, bMapChanged, LightUpdatePending);
    engine->ResetSimulationState();
  }
}

void FSirvEngineLogic::OnEpisodeSettingsChanged(const FEpisodeSettings &Settings)
{
  engine->CurrentSettings = FEpisodeSettings(Settings);

  engine->bSynchronousMode = Settings.bSynchronousMode;

  if (GEngine && GEngine->GameViewport)
  {
    GEngine->GameViewport->bDisableWorldRendering = Settings.bNoRenderingMode;
    FVector2D WindowPosition = FVector2D(3840.0f, 0.0f);
    GEngine->GameViewport->GetWindow()->MoveWindowTo(WindowPosition);

  }

  USirvEngine_SetFixedDeltaSeconds(Settings.FixedDeltaSeconds);

  // Setting parameters for physics substepping
  UPhysicsSettings* PhysSett = UPhysicsSettings::Get();
  PhysSett->bSubstepping = Settings.bSubstepping;
  PhysSett->MaxSubstepDeltaTime = Settings.MaxSubstepDeltaTime;
  PhysSett->MaxSubsteps = Settings.MaxSubsteps;
}


