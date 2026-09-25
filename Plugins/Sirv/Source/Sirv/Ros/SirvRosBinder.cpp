// Copyright (c) 2021 Laboratory of Robotics, Intelligent and Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.
// Author: Giovani Bernardes Vitor



#include "Ros/SirvRosBinder.h"
#include "Sirv.h"
 	

#include "UObject/Object.h"

#include "Camera/CameraComponent.h"

//#include "Sirv/OpenDrive/OpenDrive.h"
#include "Util/DebugShapeDrawer.h"
//#include "Util/NavigationMesh.h"
#include "Util/RayTracer.h"
#include "Vehicle/SirvWheeledVehicle.h"
//#include "Sirv/Walker/WalkerController.h"
//#include "Sirv/Walker/WalkerBase.h"

#include "Sensor/SceneCaptureCamera.h"


#include "GameFramework/CharacterMovementComponent.h"
#include "Game/Tagger.h"

#include "Kismet/GameplayStatics.h"



#include <compiler/disable-ue4-macros.h>

#include <stdio.h>

#include <sirv/Functional.h>
#include <sirv/Version.h>
//#include <sirv/rpc/String.h>
#include <sirv/rpc/Actor.h>
#include <sirv/rpc/ActorDefinition.h>
#include <sirv/rpc/ActorDescription.h>
//#include <sirv/rpc/Command.h>
#include <sirv/rpc/CommandResponse.h>
#include <sirv/rpc/DebugShape.h>
#include <sirv/rpc/EnvironmentObject.h>
#include <sirv/rpc/EpisodeInfo.h>
//#include <sirv/rpc/EpisodeSettings.h>
#include "sirv/rpc/LabelledPoint.h"
#include <sirv/rpc/LightState.h>
#include <sirv/rpc/MapInfo.h>
#include <sirv/rpc/MapLayer.h>
#include <sirv/rpc/Response.h>
//#include <sirv_ros/Bind.h>
#include <sirv/rpc/String.h>
#include <sirv/rpc/Transform.h>
#include <sirv/rpc/Vector2D.h>
#include <sirv/rpc/Vector3D.h>
#include <sirv/rpc/VehicleControl.h>
#include <sirv/rpc/VehiclePhysicsControl.h>
#include <sirv/rpc/VehicleLightState.h>
#include <sirv/rpc/VehicleLightStateList.h>
//#include <sirv/rpc/WalkerBoneControl.h>
//#include <sirv/rpc/WalkerControl.h>
#include <sirv/rpc/WeatherParameters.h>
//#include <sirv/streaming/Server.h>
#include <sirv/geom/BoundingBox.h>
//#include <sirv/rpc/VehicleControlCAT793F.h>

//#include <sirv_ros/sirv_ros_handler.hpp>


/////////////////////////////////
// #include <sirv/rpc/VehicleControlCAT793F.h>
// #include <sirv/rpc/PanelControlCAT793F.h>
// #include <sirv/rpc/ActorDefinition.h>


// #include "sirv_msgs/msg/cat793_f_instrument_cluster.hpp"
// #include "sirv_msgs/msg/cat793_f_vehicle_control.hpp"
// #include "sirv_msgs/msg/cat793_f_panel_control.hpp" 

// #include "sirv_msgs/msg/sirv_actor_definition.hpp"

// #include "sirv_msgs/srv/get_blueprints.hpp"
// #include "sirv_msgs/srv/spawn_object.hpp"
// #include "sirv_msgs/srv/destroy_object.hpp"
/////////////////////////////////






#include <compiler/enable-ue4-macros.h>



#include <vector>
#include <map>
#include <tuple>

// =============================================================================
// -- Static local functions ---------------------------------------------------
// =============================================================================

template <typename T, typename Other>
//static std::vector<T> MakeVectorFromTArray(const TArray<Other> &Array)
std::vector<T> MakeVectorFromTArray(const TArray<Other> &Array)
{
  return {Array.GetData(), Array.GetData() + Array.Num()};
}


// =============================================================================
// -- USirvRosBinder::FPimpl -----------------------------------------------
// =============================================================================


// class USirvRosBinder::FPimpl
// {

// public:

//   FPimpl()
//     //: Server(RPCPort),
//       //StreamingServer(StreamingPort),
//       //BroadcastStream(StreamingServer.MakeStream())
//   {
//     BindActions();
//   }

//   /// Map of pairs < port , ip > with all the Traffic Managers active in the simulation
//   //std::map<uint16_t, std::string> TrafficManagerInfo;

//   //sirv_ros::Binder Server;

//   sirv_ros::SirvRosHandler _Ros;
//   //sirv::streaming::Server StreamingServer;

//   //sirv::streaming::Stream BroadcastStream;
//   const USirvEpisode *Episode = nullptr;

//   //UPROPERTY()
//   //TWeakPtr<USirvEpisode> MySharedPointer = nullptr;
//   //TSharedPtr<USirvEpisode> MySharedPointer = nullptr;
  
//   //UPROPERTY()
//   //TSharedRef<USirvEpisode> Myreference;

//   size_t TickCuesReceived = 0u;

// private:

//   void BindActions();
// };

// =============================================================================
// -- Static local variables ---------------------------------------------------
// =============================================================================

//using std::placeholders::_1;

//TWeakObjectPtr<USirvEpisode> USirvRosBinder::static_Episode_;
//TWeakPtr<sirv_ros::SirvRosHandler> USirvRosBinder::static_Ros_;

USirvEpisode *USirvRosBinder::static_Episode_;

template <typename T>
using R = sirv::rpc::Response<T>;

// =============================================================================
// -- Define helper macros -----------------------------------------------------
// =============================================================================

#if WITH_EDITOR
#  define SIRV_ENSURE_GAME_THREAD() check(IsInGameThread());
#else
#  define SIRV_ENSURE_GAME_THREAD()
#endif // WITH_EDITOR

#define RESPOND_ERROR(str) {                                              \
    UE_LOG(LogSirvServer, Log, TEXT("Responding error: %s"), TEXT(str)); }
    //return sirv::rpc::ResponseError(str); }
  

#define RESPOND_ERROR_FSTRING(fstr) {                                 \
    UE_LOG(LogSirvServer, Log, TEXT("Responding error: %s"), *fstr);}

//#define REQUIRE_SIRV_EPISODE() \
//    SIRV_ENSURE_GAME_THREAD();   \
//    if (Episode == nullptr) { RESPOND_ERROR("episode not ready"); }

#define REQUIRE_SIRV_EPISODE() \
    if (static_Episode_ == nullptr) { RESPOND_ERROR("episode not ready"); }


// class RosBinder
// {
// public:

//   constexpr RosBinder(const char *name, sirv_ros::Binder &srv, int option)
//     : _name(name),
//       _server(srv),
//       _option(option) {}

//   template <typename FuncT>
//   auto operator<<(FuncT func)
//   {
//     switch (_option)
//     {
//     case 1:
//         _server.BindBluePrints(_name, func);
//       break;
    
//     case 2:
//          _server.BindSpawnActor(_name, func);
//       break;
//     default:
//       break;
//     }

//     return func;
//   }

// private:

//   const char *_name;

//   sirv_ros::Binder &_server;

//   int _option;
// };

// //#define BIND_SYNC(op,name)   auto name = RosBinder(# name, Server, true)
// //#define BIND_ASYNC(name)  auto name = RosBinder(# name, Server, false)

// #define BIND(op,name)   auto name = RosBinder(# name, Server, op)

// =============================================================================
// -- Callback Functions -------------------------------------------------------------
// =============================================================================

