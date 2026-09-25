// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#include "Game/Tagger.h"
#include "Sirv.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "PhysicsEngine/PhysicsAsset.h"

namespace crp = sirv::rpc;

template <typename T>
static auto CastEnum(T label)
{
  return static_cast<typename std::underlying_type<T>::type>(label);
}

crp::CityObjectLabel ATagger::GetLabelByFolderName(const FString &String) {
  if      (String == "Building")     return crp::CityObjectLabel::Buildings;
  else if (String == "Fence")        return crp::CityObjectLabel::Fences;
  else if (String == "Pedestrian")   return crp::CityObjectLabel::Pedestrians;
  else if (String == "Pole")         return crp::CityObjectLabel::Poles;
  else if (String == "Other")        return crp::CityObjectLabel::Other;
  else if (String == "Road")         return crp::CityObjectLabel::Roads;
  else if (String == "RoadLine")     return crp::CityObjectLabel::RoadLines;
  else if (String == "SideWalk")     return crp::CityObjectLabel::Sidewalks;
  else if (String == "TrafficSign")  return crp::CityObjectLabel::TrafficSigns;
  else if (String == "Vegetation")   return crp::CityObjectLabel::Vegetation;
  else if (String == "Vehicles")     return crp::CityObjectLabel::Vehicles;
  else if (String == "Wall")         return crp::CityObjectLabel::Walls;
  else if (String == "Sky")          return crp::CityObjectLabel::Sky;
  else if (String == "Ground")       return crp::CityObjectLabel::Ground;
  else if (String == "Bridge")       return crp::CityObjectLabel::Bridge;
  else if (String == "RailTrack")    return crp::CityObjectLabel::RailTrack;
  else if (String == "GuardRail")    return crp::CityObjectLabel::GuardRail;
  else if (String == "TrafficLight") return crp::CityObjectLabel::TrafficLight;
  else if (String == "Static")       return crp::CityObjectLabel::Static;
  else if (String == "Dynamic")      return crp::CityObjectLabel::Dynamic;
  else if (String == "Water")        return crp::CityObjectLabel::Water;
  else if (String == "Terrain")      return crp::CityObjectLabel::Terrain;
  else                               return crp::CityObjectLabel::None;
}

void ATagger::SetStencilValue(
    UPrimitiveComponent &Component,
    const crp::CityObjectLabel &Label,
    const bool bSetRenderCustomDepth) {
  Component.SetCustomDepthStencilValue(CastEnum(Label));
  Component.SetRenderCustomDepth(
      bSetRenderCustomDepth &&
      (Label != crp::CityObjectLabel::None));
}

// =============================================================================
// -- static ATagger functions -------------------------------------------------
// =============================================================================

void ATagger::TagActor(const AActor &Actor, bool bTagForSemanticSegmentation)
{
#ifdef SIRV_TAGGER_EXTRA_LOG
  UE_LOG(LogSirv, Log, TEXT("Actor: %s"), *Actor.GetName());
#endif // SIRV_TAGGER_EXTRA_LOG

  // Iterate static meshes.
  TArray<UStaticMeshComponent *> StaticMeshComponents;
  Actor.GetComponents<UStaticMeshComponent>(StaticMeshComponents);
  for (UStaticMeshComponent *Component : StaticMeshComponents) {
    //const auto Label = GetLabelByPath(Component->GetStaticMesh());
    const auto Label = crp::CityObjectLabel::None;
    SetStencilValue(*Component, Label, bTagForSemanticSegmentation);
#ifdef SIRV_TAGGER_EXTRA_LOG
    UE_LOG(LogSirv, Log, TEXT("  + StaticMeshComponent: %s"), *Component->GetName());
    UE_LOG(LogSirv, Log, TEXT("    - Label: \"%s\""), *GetTagAsString(Label));
#endif // SIRV_TAGGER_EXTRA_LOG
  }

  // Iterate skeletal meshes.
  TArray<USkeletalMeshComponent *> SkeletalMeshComponents;
  Actor.GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
  for (USkeletalMeshComponent *Component : SkeletalMeshComponents) {
    const auto Label = ATagger::GetLabelByPath(Component->GetPhysicsAsset());
    SetStencilValue(*Component, Label, bTagForSemanticSegmentation);
#ifdef SIRV_TAGGER_EXTRA_LOG
    UE_LOG(LogSirv, Log, TEXT("  + SkeletalMeshComponent: %s"), *Component->GetName());
    UE_LOG(LogSirv, Log, TEXT("    - Label: \"%s\""), *GetTagAsString(Label));
#endif // SIRV_TAGGER_EXTRA_LOG
  }
}

void ATagger::TagActorsInLevel(UWorld &World, bool bTagForSemanticSegmentation)
{
  for (TActorIterator<AActor> it(&World); it; ++it) {
    TagActor(**it, bTagForSemanticSegmentation);
  }
}

void ATagger::GetTagsOfTaggedActor(const AActor &Actor, TSet<crp::CityObjectLabel> &Tags)
{
  TArray<UPrimitiveComponent *> Components;
  Actor.GetComponents<UPrimitiveComponent>(Components);
  for (auto *Component : Components) {
    if (Component != nullptr) {
      const auto Tag = GetTagOfTaggedComponent(*Component);
      if (Tag != crp::CityObjectLabel::None) {
        Tags.Add(Tag);
      }
    }
  }
}

FString ATagger::GetTagAsString(const crp::CityObjectLabel Label)
{
  switch (Label) {
#define SIRV_GET_LABEL_STR(lbl) case crp::CityObjectLabel:: lbl : return TEXT(#lbl);
    default:
    SIRV_GET_LABEL_STR(None)
    SIRV_GET_LABEL_STR(Buildings)
    SIRV_GET_LABEL_STR(Fences)
    SIRV_GET_LABEL_STR(Other)
    SIRV_GET_LABEL_STR(Pedestrians)
    SIRV_GET_LABEL_STR(Poles)
    SIRV_GET_LABEL_STR(RoadLines)
    SIRV_GET_LABEL_STR(Roads)
    SIRV_GET_LABEL_STR(Sidewalks)
    SIRV_GET_LABEL_STR(TrafficSigns)
    SIRV_GET_LABEL_STR(Vegetation)
    SIRV_GET_LABEL_STR(Vehicles)
    SIRV_GET_LABEL_STR(Walls)
    SIRV_GET_LABEL_STR(Sky)
    SIRV_GET_LABEL_STR(Ground)
    SIRV_GET_LABEL_STR(Bridge)
    SIRV_GET_LABEL_STR(RailTrack)
    SIRV_GET_LABEL_STR(GuardRail)
    SIRV_GET_LABEL_STR(TrafficLight)
    SIRV_GET_LABEL_STR(Static)
    SIRV_GET_LABEL_STR(Dynamic)
    SIRV_GET_LABEL_STR(Water)
    SIRV_GET_LABEL_STR(Terrain)
#undef SIRV_GET_LABEL_STR
  }
}

// =============================================================================
// -- non-static ATagger functions ---------------------------------------------
// =============================================================================

ATagger::ATagger()
{
  PrimaryActorTick.bCanEverTick = false;
}

#if WITH_EDITOR
void ATagger::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);
  if (PropertyChangedEvent.Property) {
    if (bTriggerTagObjects && (GetWorld() != nullptr)) {
      TagActorsInLevel(*GetWorld(), bTagForSemanticSegmentation);
    }
  }
  bTriggerTagObjects = false;
}
#endif // WITH_EDITOR
