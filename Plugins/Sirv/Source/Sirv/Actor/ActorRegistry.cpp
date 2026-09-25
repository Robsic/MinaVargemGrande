// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once


#include "Actor/ActorRegistry.h"
#include "Sirv.h"
#include "Game/Tagger.h"
//#include "Sirv/Traffic/TrafficLightBase.h"
#include "Util/BoundingBoxCalculator.h"

namespace crp = sirv::rpc;

static FActorView::ActorType FActorRegistry_GetActorType(const FActorView &View)
{

  if (!View.IsValid())
  {
    return FActorView::ActorType::INVALID;
  }
  else if (nullptr != Cast<ASirvWheeledVehicle>(View.GetActor()))
  {
    return FActorView::ActorType::Vehicle;
  }
  /*
  else if (nullptr != Cast<ACharacter>(View.GetActor()))
  {
    return FActorView::ActorType::Walker;
  }
  else if (nullptr != Cast<ATrafficLightBase>(View.GetActor()))
  {
    return FActorView::ActorType::TrafficLight;
  }
  else if (nullptr != Cast<ATrafficSignBase>(View.GetActor()))
  {
    return FActorView::ActorType::TrafficSign;
  }*/
  else
  {
    return FActorView::ActorType::Other;
  }
  
}

static FString GetRelevantTagAsString(const TSet<crp::CityObjectLabel> &SemanticTags)
{
  for (auto &&Tag : SemanticTags)
  {
    if ((Tag != crp::CityObjectLabel::None) && (Tag != crp::CityObjectLabel::Other))
    {
      auto Str = ATagger::GetTagAsString(Tag).ToLower();
      return (Str.EndsWith(TEXT("s")) ? Str.LeftChop(1) : Str);
    }
  }
  return TEXT("unknown");
}

FActorView FActorRegistry::Register(AActor &Actor, FActorDescription Description, IdType DesiredId)
{

   UE_LOG(LogSirv, Log, TEXT("Register: '%s' uid: '%d' "), *Description.Id,Description.UId);

  static IdType ID_COUNTER = 0u;
  auto Id = ++ID_COUNTER;

  if (DesiredId != 0 && Id != DesiredId) {
    // check if the desired Id is free, then use it instead
    if (!Actors.Contains(DesiredId)){
      Id = DesiredId;
      if (ID_COUNTER < Id){
        ID_COUNTER = Id;
      }
    }
  }

  Actors.Emplace(Id, &Actor);
  if (Ids.Contains(&Actor))
  {
    UE_LOG(
        LogSirv,
        Warning,
        TEXT("This actor's memory address is already registered, "
             "either you forgot to deregister the actor "
             "or the actor was garbage collected."));
  }
  Ids.Emplace(&Actor, Id);

  auto View = MakeView(Id, Actor, std::move(Description));

  auto Result = ActorDatabase.emplace(Id, View);
  check(Result.second);
  check(static_cast<size_t>(Actors.Num()) == ActorDatabase.size());
  return Result.first->second;
}

void FActorRegistry::Deregister(IdType Id)
{
  check(Contains(Id));
  AActor *Actor = Find(Id).GetActor();
  check(Actor != nullptr);
  ActorDatabase.erase(Id);
  Actors.Remove(Id);
  Ids.Remove(Actor);
  check(static_cast<size_t>(Actors.Num()) == ActorDatabase.size());
}

void FActorRegistry::Deregister(AActor *Actor)
{
  check(Actor != nullptr);
  auto View = Find(Actor);
  check(View.GetActor() == Actor);
  Deregister(View.GetActorId());
}

FActorView FActorRegistry::FindOrFake(AActor *Actor) const
{
  auto View = Find(Actor);
  const bool bFakeActor = (View.GetActor() == nullptr) && (Actor != nullptr);
  return bFakeActor ? MakeView(0u, *Actor, FActorDescription{}) : View;
}

FActorView FActorRegistry::MakeView(
    IdType Id,
    AActor &Actor,
    FActorDescription Description) const
{
  auto Info = MakeShared<FActorInfo>();
  Info->Description = std::move(Description);
  ATagger::GetTagsOfTaggedActor(Actor, Info->SemanticTags);
  Info->BoundingBox = UBoundingBoxCalculator::GetActorBoundingBox(&Actor);

  if (Info->Description.Id.IsEmpty())
  {
    // This is a fake actor, let's fake the id based on their semantic tags.
    Info->Description.Id = TEXT("static.") + GetRelevantTagAsString(Info->SemanticTags);
  }

  Info->SerializedData.id = Id;
  Info->SerializedData.description = Info->Description;
  Info->SerializedData.bounding_box = Info->BoundingBox;
  Info->SerializedData.semantic_tags.reserve(Info->SemanticTags.Num());
  for (auto &&Tag : Info->SemanticTags)
  {
    using tag_t = decltype(Info->SerializedData.semantic_tags)::value_type;
    Info->SerializedData.semantic_tags.emplace_back(static_cast<tag_t>(Tag));
  }
  /* auto *Sensor = Cast<ASensor>(&Actor);
  if (Sensor != nullptr)
  {
    const auto &Token = Sensor->GetToken();
    // Info->SerializedData.stream_token = decltype(Info->SerializedData.stream_token)(
    //     std::begin(Token.data),
    //     std::end(Token.data));
  }
  */
  auto View = FActorView{Id, Actor, std::move(Info)};
  View.Type = FActorRegistry_GetActorType(View);
  return View;
}