void USirvRosBinder::BindActions()
{

  namespace sr = sirv::rpc;
  //namespace cg = sirv::geom;
  //UE_LOG(LogSirv, Log, TEXT("  BindActions THIS OBJECT POINTER %p "), this);
  //UE_LOG(LogSirv, Log, TEXT("  BindActions WEAK EPISODE POINTER %p "), static_Episode_);
  // UE_LOG(LogSirv, Log, TEXT("Binding Subscriptions..."));
  
  _Ros->GetBlueprints("/get_blueprints", [] (int *size) -> R<std::vector<sirv::rpc::ActorDefinition>>
  {
        REQUIRE_SIRV_EPISODE();
        //UE_LOG(LogSirv, Log, TEXT("GetBlueprints UE5 INICIAL..."));
        

        auto definitions = MakeVectorFromTArray<sirv::rpc::ActorDefinition>( static_Episode_->GetActorDefinitions());
        //auto definitions = std::vector<sirv::rpc::ActorDefinition>(5);
        
        //UE_LOG(LogSirv, Log, TEXT("get_blueprints UE5 definitions size[%d]..."),definitions.size());

         return definitions;

  });
  UE_LOG(LogSirv, Log, TEXT("get_blueprints ROS2 Service CREATED..."));




  _Ros->GetAllLevelBBs("/get_all_level_BBs", [] (int *QueriedTag) -> R<std::vector<sirv::geom::BoundingBox>>
  {
        REQUIRE_SIRV_EPISODE();
        //UE_LOG(LogSirv, Log, TEXT("GetBlueprints UE5 INICIAL..."));
        
    auto Actors = std::vector<sirv::geom::BoundingBox>();

    FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&Actors, QueriedTag]()
    {
      
      UE_LOG(LogSirv, Log, TEXT("GetAllLevelBBs... "));

      REQUIRE_SIRV_EPISODE();
      ASirvGameModeBase* GameMode = USirvStatics::GetGameMode(static_Episode_->GetWorld());
      
      if (!GameMode)
      {
        RESPOND_ERROR("unable to find SIRV game mode");
      }
        
      Actors = MakeVectorFromTArray<sirv::geom::BoundingBox>( GameMode->GetAllBBsOfLevel(*QueriedTag));
        //auto definitions = std::vector<sirv::rpc::ActorDefinition>(5);
        //UE_LOG(LogSirv, Log, TEXT("get_blueprints UE5 definitions size[%d]..."),definitions.size());

        // return actors;
     
    
    }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);


    return Actors;


  });
  UE_LOG(LogSirv, Log, TEXT("get_all_level_BBs ROS2 Service CREATED..."));
  

  _Ros->spawnActor("/spawnActor",[](
      sirv::rpc::ActorDescription Description,
      const sirv::rpc::Transform &Transform) -> R<sirv::rpc::Actor>
  {

    UE_LOG(LogSirv, Log, TEXT(" DENTRO do spawnActor..."));
    

    sirv::rpc::Actor Actor;

    FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&Actor, &Description, &Transform]()
    {
        UE_LOG(LogSirv, Log, TEXT("TRANFORM ANTES : x[%2f], y[%2f], z[%2f] R[%2f] P[%2f] Y[%2f]"),Transform.location.x,Transform.location.y,Transform.location.z,
                                                                                                  Transform.rotation.roll,Transform.rotation.pitch,Transform.rotation.yaw);

      REQUIRE_SIRV_EPISODE();
        
      auto Result = static_Episode_->SpawnActorWithInfo(std::move(Transform), std::move(Description));
      if (Result.Key != EActorSpawnResultStatus::Success)
      {
        RESPOND_ERROR_FSTRING(FActorSpawnResult::StatusToString(Result.Key));
      }
      if (!Result.Value.IsValid())
      {
        RESPOND_ERROR("internal error: actor could not be spawned");
      }

      Actor = static_Episode_->SerializeActor(Result.Value);
    
    }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);

    
    return Actor;
    
});
UE_LOG(LogSirv, Log, TEXT("spawnActor ROS2 Service CREATED..."));

