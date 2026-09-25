// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Sirv/Actor/ActorInfo.h"

class AActor;

/// A view over an actor and its properties.
class FActorView
{
public:

  using IdType = uint32;

  enum class ActorType : uint8
  {
    Other,
    Vehicle,
    Walker,
    TrafficLight,
    TrafficSign,
    INVALID
  };

  FActorView() = default;
  FActorView(const FActorView &) = default;
  FActorView(FActorView &&) = default;


  bool IsValid() const
  {
    //return (TheActor != nullptr) && !TheActor->IsPendingKill();
    return (TheActor != nullptr) && ::IsValidChecked(TheActor);
  }

  IdType GetActorId() const
  {
    return Id;
  }

  ActorType GetActorType() const
  {
    return Type;
  }

  AActor *GetActor()
  {
    return TheActor;
  }

  const AActor *GetActor() const
  {
    return TheActor;
  }

  const FActorInfo *GetActorInfo() const
  {
    return Info.Get();
  }

private:

  friend class FActorRegistry;

  FActorView(IdType ActorId, AActor &Actor, TSharedPtr<const FActorInfo> Info)
    : Id(ActorId),
      TheActor(&Actor),
      Info(std::move(Info)) {}

  IdType Id = 0u;

  ActorType Type = ActorType::Other;

  AActor *TheActor = nullptr;

  TSharedPtr<const FActorInfo> Info = nullptr;
};
