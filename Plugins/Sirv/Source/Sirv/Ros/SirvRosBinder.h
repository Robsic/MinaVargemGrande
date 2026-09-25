// Copyright (c) 2021 Laboratory of Robotics, Intelligent and Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.
// Author: Giovani Bernardes Vitor


#pragma once

#include "Actor/ActorView.h"


#include "Sirv.h"

#include <compiler/disable-ue4-macros.h>
#include <sirv_ros/sirv_ros_handler.hpp>
#include <compiler/enable-ue4-macros.h>

#include "UObject/WeakObjectPtrTemplates.h"

#include "Async/TaskGraphInterfaces.h"


#include "CoreMinimal.h"

#include "SirvRosBinder.generated.h"

class USirvEpisode;

UCLASS()
class USirvRosBinder: public UObject
{

  GENERATED_BODY()

public:

  USirvRosBinder(const FObjectInitializer &ObjectInitializer);

  ~USirvRosBinder();

  //FDataMultiStream Start(uint16_t RPCPort, uint16_t StreamingPort);
  void Start();
  
  void NotifyBeginEpisode(USirvEpisode &Episode);

  void NotifyEndEpisode();

  //void AsyncRun(uint32 NumberOfWorkerThreads);

  //void RunSome(uint32 Milliseconds);

  //bool TickCueReceived();


  //void Stop();

  void publish_timestamp(double time);
  
  uint32 advertiseSensorTopic(const FActorDescription &Description) const;
  void   CameraDataPublisher(int camera_id, sirv::Buffer &data) const;

private:

  
  UPROPERTY()
  const USirvEpisode *Episode;

  TSharedPtr<sirv_ros::SirvRosHandler> _Ros = nullptr;

  //UPROPERTY()
  static USirvEpisode *static_Episode_;
  //TSharedPtr<USirvEpisode> Episode1 = nullptr;


  size_t TickCuesReceived = 0u;
  
  void BindActions();

};