_Ros->spawnActorWithParent("/spawnActorWithParent",[](
      sirv::rpc::ActorDescription Description,
      const sirv::rpc::Transform &Transform,
      sirv::rpc::ActorId ParentId,
      sirv::rpc::AttachmentType InAttachmentType) -> R<sirv::rpc::Actor>
  {

    UE_LOG(LogSirv, Log, TEXT(" DENTRO do spawnActorWithParent... ParentID: [%d] Attachment [%d]"),ParentId,InAttachmentType);
    
    //for(int i=0; i< Description.attributes.size(); i++)
    //{
    //    UE_LOG(LogSirv, Log, TEXT(" ID: [%s] type: [%d] value: [%s] "),*FString(Description.attributes[i].id.c_str()),Description.attributes[i].type,*FString(Description.attributes[i].value.c_str()));
    //}


    sirv::rpc::Actor Actor;

    FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&Actor, &Description, &Transform, &ParentId, &InAttachmentType]()
    {
        UE_LOG(LogSirv, Log, TEXT("TRANFORM ANTES : x[%2f], y[%2f], z[%2f] R[%2f] P[%2f] Y[%2f]"),Transform.location.x,Transform.location.y,Transform.location.z,
                                                                                                  Transform.rotation.roll,Transform.rotation.pitch,Transform.rotation.yaw);

      REQUIRE_SIRV_EPISODE();
        
      auto Result = static_Episode_->SpawnActorWithInfo(std::move(Transform), std::move(Description));
      if (Result.Key != EActorSpawnResultStatus::Success)
      {
        RESPOND_ERROR_FSTRING(FActorSpawnResult::StatusToString(Result.Key));
      }
      if (!Result.Value.IsValid())
      {
        RESPOND_ERROR("internal error: actor could not be spawned");
      }

      auto ParentActorView = static_Episode_->FindActor(ParentId);
      if (!ParentActorView.IsValid())
      {
        RESPOND_ERROR("unable to attach actor: parent actor not found");
      }
    
      static_Episode_->AttachActors(
        Result.Value.GetActor(),
        ParentActorView.GetActor(),
        static_cast<EAttachmentType>(InAttachmentType));
    

      Actor = static_Episode_->SerializeActor(Result.Value);
    
    }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);
    
    return Actor;
    
});
UE_LOG(LogSirv, Log, TEXT("spawnActorWithParent ROS2 Service CREATED..."));


  _Ros->destroyObject("/destroyObject",[](
       sirv::rpc::ActorId ActorId) -> R<void>
  {
    UE_LOG(LogSirv, Log, TEXT(" DENTRO do DestroyActor..."));
    
    FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&ActorId]()
    {
      REQUIRE_SIRV_EPISODE();
        
      auto ActorView = static_Episode_->FindActor(ActorId);
      if (!ActorView.IsValid())
      {
        RESPOND_ERROR("unable to destroy actor: not found");
      }
      if (!static_Episode_->DestroyActor(ActorView.GetActor()))
      {
        RESPOND_ERROR("internal error: unable to destroy actor");
      }

    }, TStatId(), nullptr, ENamedThreads::GameThread);
    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);

    return R<void>::Success();
    
  });
  UE_LOG(LogSirv, Log, TEXT("Destroy Object ROS2 Service CREATED..."));




  _Ros->apply_control_to_vehicle("/apply_control_to_vehicle",[](
      sirv::rpc::ActorId ActorId,
      sirv::rpc::VehicleControl Control) -> R<void>
  {
      REQUIRE_SIRV_EPISODE();
      auto ActorView = static_Episode_->FindActor(ActorId);
      if (!ActorView.IsValid())
      {
        RESPOND_ERROR("unable to apply control: actor not found");
      }
      auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
      if (Vehicle == nullptr)
      {
        RESPOND_ERROR("unable to apply control: actor is not a vehicle");
      }
      //Control.hand_brake = false;
      //UE_LOG(LogSirv, Log, TEXT("1 APPLY CONTROL: steer[%2f], throttle[%2f], brake[%2f] hoist[%d]"),Control.steer,Control.throttle,Control.brake, Control.hoist);
      Vehicle->ApplyVehicleControl(Control, EVehicleInputPriority::Client);
      
      // This function is needed to enable the debug into unreal....
      //Vehicle->FlushVehicleControl();
      
      return R<void>::Success();
  });
  UE_LOG(LogSirv, Log, TEXT("Apply_control_to_vehicle ROS2 subscriber CREATED..."));


  _Ros->set_view_target_to_vehicle("/set_view_target_to_vehicle",[](
      sirv::rpc::ActorId ActorId) -> R<void>
  {
      
      //UE_LOG(LogSirv, Log, TEXT(" DENTRO UE5 VIEW TARGET ..."));
      //UE_LOG(LogSirv, Log, TEXT(" VIEW TARGET EPISODE POINTER %p "), Episode);

      FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&ActorId]()
    {
        REQUIRE_SIRV_EPISODE();

        UE_LOG(LogSirv, Log, TEXT(" DENTRO UE5 VIEW TARGET  FGraphEventRef..."));
        //UE_LOG(LogSirv, Log, TEXT(" DENTRO VIEW TARGET EPISODE POINTER %p "), Episode);

        auto ActorView = static_Episode_->FindActor(ActorId);
        if (!ActorView.IsValid())
        {
          RESPOND_ERROR("unable to apply view target to Vehicle: actor not found");
        }
        auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
        if (Vehicle == nullptr)
        {
          RESPOND_ERROR("unable to apply view target to Vehicle: actor is not a vehicle");
        }

        auto World = static_Episode_->GetWorld();
        check(World != nullptr);
        auto PlayerController = UGameplayStatics::GetPlayerController(World, 0);
        if (PlayerController == nullptr)
        {
          UE_LOG(LogSirv, Error, TEXT("unable to apply view target to Vehicle: Can't find player controller!"));
          //return R<void>::GetError();
        }
        //Spectator = PlayerController->GetPawn();
        //auto cameras = PlayerController->GetComponentsByClass(UCameraComponent::StaticClass());
        //TArray<UCameraComponent*> CameraComps;
        //PlayerController->GetComponents<UCameraComponent>(CameraComps);
        
        //UE_LOG(LogSirv, Log, TEXT(" APPLY SET VIEW TARGET: CAMERA NUMBERS [%d]"),CameraComps.Num());

        PlayerController->SetViewTargetWithBlend(Vehicle,true);

        UE_LOG(LogSirv, Log, TEXT(" APPLY SET VIEW TARGET: Actor ID[%d] UE5"),ActorId);


    }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);
    
    return R<void>::Success();
  
  });
  UE_LOG(LogSirv, Log, TEXT("set_view_target_to_vehicle ROS2 subscriber CREATED..."));

  
  _Ros->set_pause_resume_simulation("/set_pause_resume_simulation",[](
      sirv::rpc::ActorId ActorId) -> R<void>
  {
      
      //UE_LOG(LogSirv, Log, TEXT(" DENTRO UE5 VIEW TARGET ..."));
      //UE_LOG(LogSirv, Log, TEXT(" VIEW TARGET EPISODE POINTER %p "), Episode);

      FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&ActorId]()
    {
        REQUIRE_SIRV_EPISODE();

        auto World = static_Episode_->GetWorld();
        check(World != nullptr);

        bool flag = (ActorId == 1) ? true : false;
        bool resp = UGameplayStatics::SetGamePaused(World, flag); //true = paused, false = resume

        UE_LOG(LogSirv, Log, TEXT(" APPLY SET PAUSE OR RESUME SIMULATION: Status[%d] Resp[%d]"),flag,resp);


    }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);
    
    return R<void>::Success();
  
  });
  UE_LOG(LogSirv, Log, TEXT("set_pause_resume_simulation ROS2 subscriber CREATED..."));

  _Ros->set_vehicle_light_state("/set_vehicle_light_state",[](
      sirv::rpc::ActorId ActorId,
      sirv::rpc::VehicleLightState LightState) -> R<void>
  {

      FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&ActorId, &LightState]()
      {
          REQUIRE_SIRV_EPISODE();
          auto ActorView = static_Episode_->FindActor(ActorId);
          if (!ActorView.IsValid())
          {
            RESPOND_ERROR("unable to apply actor light state: actor not found");
          }
          auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
          if (Vehicle == nullptr)
          {
            RESPOND_ERROR("unable to apply actor light state: actor is not a vehicle");
          }

          Vehicle->SetVehicleLightState(FVehicleLightState(LightState));

      }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);

    return R<void>::Success();
  });
  UE_LOG(LogSirv, Log, TEXT("set_vehicle_light_state ROS2 subscriber CREATED..."));

  _Ros->set_vehicle_sound_state("/set_vehicle_sound_state",[](
      sirv::rpc::ActorId ActorId,
      sirv::rpc::VehicleSoundState SoundState) -> R<void>
  {

      FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&ActorId, &SoundState]()
      {
          REQUIRE_SIRV_EPISODE();
          auto ActorView = static_Episode_->FindActor(ActorId);
          if (!ActorView.IsValid())
          {
            RESPOND_ERROR("unable to apply actor sound state: actor not found");
          }
          auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
          if (Vehicle == nullptr)
          {
            RESPOND_ERROR("unable to apply actor sound state: actor is not a vehicle");
          }

          Vehicle->SetVehicleSoundState(FVehicleSoundState(SoundState));

      }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);

    return R<void>::Success();
  });
  UE_LOG(LogSirv, Log, TEXT("set_vehicle_sound_state ROS2 subscriber CREATED..."));

  _Ros->set_vehicle_effects_state("/set_vehicle_effects_state",[](
      sirv::rpc::ActorId ActorId,
      sirv::rpc::VehicleEffectsState EffectsState) -> R<void>
  {

      FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&ActorId, &EffectsState]()
      {
          REQUIRE_SIRV_EPISODE();
          auto ActorView = static_Episode_->FindActor(ActorId);
          if (!ActorView.IsValid())
          {
            RESPOND_ERROR("unable to apply actor effects state: actor not found");
          }
          auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
          if (Vehicle == nullptr)
          {
            RESPOND_ERROR("unable to apply actor effects state: actor is not a vehicle");
          }

          Vehicle->SetVehicleEffectsState(FVehicleEffectsState(EffectsState));

      }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);

    return R<void>::Success();
  });
  UE_LOG(LogSirv, Log, TEXT("set_vehicle_effects_state ROS2 subscriber CREATED..."));
  
  _Ros->get_vehicle_telemetry("/get_vehicle_telemetry", [] (int actor) -> R<sirv::rpc::VehicleTelemetry>
     {
        REQUIRE_SIRV_EPISODE();
        //UE_LOG(LogSirv, Log, TEXT("GetVehicle Telemetry UE5 INICIAL..."));
        
        auto ActorView = static_Episode_->FindActor(actor);
          if (!ActorView.IsValid())
          {
            RESPOND_ERROR("unable to get actor telemetry state: actor not found");
          }
          auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
          if (Vehicle == nullptr)
          {
            RESPOND_ERROR("unable to get actor telemetry state: actor is not a vehicle");
          }

          auto telemetry = Vehicle->GetVehicleTelemetry();

        sirv::rpc::VehicleTelemetry msg(telemetry);
        return msg;

    });
    UE_LOG(LogSirv, Log, TEXT("get_vehicle_telemetry ROS2 Service CREATED..."));



  _Ros->set_vehicle_physics_control("/apply_physics_control",[](
      sirv::rpc::ActorId ActorId,
      sirv::rpc::VehiclePhysicsControl PhysicsControl) -> R<void>
  {

      FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&ActorId, &PhysicsControl]()
      {
          REQUIRE_SIRV_EPISODE();
          auto ActorView = static_Episode_->FindActor(ActorId);
          if (!ActorView.IsValid())
          {
            RESPOND_ERROR("unable to apply actor physics control: actor not found");
          }
          auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
          if (Vehicle == nullptr)
          {
            RESPOND_ERROR("unable to apply actor physics control: actor is not a vehicle");
          }

          Vehicle->ApplyVehiclePhysicsControl(FVehiclePhysicsControl(PhysicsControl));

      }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);

    return R<void>::Success();
  });
  UE_LOG(LogSirv, Log, TEXT("apply_physics_control ROS2 subscriber CREATED..."));



/* 
  /// Looks for a Traffic Manager running on port
  BIND_SYNC(is_traffic_manager_running) << [this] (uint16_t port) ->R<bool>
  {
    return (TrafficManagerInfo.find(port) != TrafficManagerInfo.end());
  };
 */
/* 
  /// Gets a pair filled with the <IP, port> of the Trafic Manager running on port.
  /// If there is no Traffic Manager running the pair will be ("", 0)
  BIND_SYNC(get_traffic_manager_running) << [this] (uint16_t port) ->R<std::pair<std::string, uint16_t>>
  {
    auto it = TrafficManagerInfo.find(port);
    if(it != TrafficManagerInfo.end()) {
      return std::pair<std::string, uint16_t>(it->second, it->first);
    }
    return std::pair<std::string, uint16_t>("",0);
  };
 */
/* 
  /// Add a new Traffic Manager running on <IP, port>
  BIND_SYNC(add_traffic_manager_running) << [this] (std::pair<std::string, uint16_t> trafficManagerInfo) ->R<bool>
  {
    uint16_t port = trafficManagerInfo.second;
    auto it = TrafficManagerInfo.find(port);
    if(it == TrafficManagerInfo.end()) {
      TrafficManagerInfo.insert(
        std::pair<uint16_t, std::string>(port, trafficManagerInfo.first));
      return true;
    }
    return false;

  };
 */
/* 
  BIND_SYNC(destroy_traffic_manager) << [this] (uint16_t port) ->R<bool>
  {
    auto it = TrafficManagerInfo.find(port);
    if(it != TrafficManagerInfo.end()) {
      TrafficManagerInfo.erase(it);
      return true;
    }
    return false;
  };
 */
