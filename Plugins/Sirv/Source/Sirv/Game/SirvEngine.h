// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once



//#include "Sirv/Recorder/SirvRecorder.h"
//include "Sirv/Sensor/WorldObserver.h"
#include "Ros/SirvRosBinder.h"
#include "Game/SirvEpisode.h"
#include "Settings/EpisodeSettings.h"
#include "Util/NonCopyable.h"

#include "Misc/CoreDelegates.h"

#include "CoreMinimal.h"

#include "SirvEngine.generated.h"

class USirvSettings;
struct FEpisodeSettings;


class USirvEngine;

class FSirvEngineLogic
{

  public:

    FSirvEngineLogic(USirvEngine *_engine);
    ~FSirvEngineLogic();

    //friend USirvEngine;

    USirvEngine *engine;
    
    void OnPreTick(UWorld *World, ELevelTick TickType, float DeltaSeconds);

    void OnPostTick(UWorld *World, ELevelTick TickType, float DeltaSeconds);

    void OnEpisodeSettingsChanged(const FEpisodeSettings &Settings);


};


//class USirvEngine : private NonCopyable
UCLASS()
class USirvEngine : public UObject
{

  GENERATED_BODY()

public:

  static uint64_t FrameCounter;

  USirvEngine(const FObjectInitializer &ObjectInitializer);
  ~USirvEngine();

  void NotifyInitGame(const USirvSettings &Settings);

  void NotifyBeginEpisode(USirvEpisode &Episode);

  void NotifyEndEpisode();

  // const FSirvServer &GetServer() const
  // {
  //   return Server;
  // }
     const USirvRosBinder *GetRosNode() const
    {
        return Ros;
    }
    


  USirvEpisode *GetCurrentEpisode()
  {
    return CurrentEpisode;
  }

  // void SetRecorder(ASirvRecorder *InRecorder)
  // {
  //   Recorder = InRecorder;
  // }

  static uint64_t GetFrameCounter()
  { 
    return USirvEngine::FrameCounter;
  }

  static uint64_t UpdateFrameCounter() 
  { 
    USirvEngine::FrameCounter += 1;
    return USirvEngine::FrameCounter;
  }

  static void ResetFrameCounter(uint64_t Value = 0)
  { 
    USirvEngine::FrameCounter = Value;
  }

private:

   friend FSirvEngineLogic;

   TSharedPtr<FSirvEngineLogic> logic = nullptr;
  //void OnPreTick(UWorld *World, ELevelTick TickType, float DeltaSeconds);

  //void OnPostTick(UWorld *World, ELevelTick TickType, float DeltaSeconds);

  //void OnEpisodeSettingsChanged(const FEpisodeSettings &Settings);

  void ResetSimulationState();

  bool bIsRunning = false;

  bool bSynchronousMode = false;

  bool bMapChanged = false;

  //FSirvServer Server;

  UPROPERTY()
  USirvRosBinder *Ros = nullptr;
  
  //FWorldObserver WorldObserver;
  
  UPROPERTY()
  USirvEpisode *CurrentEpisode = nullptr;

  FEpisodeSettings CurrentSettings;

  //ASirvRecorder *Recorder = nullptr;

  FDelegateHandle OnPreTickHandle;

  FDelegateHandle OnPostTickHandle;

  FDelegateHandle OnEpisodeSettingsChangeHandle;
};
