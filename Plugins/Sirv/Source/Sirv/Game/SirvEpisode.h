// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Actor/ActorDispatcher.h"
//#include "Sirv/Recorder/SirvRecorder.h"
//#include "Sirv/Sensor/WorldObserver.h"
//#include "Sirv/Server/SirvServer.h"
#include "Sirv/Settings/EpisodeSettings.h"
#include "Sirv/Util/ActorAttacher.h"
#include "Sirv/Weather/Weather.h"

#include "GameFramework/Pawn.h"

#include <compiler/disable-ue4-macros.h>
#include <sirv/Logging.h>
#include <sirv/geom/BoundingBox.h>
#include <sirv/geom/GeoLocation.h>
#include <sirv/rpc/Actor.h>
#include <sirv/rpc/ActorDescription.h>
#include <sirv/rpc/OpendriveGenerationParameters.h>
//#include <sirv/streaming/Server.h>
#include <compiler/enable-ue4-macros.h>

#include "SirvEpisode.generated.h"

/// A simulation episode.
///
/// Each time the level is restarted a new episode is created.
UCLASS(BlueprintType, Blueprintable)
class SIRV_API USirvEpisode : public UObject
{
  GENERATED_BODY()

  // ===========================================================================
  // -- Constructor ------------------------------------------------------------
  // ===========================================================================

public:

  USirvEpisode(const FObjectInitializer &ObjectInitializer);

  // ===========================================================================
  // -- Load a new episode -----------------------------------------------------
  // ===========================================================================

public:

  /// Load a new map and start a new episode.
  ///
  /// If @a MapString is empty, the current map is reloaded.
  UFUNCTION(BlueprintCallable)
  bool LoadNewEpisode(const FString &MapString, bool reset_settings = true);

  /// Load a new map generating the mesh from OpenDRIVE data and
  /// start a new episode.
  ///
  /// If @a MapString is empty, it fails.
 /*  bool LoadNewOpendriveEpisode(
      const FString &OpenDriveString,
      const sirv::rpc::OpendriveGenerationParameters &Params);
 */
  // ===========================================================================
  // -- Episode settings -------------------------------------------------------
  // ===========================================================================

public:

  UFUNCTION(BlueprintCallable)
  const FEpisodeSettings &GetSettings() const
  {
    return EpisodeSettings;
  }

  UFUNCTION(BlueprintCallable)
  void ApplySettings(const FEpisodeSettings &Settings);

  // ===========================================================================
  // -- Retrieve info about this episode ---------------------------------------
  // ===========================================================================

public:

  /// Return the unique id of this episode.
  auto GetId() const
  {
    return Id;
  }

  /// Return the name of the map loaded in this episode.
  UFUNCTION(BlueprintCallable)
  const FString &GetMapName() const
  {
    return MapName;
  }

  /// Game seconds since the start of this episode.
  double GetElapsedGameTime() const
  {
    return ElapsedGameTime;
  }

  /// Return the list of actor definitions that are available to be spawned this
  /// episode.
  UFUNCTION(BlueprintCallable)
  const TArray<FActorDefinition> &GetActorDefinitions() const
  {
    return ActorDispatcher->GetActorDefinitions();
  }

   /// Return the list of recommended spawn points for vehicles.
  UFUNCTION(BlueprintCallable)
  TArray<FTransform> GetRecommendedSpawnPoints() const;
 
  /// Return the GeoLocation point of the map loaded
  const sirv::geom::GeoLocation &GetGeoReference() const
  {
    return MapGeoReference;
  }

  // ===========================================================================
  // -- Retrieve special actors ------------------------------------------------
  // ===========================================================================

public:

  UFUNCTION(BlueprintCallable)
  APawn *GetSpectatorPawn() const
  {
    return Spectator;
  }

   UFUNCTION(BlueprintCallable)
  AWeather *GetWeather() const
  {
    return Weather;
  } 

  const FActorRegistry &GetActorRegistry() const
  {
    return ActorDispatcher->GetActorRegistry();
  }

  // ===========================================================================
  // -- Actor look up methods --------------------------------------------------
  // ===========================================================================

public:

  /// Find a Sirv actor by id.
  ///
  /// If the actor is not found or is pending kill, the returned view is
  /// invalid.
  FActorView FindActor(FActorView::IdType ActorId) const
  {
     checkf(
      ActorDispatcher != nullptr,
      TEXT("ActorDispatcher is NULL"));

    return ActorDispatcher->GetActorRegistry().Find(ActorId);
  }

  /// Find the actor view of @a Actor.
  ///
  /// If the actor is not found or is pending kill, the returned view is
  /// invalid.
  FActorView FindActor(AActor *Actor) const
  {
    return ActorDispatcher->GetActorRegistry().Find(Actor);
  }