/* 
  BIND_ASYNC(version) << [] () -> R<std::string>
  {
    return sirv::version();
  };
 */
  // ~~ Tick ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* 
  BIND_SYNC(tick_cue) << [this]() -> R<uint64_t>
  {
    ++TickCuesReceived;
    return FSirvEngine::GetFrameCounter();
  };
 */
  // ~~ Load new episode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* 
  BIND_ASYNC(get_available_maps) << [this]() -> R<std::vector<std::string>>
  {
    const auto MapNames = USirvStatics::GetAllMapNames();
    std::vector<std::string> result;
    result.reserve(MapNames.Num());
    for (const auto &MapName : MapNames)
    {
      if (MapName.Contains("/Sublevels/"))
        continue;
      if (MapName.Contains("/BaseMap/"))
        continue;

      result.emplace_back(cr::FromFString(MapName));
    }
    return result;
  };
 */
/* 
  BIND_SYNC(load_new_episode) << [this](const std::string &map_name, const bool reset_settings, cr::MapLayer MapLayers) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();

    USirvGameInstance* GameInstance = USirvStatics::GetGameInstance(Episode->GetWorld());
    if (!GameInstance)
    {
      RESPOND_ERROR("unable to find SIRV game instance");
    }
    GameInstance->SetMapLayer(static_cast<int32>(MapLayers));

    if(!Episode->LoadNewEpisode(cr::ToFString(map_name), reset_settings))
    {
      RESPOND_ERROR("map not found");
    }

    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(load_map_layer) << [this](cr::MapLayer MapLayers) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();

    ASirvGameModeBase* GameMode = USirvStatics::GetGameMode(Episode->GetWorld());
    if (!GameMode)
    {
      RESPOND_ERROR("unable to find SIRV game mode");
    }
    GameMode->LoadMapLayer(static_cast<int32>(MapLayers));

    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(unload_map_layer) << [this](cr::MapLayer MapLayers) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();

    ASirvGameModeBase* GameMode = USirvStatics::GetGameMode(Episode->GetWorld());
    if (!GameMode)
    {
      RESPOND_ERROR("unable to find SIRV game mode");
    }
    GameMode->UnLoadMapLayer(static_cast<int32>(MapLayers));

    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(copy_opendrive_to_file) << [this](const std::string &opendrive, cr::OpendriveGenerationParameters Params) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    if (!Episode->LoadNewOpendriveEpisode(cr::ToLongFString(opendrive), Params))
    {
      RESPOND_ERROR("opendrive could not be correctly parsed");
    }
    return R<void>::Success();
  };
 */
  // ~~ Episode settings and info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
  BIND_SYNC(get_episode_info) << [this]() -> R<cr::EpisodeInfo>
  {
    REQUIRE_SIRV_EPISODE();
    return cr::EpisodeInfo{Episode->GetId(), BroadcastStream.token()};
  };
*/
/* 
  BIND_SYNC(get_map_info) << [this]() -> R<cr::MapInfo>
  {
    REQUIRE_SIRV_EPISODE();
    auto FileContents = UOpenDrive::LoadXODR(Episode->GetMapName());
    const auto &SpawnPoints = Episode->GetRecommendedSpawnPoints();
    return cr::MapInfo{
      cr::FromFString(Episode->GetMapName()),
      cr::FromLongFString(FileContents),
      MakeVectorFromTArray<cg::Transform>(SpawnPoints)};
  };
 */
/* 
  BIND_SYNC(get_navigation_mesh) << [this]() -> R<std::vector<uint8_t>>
  {
    REQUIRE_SIRV_EPISODE();
    auto FileContents = FNavigationMesh::Load(Episode->GetMapName());
    // make a mem copy (from TArray to std::vector)
    std::vector<uint8_t> Result(FileContents.Num());
    memcpy(&Result[0], FileContents.GetData(), FileContents.Num());
    return Result;
  };
 */
/* 
  BIND_SYNC(get_episode_settings) << [this]() -> R<cr::EpisodeSettings>
  {
    REQUIRE_SIRV_EPISODE();
    return cr::EpisodeSettings{Episode->GetSettings()};
  };
   */
/*
  BIND_SYNC(set_episode_settings) << [this](
      const cr::EpisodeSettings &settings) -> R<uint64_t>
  {
    REQUIRE_SIRV_EPISODE();
    Episode->ApplySettings(settings);
    StreamingServer.SetSynchronousMode(settings.synchronous_mode);
    return FSirvEngine::GetFrameCounter();
  };
*/
/* 
  BIND_SYNC(get_actor_definitions) << [this]() -> R<std::vector<cr::ActorDefinition>>
  {
    REQUIRE_SIRV_EPISODE();
    return MakeVectorFromTArray<cr::ActorDefinition>(Episode->GetActorDefinitions());
  };
 */
/* 
  BIND_SYNC(get_spectator) << [this]() -> R<cr::Actor>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(Episode->GetSpectatorPawn());
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("internal error: unable to find spectator");
    }
    return Episode->SerializeActor(ActorView);
  };
 */

/* 
  BIND_SYNC(get_environment_objects) << [this](uint8 QueriedTag) -> R<std::vector<cr::EnvironmentObject>>
  {
    REQUIRE_SIRV_EPISODE();
    ASirvGameModeBase* GameMode = USirvStatics::GetGameMode(Episode->GetWorld());
    if (!GameMode)
    {
      RESPOND_ERROR("unable to find SIRV game mode");
    }
    TArray<FEnvironmentObject> Result = GameMode->GetEnvironmentObjects(QueriedTag);
    return MakeVectorFromTArray<cr::EnvironmentObject>(Result);
  };
 */
/* 
  BIND_SYNC(enable_environment_objects) << [this](std::vector<uint64_t> EnvObjectIds, bool Enable) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    ASirvGameModeBase* GameMode = USirvStatics::GetGameMode(Episode->GetWorld());
    if (!GameMode)
    {
      RESPOND_ERROR("unable to find SIRV game mode");
    }

    TSet<uint64> EnvObjectIdsSet;
    for(uint64 Id : EnvObjectIds)
    {
      EnvObjectIdsSet.Emplace(Id);
    }

    GameMode->EnableEnvironmentObjects(EnvObjectIdsSet, Enable);
    return R<void>::Success();
  };
 */
  // ~~ Weather ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

_Ros->get_weather_parameters("/get_weather_parameters", [] () -> R<sirv::rpc::WeatherParameters>
     {
        REQUIRE_SIRV_EPISODE();
        //UE_LOG(LogSirv, Log, TEXT("GetWeather Parameters UE5 INICIAL..."));
        
        auto *Weather = static_Episode_->GetWeather();
        if (Weather == nullptr)
        {
          RESPOND_ERROR("internal error: unable to find weather");
        }
        
        sirv::rpc::WeatherParameters msg(Weather->GetCurrentWeather());
        return msg;

    });
    UE_LOG(LogSirv, Log, TEXT("get_weather_parameters ROS2 Service CREATED..."));

  _Ros->set_weather_parameters("/set_weather_parameters", [] (sirv::rpc::WeatherParameters &weather) -> R<void>
     {

      
      FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&weather]()
      {
          REQUIRE_SIRV_EPISODE();
          UE_LOG(LogSirv, Log, TEXT("SetWeather Parameters UE5 INICIAL..."));
        
          auto *Weather = static_Episode_->GetWeather();
          if (Weather == nullptr)
          {
            RESPOND_ERROR("internal error: unable to find weather");
          }
          
          Weather->ApplyWeather(weather);        

      }, TStatId(), nullptr, ENamedThreads::GameThread);

    FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);

    return R<void>::Success();    

    });
    UE_LOG(LogSirv, Log, TEXT("set_weather_parameters ROS2 Service CREATED..."));



  // ~~ Actor operations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* 
  BIND_SYNC(get_actors_by_id) << [this](
      const std::vector<FActorView::IdType> &ids) -> R<std::vector<cr::Actor>>
  {
    REQUIRE_SIRV_EPISODE();
    std::vector<cr::Actor> Result;
    Result.reserve(ids.size());
    for (auto &&Id : ids)
    {
      auto View = Episode->FindActor(Id);
      if (View.IsValid())
      {
        Result.emplace_back(Episode->SerializeActor(View));
      }
    }
    return Result;
  };
 */

/* 
  BIND_SYNC(spawn_actor_with_parent) << [this](
      cr::ActorDescription Description,
      const cr::Transform &Transform,
      cr::ActorId ParentId,
      cr::AttachmentType InAttachmentType) -> R<cr::Actor>
  {
    REQUIRE_SIRV_EPISODE();
    auto Result = Episode->SpawnActorWithInfo(Transform, std::move(Description));
    if (Result.Key != EActorSpawnResultStatus::Success)
    {
      RESPOND_ERROR_FSTRING(FActorSpawnResult::StatusToString(Result.Key));
    }
    if (!Result.Value.IsValid())
    {
      RESPOND_ERROR("internal error: actor could not be spawned");
    }
    auto ParentActorView = Episode->FindActor(ParentId);
    if (!ParentActorView.IsValid())
    {
      RESPOND_ERROR("unable to attach actor: parent actor not found");
    }
    Episode->AttachActors(
        Result.Value.GetActor(),
        ParentActorView.GetActor(),
        static_cast<EAttachmentType>(InAttachmentType));
    return Episode->SerializeActor(Result.Value);
  };
 */
/* 
  BIND_SYNC(destroy_actor) << [this](cr::ActorId ActorId) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to destroy actor: not found");
    }
    if (!Episode->DestroyActor(ActorView.GetActor()))
    {
      RESPOND_ERROR("internal error: unable to destroy actor");
    }
    return R<void>::Success();
  };
 */
  // ~~ Actor physics ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* 
  BIND_SYNC(set_actor_location) << [this](
      cr::ActorId ActorId,
      cr::Location Location) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set actor location: actor not found");
    }
    ActorView.GetActor()->SetActorRelativeLocation(
        Location,
        false,
        nullptr,
        ETeleportType::TeleportPhysics);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_actor_transform) << [this](
      cr::ActorId ActorId,
      cr::Transform Transform) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set actor transform: actor not found");
    }
    ActorView.GetActor()->SetActorRelativeTransform(
        Transform,
        false,
        nullptr,
        ETeleportType::TeleportPhysics);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_walker_state) << [this] (
      cr::ActorId ActorId,
      cr::Transform Transform,
      float Speed) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set walker state: actor not found");
    }

    auto * Walker = Cast<AWalkerBase>(ActorView.GetActor());
    if (Walker && !Walker->bAlive)
    {
      RESPOND_ERROR("unable to set actor state: walker is dead");
    }

    // apply walker transform
    FTransform NewTransform = Transform;
    FVector NewLocation = NewTransform.GetLocation();

    FTransform CurrentTransform = ActorView.GetActor()->GetTransform();
    FVector CurrentLocation = CurrentTransform.GetLocation();
    NewLocation.Z += 90.0f; // move point up because in Unreal walker is centered in the middle height

    // if difference between Z position is small, then we keep current, otherwise we set the new one
    // (to avoid Z fighting position and falling pedestrians)
    if (NewLocation.Z - CurrentLocation.Z < 100.0f)
      NewLocation.Z = CurrentLocation.Z;

    NewTransform.SetLocation(NewLocation);

    ActorView.GetActor()->SetActorRelativeTransform(
    NewTransform,
    false,
    nullptr,
    ETeleportType::TeleportPhysics);

    // apply walker speed
    auto Pawn = Cast<APawn>(ActorView.GetActor());
    if (Pawn == nullptr)
    {
      RESPOND_ERROR("unable to set walker state: actor is not a walker");
    }
    auto Controller = Cast<AWalkerController>(Pawn->GetController());
    if (Controller == nullptr)
    {
      RESPOND_ERROR("unable to set walker state: walker has an incompatible controller");
    }
    cr::WalkerControl Control(Transform.GetForwardVector(), Speed, false);
    Controller->ApplyWalkerControl(Control);

    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_actor_target_velocity) << [this](
      cr::ActorId ActorId,
      cr::Vector3D vector) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set actor target velocity: actor not found");
    }
    auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
    if (RootComponent == nullptr)
    {
      RESPOND_ERROR("unable to set actor target velocity: not supported by actor");
    }
    RootComponent->SetPhysicsLinearVelocity(
        vector.ToCentimeters().ToFVector(),
        false,
        "None");
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_actor_target_angular_velocity) << [this](
      cr::ActorId ActorId,
      cr::Vector3D vector) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set actor target angular velocity: actor not found");
    }
    auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
    if (RootComponent == nullptr)
    {
      RESPOND_ERROR("unable to set actor target angular velocity: not supported by actor");
    }
    RootComponent->SetPhysicsAngularVelocityInDegrees(
        vector.ToFVector(),
        false,
        "None");
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(enable_actor_constant_velocity) << [this](
      cr::ActorId ActorId,
      cr::Vector3D vector) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set actor velocity: actor not found");
    }
    auto SirvVehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
    if (SirvVehicle == nullptr)
    {
      RESPOND_ERROR("unable to set actor velocity: not supported by actor");
    }

    SirvVehicle->ActivateVelocityControl(vector.ToCentimeters().ToFVector());

    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(disable_actor_constant_velocity) << [this](
      cr::ActorId ActorId) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set actor velocity: actor not found");
    }
    auto SirvVehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
    if (SirvVehicle == nullptr)
    {
      RESPOND_ERROR("unable to set actor velocity: not supported by actor");
    }

    SirvVehicle->DeactivateVelocityControl();

    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(add_actor_impulse) << [this](
      cr::ActorId ActorId,
      cr::Vector3D vector) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to add actor impulse: actor not found");
    }
    auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
    if (RootComponent == nullptr)
    {
      RESPOND_ERROR("unable to add actor impulse: not supported by actor");
    }
    RootComponent->AddImpulse(
        vector.ToCentimeters().ToFVector(),
        "None",
        false);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(add_actor_impulse_at_location) << [this](
      cr::ActorId ActorId,
      cr::Vector3D impulse,
      cr::Vector3D location) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to add actor impulse: actor not found");
    }
    auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
    if (RootComponent == nullptr)
    {
      RESPOND_ERROR("unable to add actor impulse: not supported by actor");
    }

    UE_LOG(LogSirv, Warning, TEXT("AddImpulseAtLocation: Experimental feature, use carefully."));

    RootComponent->AddImpulseAtLocation(
        impulse.ToCentimeters().ToFVector(),
        location.ToCentimeters().ToFVector(),
        "None");
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(add_actor_force) << [this](
      cr::ActorId ActorId,
      cr::Vector3D vector) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to add actor impulse: actor not found");
    }
    auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
    if (RootComponent == nullptr)
    {
      RESPOND_ERROR("unable to add actor impulse: not supported by actor");
    }
    RootComponent->AddForce(
        vector.ToCentimeters().ToFVector(),
        "None",
        false);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(add_actor_force_at_location) << [this](
      cr::ActorId ActorId,
      cr::Vector3D force,
      cr::Vector3D location) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to add actor impulse: actor not found");
    }
    auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
    if (RootComponent == nullptr)
    {
      RESPOND_ERROR("unable to add actor impulse: not supported by actor");
    }

    UE_LOG(LogSirv, Warning, TEXT("AddImpulseAtLocation: Experimental feature, use carefully."));

    RootComponent->AddForceAtLocation(
        force.ToCentimeters().ToFVector(),
        location.ToCentimeters().ToFVector(),
        "None");
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(add_actor_angular_impulse) << [this](
      cr::ActorId ActorId,
      cr::Vector3D vector) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to add actor angular impulse: actor not found");
    }
    auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
    if (RootComponent == nullptr)
    {
      RESPOND_ERROR("unable to add actor angular impulse: not supported by actor");
    }
    RootComponent->AddAngularImpulseInDegrees(
        vector.ToFVector(),
        "None",
        false);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(add_actor_torque) << [this](
      cr::ActorId ActorId,
      cr::Vector3D vector) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to add actor torque: actor not found");
    }
    auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
    if (RootComponent == nullptr)
    {
      RESPOND_ERROR("unable to add actor torque: not supported by actor");
    }
    RootComponent->AddTorqueInDegrees(
        vector.ToFVector(),
        "None",
        false);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(get_physics_control) << [this](
      cr::ActorId ActorId) -> R<cr::VehiclePhysicsControl>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to get actor physics control: actor not found");
    }
    auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
    if (Vehicle == nullptr)
    {
      RESPOND_ERROR("unable to get actor physics control: actor is not a vehicle");
    }

    return cr::VehiclePhysicsControl(Vehicle->GetVehiclePhysicsControl());
  };
 */
/* 
  BIND_SYNC(get_vehicle_light_state) << [this](
      cr::ActorId ActorId) -> R<cr::VehicleLightState>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to get actor physics control: actor not found");
    }
    auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
    if (Vehicle == nullptr)
    {
      RESPOND_ERROR("unable to get actor physics control: actor is not a vehicle");
    }

    return cr::VehicleLightState(Vehicle->GetVehicleLightState());
  };
 */