  /// Find the actor view of @a Actor. If the actor is not found, a "fake" view
  /// is returned emulating an existing Sirv actor. Use this to return views
  /// over static actors present in the map.
  ///
  /// If the actor is pending kill, the returned view is invalid.
  FActorView FindOrFakeActor(AActor *Actor) const
  {
    return ActorDispatcher->GetActorRegistry().FindOrFake(Actor);
  }

  // ===========================================================================
  // -- Actor handling methods -------------------------------------------------
  // ===========================================================================

public:

  /// Spawns an actor based on @a ActorDescription at @a Transform. To properly
  /// despawn an actor created with this function call DestroyActor.
  ///
  /// @return A pair containing the result of the spawn function and a view over
  /// the actor and its properties. If the status is different of Success the
  /// view is invalid.
  TPair<EActorSpawnResultStatus, FActorView> SpawnActorWithInfo(
      const FTransform &Transform,
      FActorDescription thisActorDescription,
      FActorView::IdType DesiredId = 0)
  {
    auto result = ActorDispatcher->SpawnActor(Transform, thisActorDescription, DesiredId);
    // if (Recorder->IsEnabled())
    // {
    //   if (result.Key == EActorSpawnResultStatus::Success)
    //   {
    //     Recorder->CreateRecorderEventAdd(
    //       result.Value.GetActorId(),
    //       static_cast<uint8_t>(result.Value.GetActorType()),
    //       Transform,
    //       std::move(thisActorDescription)
    //     );
    //   }
    // }

    return result;
  }

  /// Spawns an actor based on @a ActorDescription at @a Transform. To properly
  /// despawn an actor created with this function call DestroyActor.
  ///
  /// @return nullptr on failure.
  ///
  /// @note Special overload for blueprints.
  UFUNCTION(BlueprintCallable)
  AActor *SpawnActor(
      const FTransform &Transform,
      FActorDescription ActorDescription)
  {
    return SpawnActorWithInfo(Transform, std::move(ActorDescription)).Value.GetActor();
  }

  /// Attach @a Child to @a Parent.
  ///
  /// @pre Actors cannot be null.
  UFUNCTION(BlueprintCallable)
  void AttachActors(
      AActor *Child,
      AActor *Parent,
      EAttachmentType InAttachmentType = EAttachmentType::Rigid);

  /// @copydoc FActorDispatcher::DestroyActor(AActor*)
  UFUNCTION(BlueprintCallable)
  bool DestroyActor(AActor *Actor)
  {
      // if (Recorder->IsEnabled())
      // {
      //   // recorder event
      //   // SirvRecorderEventDel RecEvent
      //   // {
      //   //   GetActorRegistry().Find(Actor).GetActorId()
      //   // };
      //   // Recorder->AddEvent(std::move(RecEvent));
      // }

    return ActorDispatcher->DestroyActor(Actor);
  }

  // ===========================================================================
  // -- Other methods ----------------------------------------------------------
  // ===========================================================================

public:

  /// Create a serializable object describing the actor.
  sirv::rpc::Actor SerializeActor(FActorView ActorView) const;

  // ===========================================================================
  // -- Private methods and members --------------------------------------------
  // ===========================================================================

  // ASirvRecorder *GetRecorder() const
  // {
  //   return Recorder;
  // }

  // void SetRecorder(ASirvRecorder *Rec)
  // {
  //   Recorder = Rec;
  // }

  // SirvReplayer *GetReplayer() const
  // {
  //   return Recorder->GetReplayer();
  // }

  // std::string StartRecorder(std::string name, bool AdditionalData);

private:

  friend class ASirvGameModeBase;
  friend class USirvEngine;
  friend class FSirvEngineLogic;
  
  void InitializeAtBeginPlay();

  void EndPlay();

  void RegisterActorFactory(ASirvActorFactory &ActorFactory)
  {
    ActorDispatcher->Bind(ActorFactory);
  }

  std::pair<int, FActorView&> TryToCreateReplayerActor(
    FVector &Location,
    FVector &Rotation,
    FActorDescription &ActorDesc,
    unsigned int desiredId);

  bool SetActorSimulatePhysics(FActorView &ActorView, bool bEnabled);

  void TickTimers(float DeltaSeconds)
  {
    ElapsedGameTime += DeltaSeconds;
  }

  const uint64 Id = 0u;

  double ElapsedGameTime = 0.0;

  UPROPERTY(VisibleAnywhere)
  FString MapName;

  UPROPERTY(VisibleAnywhere)
  FEpisodeSettings EpisodeSettings;

  UPROPERTY(VisibleAnywhere)
  UActorDispatcher *ActorDispatcher = nullptr;

  UPROPERTY(VisibleAnywhere)
  APawn *Spectator = nullptr;

  UPROPERTY(VisibleAnywhere)
   AWeather *Weather = nullptr;
 
  //ASirvRecorder *Recorder = nullptr;

  sirv::geom::GeoLocation MapGeoReference;
};