/* 
  BIND_SYNC(apply_physics_control) << [this](
      cr::ActorId ActorId,
      cr::VehiclePhysicsControl PhysicsControl) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to apply actor physics control: actor not found");
    }
    auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
    if (Vehicle == nullptr)
    {
      RESPOND_ERROR("unable to apply actor physics control: actor is not a vehicle");
    }

    Vehicle->ApplyVehiclePhysicsControl(FVehiclePhysicsControl(PhysicsControl));

    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_vehicle_light_state) << [this](
      cr::ActorId ActorId,
      cr::VehicleLightState LightState) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to apply actor light state: actor not found");
    }
    auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
    if (Vehicle == nullptr)
    {
      RESPOND_ERROR("unable to apply actor light state: actor is not a vehicle");
    }

    Vehicle->SetVehicleLightState(FVehicleLightState(LightState));

    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_actor_simulate_physics) << [this](
      cr::ActorId ActorId,
      bool bEnabled) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set actor simulate physics: actor not found");
    }

    auto Character = Cast<ACharacter>(ActorView.GetActor());
    // The physics in the walkers works in a different way so to disable them,
    // we need to do it in the UCharacterMovementComponent.
    if (Character != nullptr)
    {
      auto CharacterMovement = Cast<UCharacterMovementComponent>(Character->GetCharacterMovement());

      if(bEnabled) {
        CharacterMovement->SetDefaultMovementMode();
      }
      else {
        CharacterMovement->DisableMovement();
      }
    }
    // In the rest of actors, the physics is controlled with the UPrimitiveComponent, so we use
    // that for disable it.
    else
    {
      auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
      if (RootComponent == nullptr)
      {
        RESPOND_ERROR("unable to set actor simulate physics: not supported by actor");
      }
      auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
      if(Vehicle)
      {
        Vehicle->SetActorEnableCollision(true);
        #ifdef WITH_CARSIM
        if(!Vehicle->IsCarSimEnabled())
        #endif
        {
          RootComponent->SetSimulatePhysics(bEnabled);
          RootComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
      }
      else
      {
        RootComponent->SetSimulatePhysics(bEnabled);
        RootComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
      }
    }

    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_actor_enable_gravity) << [this](
      cr::ActorId ActorId,
      bool bEnabled) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set actor enable gravity: actor not found");
    }

    auto Character = Cast<ACharacter>(ActorView.GetActor());
    // The physics in the walkers works in a different way so to disable them,
    // we need to do it in the UCharacterMovementComponent.
    if (Character != nullptr)
    {
      auto CharacterMovement = Cast<UCharacterMovementComponent>(Character->GetCharacterMovement());

      if(bEnabled) {
        CharacterMovement->SetDefaultMovementMode();
      }
      else {
        if (CharacterMovement->IsFlying() || CharacterMovement->IsFalling())
          CharacterMovement->DisableMovement();
      }
    }
    // In the rest of actors, the physics is controlled with the UPrimitiveComponent, so we use
    // that for disable it.
    else
    {
      auto RootComponent = Cast<UPrimitiveComponent>(ActorView.GetActor()->GetRootComponent());
      if (RootComponent == nullptr)
      {
        RESPOND_ERROR("unable to set actor enable gravity: not supported by actor");
      }
      RootComponent->SetEnableGravity(bEnabled);
    }

    return R<void>::Success();
  };
 */
  // ~~ Apply control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* 
  BIND_SYNC(apply_control_to_vehicle) << [this](
      cr::ActorId ActorId,
      cr::VehicleControl Control) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to apply control: actor not found");
    }
    auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
    if (Vehicle == nullptr)
    {
      RESPOND_ERROR("unable to apply control: actor is not a vehicle");
    }
    Vehicle->ApplyVehicleControl(Control, EVehicleInputPriority::Client);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(apply_control_to_walker) << [this](
      cr::ActorId ActorId,
      cr::WalkerControl Control) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to apply control: actor not found");
    }
    auto Pawn = Cast<APawn>(ActorView.GetActor());
    if (Pawn == nullptr)
    {
      RESPOND_ERROR("unable to apply control: actor is not a walker");
    }
    auto Controller = Cast<AWalkerController>(Pawn->GetController());
    if (Controller == nullptr)
    {
      RESPOND_ERROR("unable to apply control: walker has an incompatible controller");
    }
    Controller->ApplyWalkerControl(Control);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(apply_bone_control_to_walker) << [this](
      cr::ActorId ActorId,
      cr::WalkerBoneControl Control) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to apply control: actor not found");
    }
    auto Pawn = Cast<APawn>(ActorView.GetActor());
    if (Pawn == nullptr)
    {
      RESPOND_ERROR("unable to apply control: actor is not a walker");
    }
    auto Controller = Cast<AWalkerController>(Pawn->GetController());
    if (Controller == nullptr)
    {
      RESPOND_ERROR("unable to apply control: walker has an incompatible controller");
    }
    Controller->ApplyWalkerControl(Control);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_actor_autopilot) << [this](
      cr::ActorId ActorId,
      bool bEnabled) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->FindActor(ActorId);
    if (!ActorView.IsValid())
    {
      RESPOND_ERROR("unable to set autopilot: actor not found");
    }
    auto Vehicle = Cast<ASirvWheeledVehicle>(ActorView.GetActor());
    if (Vehicle == nullptr)
    {
      RESPOND_ERROR("unable to set autopilot: actor does not support autopilot");
    }
    auto Controller = Cast<AWheeledVehicleAIController>(Vehicle->GetController());
    if (Controller == nullptr)
    {
      RESPOND_ERROR("unable to set autopilot: vehicle controller does not support autopilot");
    }
    Controller->SetAutopilot(bEnabled);
    return R<void>::Success();
  };
 */

// ~~ Traffic lights ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* 
  BIND_SYNC(set_traffic_light_state) << [this](
      cr::ActorId ActorId,
      cr::TrafficLightState trafficLightState) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->GetActorRegistry().Find(ActorId);
    if (!ActorView.IsValid() || ActorView.GetActor()->IsPendingKill())
    {
      RESPOND_ERROR("unable to set state: actor not found");
    }
    auto TrafficLight = Cast<ATrafficLightBase>(ActorView.GetActor());
    if (TrafficLight == nullptr)
    {
      RESPOND_ERROR("unable to set state: actor is not a traffic light");
    }
    TrafficLight->SetTrafficLightState(static_cast<ETrafficLightState>(trafficLightState));
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_traffic_light_green_time) << [this](
      cr::ActorId ActorId,
      float GreenTime) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->GetActorRegistry().Find(ActorId);
    if (!ActorView.IsValid() || ActorView.GetActor()->IsPendingKill())
    {
      RESPOND_ERROR("unable to set green time: actor not found");
    }
    auto TrafficLight = Cast<ATrafficLightBase>(ActorView.GetActor());
    if (TrafficLight == nullptr)
    {
      RESPOND_ERROR("unable to set green time: actor is not a traffic light");
    }
    TrafficLight->SetGreenTime(GreenTime);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_traffic_light_yellow_time) << [this](
      cr::ActorId ActorId,
      float YellowTime) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->GetActorRegistry().Find(ActorId);
    if (!ActorView.IsValid() || ActorView.GetActor()->IsPendingKill())
    {
      RESPOND_ERROR("unable to set yellow time: actor not found");
    }
    auto TrafficLight = Cast<ATrafficLightBase>(ActorView.GetActor());
    if (TrafficLight == nullptr)
    {
      RESPOND_ERROR("unable to set yellow time: actor is not a traffic light");
    }
    TrafficLight->SetYellowTime(YellowTime);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_traffic_light_red_time) << [this](
      cr::ActorId ActorId,
      float RedTime) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->GetActorRegistry().Find(ActorId);
    if (!ActorView.IsValid() || ActorView.GetActor()->IsPendingKill())
    {
      RESPOND_ERROR("unable to set red time: actor not found");
    }
    auto TrafficLight = Cast<ATrafficLightBase>(ActorView.GetActor());
    if (TrafficLight == nullptr)
    {
      RESPOND_ERROR("unable to set red time: actor is not a traffic light");
    }
    TrafficLight->SetRedTime(RedTime);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(freeze_traffic_light) << [this](
      cr::ActorId ActorId,
      bool Freeze) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->GetActorRegistry().Find(ActorId);
    if (!ActorView.IsValid() || ActorView.GetActor()->IsPendingKill())
    {
      RESPOND_ERROR("unable to alter frozen state: actor not found");
    }
    auto TrafficLight = Cast<ATrafficLightBase>(ActorView.GetActor());
    if (TrafficLight == nullptr)
    {
      RESPOND_ERROR("unable to alter frozen state: actor is not a traffic light");
    }
    TrafficLight->SetTimeIsFrozen(Freeze);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(reset_traffic_light_group) << [this](
      cr::ActorId ActorId) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->GetActorRegistry().Find(ActorId);
    if (!ActorView.IsValid() || ActorView.GetActor()->IsPendingKill())
    {
      RESPOND_ERROR("unable to reset traffic lights: actors not found");
    }
    auto TrafficLight = Cast<ATrafficLightBase>(ActorView.GetActor());
    if (TrafficLight == nullptr)
    {
      RESPOND_ERROR("unable to reset traffic lights: actor is not a traffic light");
    }
    TrafficLight->GetTrafficLightComponent()->GetGroup()->ResetGroup();
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(reset_all_traffic_lights) << [this]() -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    for (TActorIterator<ATrafficLightGroup> It(Episode->GetWorld()); It; ++It)
    {
      It->ResetGroup();
    }
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(freeze_all_traffic_lights) << [this]
      (bool frozen) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto* GameMode = USirvStatics::GetGameMode(Episode->GetWorld());
    if (!GameMode)
    {
      RESPOND_ERROR("unable to find SIRV game mode");
    }
    auto* TraffiLightManager = GameMode->GetTrafficLightManager();
    TraffiLightManager->SetFrozen(frozen);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(get_vehicle_light_states) << [this]() -> R<cr::VehicleLightStateList>
  {
    REQUIRE_SIRV_EPISODE();
    cr::VehicleLightStateList List;

    auto It = Episode->GetActorRegistry().begin();
    for (; It != Episode->GetActorRegistry().end(); ++It)
    {
      auto Actor = It->GetActor();
      if (!Actor->IsPendingKill() && It->GetActorType() == FActorView::ActorType::Vehicle)
      {
        const ASirvWheeledVehicle *Vehicle = Cast<ASirvWheeledVehicle>(Actor);
        List.emplace_back(
            It->GetActorId(),
            cr::VehicleLightState(Vehicle->GetVehicleLightState()).GetLightStateAsValue());
      }
    }

    return List;
  };
 */
/* 
  BIND_SYNC(get_group_traffic_lights) << [this](
      const cr::ActorId ActorId) -> R<std::vector<cr::ActorId>>
  {
    REQUIRE_SIRV_EPISODE();
    auto ActorView = Episode->GetActorRegistry().Find(ActorId);
    if (!ActorView.IsValid() || ActorView.GetActor()->IsPendingKill())
    {
      RESPOND_ERROR("unable to get group traffic lights: actor not found");
    }
    auto TrafficLight = Cast<ATrafficLightBase>(ActorView.GetActor());
    if (TrafficLight == nullptr)
    {
      RESPOND_ERROR("unable to get group traffic lights: actor is not a traffic light");
    }
    std::vector<cr::ActorId> Result;
    for (auto TLight : TrafficLight->GetGroupTrafficLights())
    {
      auto View = Episode->FindActor(TLight);
      if (View.IsValid())
      {
        Result.push_back(View.GetActorId());
      }
    }
    return Result;
  };
 */
  // ~~ Logging and playback ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* 
  BIND_SYNC(start_recorder) << [this](std::string name, bool AdditionalData) -> R<std::string>
  {
    REQUIRE_SIRV_EPISODE();
    return R<std::string>(Episode->StartRecorder(name, AdditionalData));
  };
 */
/* 
  BIND_SYNC(stop_recorder) << [this]() -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    Episode->GetRecorder()->Stop();
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(show_recorder_file_info) << [this](
      std::string name,
      bool show_all) -> R<std::string>
  {
    REQUIRE_SIRV_EPISODE();
    return R<std::string>(Episode->GetRecorder()->ShowFileInfo(
        name,
        show_all));
  };
 */
/* 
  BIND_SYNC(show_recorder_collisions) << [this](
      std::string name,
      char type1,
      char type2) -> R<std::string>
  {
    REQUIRE_SIRV_EPISODE();
    return R<std::string>(Episode->GetRecorder()->ShowFileCollisions(
        name,
        type1,
        type2));
  };
 */
/* 
  BIND_SYNC(show_recorder_actors_blocked) << [this](
      std::string name,
      double min_time,
      double min_distance) -> R<std::string>
  {
    REQUIRE_SIRV_EPISODE();
    return R<std::string>(Episode->GetRecorder()->ShowFileActorsBlocked(
        name,
        min_time,
        min_distance));
  };
 */
/* 
  BIND_SYNC(replay_file) << [this](
      std::string name,
      double start,
      double duration,
      uint32_t follow_id) -> R<std::string>
  {
    REQUIRE_SIRV_EPISODE();
    return R<std::string>(Episode->GetRecorder()->ReplayFile(
        name,
        start,
        duration,
        follow_id));
  };
 */
/* 
  BIND_SYNC(set_replayer_time_factor) << [this](double time_factor) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    Episode->GetRecorder()->SetReplayerTimeFactor(time_factor);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(set_replayer_ignore_hero) << [this](bool ignore_hero) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    Episode->GetRecorder()->SetReplayerIgnoreHero(ignore_hero);
    return R<void>::Success();
  };
 */
/* 
  BIND_SYNC(stop_replayer) << [this](bool keep_actors) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    Episode->GetRecorder()->StopReplayer(keep_actors);
    return R<void>::Success();
  };
 */
  // ~~ Draw debug shapes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* 
  BIND_SYNC(draw_debug_shape) << [this](const cr::DebugShape &shape) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto *World = Episode->GetWorld();
    check(World != nullptr);
    FDebugShapeDrawer Drawer(*World);
    Drawer.Draw(shape);
    return R<void>::Success();
  };
 */
  // ~~ Apply commands in batch ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  //using C = cr::Command;
  //using CR = cr::CommandResponse;
  using ActorId = sirv::ActorId;
/* 
  auto parse_result = [](ActorId id, const auto &response) {
    return response.HasError() ? CR{response.GetError()} : CR{id};
  };

#define MAKE_RESULT(operation) return parse_result(c.actor, operation);

  auto command_visitor = sirv::Functional::MakeRecursiveOverload(
      [=](auto self, const C::SpawnActor &c) -> CR {
        auto result = c.parent.has_value() ?
        spawn_actor_with_parent(
            c.description,
            c.transform,
            *c.parent,
            cr::AttachmentType::Rigid) :
        spawn_actor(c.description, c.transform);
        if (!result.HasError())
        {
          ActorId id = result.Get().id;
          auto set_id = sirv::Functional::MakeOverload(
              [](C::SpawnActor &) {},
              [id](auto &s) { s.actor = id; });
          for (auto command : c.do_after)
          {
            boost::apply_visitor(set_id, command.command);
            boost::apply_visitor(self, command.command);
          }
          return id;
        }
        return result.GetError();
      },
      [=](auto, const C::DestroyActor &c) {         MAKE_RESULT(destroy_actor(c.actor)); },
      [=](auto, const C::ApplyVehicleControl &c) {  MAKE_RESULT(apply_control_to_vehicle(c.actor, c.control)); },
      [=](auto, const C::ApplyWalkerControl &c) {   MAKE_RESULT(apply_control_to_walker(c.actor, c.control)); },
      [=](auto, const C::ApplyTransform &c) {       MAKE_RESULT(set_actor_transform(c.actor, c.transform)); },
      [=](auto, const C::ApplyTargetVelocity &c) {  MAKE_RESULT(set_actor_target_velocity(c.actor, c.velocity)); },
      [=](auto, const C::ApplyTargetAngularVelocity &c) { MAKE_RESULT(set_actor_target_angular_velocity(c.actor, c.angular_velocity)); },
      [=](auto, const C::ApplyImpulse &c) {         MAKE_RESULT(add_actor_impulse(c.actor, c.impulse)); },
      [=](auto, const C::ApplyForce &c) {           MAKE_RESULT(add_actor_force(c.actor, c.force)); },
      [=](auto, const C::ApplyAngularImpulse &c) {  MAKE_RESULT(add_actor_angular_impulse(c.actor, c.impulse)); },
      [=](auto, const C::ApplyTorque &c) {          MAKE_RESULT(add_actor_torque(c.actor, c.torque)); },
      [=](auto, const C::SetSimulatePhysics &c) {   MAKE_RESULT(set_actor_simulate_physics(c.actor, c.enabled)); },
      [=](auto, const C::SetEnableGravity &c) {   MAKE_RESULT(set_actor_enable_gravity(c.actor, c.enabled)); },
      // TODO: SetAutopilot should be removed. This is the old way to control the vehicles
      [=](auto, const C::SetAutopilot &c) {         MAKE_RESULT(set_actor_autopilot(c.actor, c.enabled)); },
      [=](auto, const C::SetVehicleLightState &c) { MAKE_RESULT(set_vehicle_light_state(c.actor, c.light_state)); },
      [=](auto, const C::ApplyWalkerState &c) {     MAKE_RESULT(set_walker_state(c.actor, c.transform, c.speed)); });

#undef MAKE_RESULT
 */
/* 
  BIND_SYNC(apply_batch) << [=](
      const std::vector<cr::Command> &commands,
      bool do_tick_cue)
  {
    std::vector<CR> result;
    result.reserve(commands.size());
    for (const auto &command : commands)
    {
      result.emplace_back(boost::apply_visitor(command_visitor, command.command));
    }
    if (do_tick_cue)
    {
      tick_cue();
    }
    return result;
  };
 */
  // ~~ Light Subsystem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* 
  BIND_SYNC(query_lights_state) << [this](std::string client) -> R<std::vector<cr::LightState>>
  {
    REQUIRE_SIRV_EPISODE();
    std::vector<cr::LightState> result;
    auto *World = Episode->GetWorld();
    if(World) {
      USirvLightSubsystem* SirvLightSubsystem = World->GetSubsystem<USirvLightSubsystem>();
      result = SirvLightSubsystem->GetLights(FString(client.c_str()));
    }
    return result;
  };
 */
/* 
  BIND_SYNC(update_lights_state) << [this]
    (std::string client, const std::vector<cr::LightState>& lights, bool discard_client) -> R<void>
  {
    REQUIRE_SIRV_EPISODE();
    auto *World = Episode->GetWorld();
    if(World) {
      USirvLightSubsystem* SirvLightSubsystem = World->GetSubsystem<USirvLightSubsystem>();
      SirvLightSubsystem->SetLights(FString(client.c_str()), lights, discard_client);
    }
    return R<void>::Success();
  };

 */
  // ~~ Ray Casting ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* 
  BIND_SYNC(project_point) << [this]
      (cr::Location Location, cr::Vector3D Direction, float SearchDistance)
      -> R<std::pair<bool,cr::LabelledPoint>>
  {
    REQUIRE_SIRV_EPISODE();
    auto *World = Episode->GetWorld();
    constexpr float meter_to_centimeter = 100.0f;
    return URayTracer::ProjectPoint(Location, Direction.ToFVector(),
        meter_to_centimeter * SearchDistance, World);
  };
 */
/* 
  BIND_SYNC(cast_ray) << [this]
      (cr::Location StartLocation, cr::Location EndLocation)
      -> R<std::vector<cr::LabelledPoint>>
  {
    REQUIRE_SIRV_EPISODE();
    auto *World = Episode->GetWorld();
    return URayTracer::CastRay(StartLocation, EndLocation, World);
  };
 */
}

// =============================================================================
// -- Undef helper macros ------------------------------------------------------
// =============================================================================

#undef BIND_ASYNC
#undef BIND_SYNC
#undef REQUIRE_SIRV_EPISODE
#undef RESPOND_ERROR_FSTRING
#undef RESPOND_ERROR
#undef SIRV_ENSURE_GAME_THREAD

// =============================================================================
// -- USirvRosBinder -------------------------------------------------------
// =============================================================================

USirvRosBinder::USirvRosBinder(const FObjectInitializer &ObjectInitializer)
: Super(ObjectInitializer)
{

}

USirvRosBinder::~USirvRosBinder() {}


void USirvRosBinder::Start()
{
  //Pimpl = MakeUnique<FPimpl>();
  
  
  //Pimpl->Episode = nullptr;
  
  //BindActions();
  
  UE_LOG(LogSirvServer, Log, TEXT("::::::::: USirvRosBinder STARTED"));

  //Ros = MakeShared<sirv_ros::SirvRosHandler>();
  /* static_Ros_ = TWeakPtr<sirv_ros::SirvRosHandler> (Ros);

  if(static_Ros_.IsValid()){
    UE_LOG(LogSirvServer, Log, TEXT("::::::::: static_Ros1_ ISVALID"));
    }else{
    UE_LOG(LogSirvServer, Log, TEXT("::::::::: static_Ros1_ NOT ISVALID"));
    } */


  
}

void USirvRosBinder::NotifyBeginEpisode(USirvEpisode &_Episode)
{

  //check(Pimpl != nullptr);
  //check(&Episode != nullptr);

  UE_LOG(LogSirvServer, Log, TEXT("New episode '%s' started"), *_Episode.GetMapName());

  

  Episode = &_Episode;

  //teste = static_cast<void*>(&_Episode);
  //Episode1 = MakeShareable(&_Episode);
  //TSharedRef<USirvEpisode> Episode1 = MakeShareable(&_Episode);
  //TWeakPtr<USirvEpisode> static_();
  static_Episode_ = &_Episode;

  //Episode->AddToRoot();
  _Ros = MakeShareable(new sirv_ros::SirvRosHandler());


  //UE_LOG(LogSirv, Log, TEXT(" ROSBINDER _EPISODE POINTER %p "), &_Episode);
  //UE_LOG(LogSirv, Log, TEXT(" ROSBINDER  EPISODE POINTER %p "), Episode);
  UE_LOG(LogSirv, Log, TEXT("ROSBINDER :: NotifyBeginEpisode - THIS OBJECT POINTER %p "), this);
  //UE_LOG(LogSirv, Log, TEXT(" ROSBINDER  shared EPISODE POINTER %p "), Episode1.Get());

  //TSharedPtr<USirvEpisode> test = MakeShareable(&Episode);
  //Pimpl->MySharedPointer = MakeShareable(&Episode);
  //Pimpl->MySharedPointer = test;

  //UE_LOG(LogSirv, Log, TEXT(" GetBlueprints MySharedPointer POINTER %p "), Pimpl->MySharedPointer.Pin().Get());


  // static_Episode_ = &Episode;
  
  BindActions();


  UE_LOG(LogSirvServer, Log, TEXT("::::::::: NotifyBeginEpisode "));
}

void USirvRosBinder::NotifyEndEpisode()
{
  //check(Pimpl != nullptr);

  //Episode->RemoveFromRoot();

  //Episode = nullptr;
  _Ros = nullptr;

  static_Episode_ = nullptr;
  //Episode1.Reset();
  //Pimpl->MySharedPointer = nullptr;

  //Pimpl->MySharedPointer.Reset();

  //static_Ros_ = nullptr;
  //static_Episode_ = nullptr;

  UE_LOG(LogSirvServer, Log, TEXT("::::::::: NotifyEndEpisode "));

}

void USirvRosBinder::publish_timestamp(double time)
{
    check(_Ros != nullptr);
    _Ros->clock_publisher(time);
}

uint32 USirvRosBinder::advertiseSensorTopic(const FActorDescription &Description) const
{
    check(_Ros != nullptr);
    
    FString topicName = UActorBlueprintFunctionLibrary::ActorAttributeToString(Description.Variations["topic_publisher"],TEXT("/sensor/camera/image"));
    std::string topicNameChar = TCHAR_TO_UTF8(*topicName);

    FString frameid = UActorBlueprintFunctionLibrary::ActorAttributeToString(Description.Variations["role_name"],TEXT("DefaultCamera"));
    std::string frame = TCHAR_TO_UTF8(*frameid);

    
    // for(int i=0; i< DescriptionRos.attributes.size(); i++)
    // {
    //     UE_LOG(LogSirvServer, Log, TEXT(" ID: [%s] type: [%d] value: [%s] "),*FString(DescriptionRos.attributes[i].id.c_str()),DescriptionRos.attributes[i].type,*FString(DescriptionRos.attributes[i].value.c_str()));

    // }

    //Camera sensor
    if(Description.Class == ASceneCaptureCamera::StaticClass())
    {
      UE_LOG(LogSirvServer, Log, TEXT("::::::::: createCameraPublisher: TOPIC NAME: %s "),*topicName);

      int res_x = UActorBlueprintFunctionLibrary::ActorAttributeToInt(Description.Variations["image_size_x"],640);
      int res_y = UActorBlueprintFunctionLibrary::ActorAttributeToInt(Description.Variations["image_size_y"],480);

      return _Ros->createCameraPublisher(topicNameChar.c_str(), res_x, res_y, frame.c_str());
    }

    return 0;
}

void USirvRosBinder::CameraDataPublisher(int camera_id, sirv::Buffer &data) const
{
    _Ros->PublishCamera(camera_id, data);

}

// void USirvRosBinder::AsyncRun(uint32 NumberOfWorkerThreads)
// {
//   //check(Pimpl != nullptr);
//   /// @todo Define better the number of threads each server gets.
//   //auto RPCThreads = NumberOfWorkerThreads;
// bool USirvRosBinder::TickCueReceived()
// {
//   // if (Pimpl->TickCuesReceived > 0u)
//   // {
//   //   --(Pimpl->TickCuesReceived);
//   //   return true;
//   // }
//   return false;
// }

// void USirvRosBinder::Stop()
// {
//   check(Pimpl != nullptr);
//   Pimpl = nullptr;
//   //Pimpl->Server.Stop();

//   UE_LOG(LogSirvServer, Log, TEXT("::::::::: Pimpl STOPPED"));

// }

