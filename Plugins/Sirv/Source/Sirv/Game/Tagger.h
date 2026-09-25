// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

//#include "Util/rpc/ObjectLabel.h"

#include <compiler/disable-ue4-macros.h>
#include <sirv/rpc/ObjectLabel.h>
#include <compiler/enable-ue4-macros.h>

#include "Tagger.generated.h"

namespace crp = sirv::rpc;

/// Sets actors' custom depth stencil value for semantic segmentation according
/// to their meshes.
///
/// Non-static functions present so it can be dropped into the scene for testing
/// purposes.
UCLASS()
class SIRV_API ATagger : public AActor
{
  GENERATED_BODY()

public:

  /// Set the tag of an actor.
  ///
  /// If bTagForSemanticSegmentation true, activate the custom depth pass. This
  /// pass is necessary for rendering the semantic segmentation. However, it may
  /// add a performance penalty since occlusion doesn't seem to be applied to
  /// objects having this value active.
  static void TagActor(const AActor &Actor, bool bTagForSemanticSegmentation);


  /// Set the tag of every actor in level.
  ///
  /// If bTagForSemanticSegmentation true, activate the custom depth pass. This
  /// pass is necessary for rendering the semantic segmentation. However, it may
  /// add a performance penalty since occlusion doesn't seem to be applied to
  /// objects having this value active.
  static void TagActorsInLevel(UWorld &World, bool bTagForSemanticSegmentation);

  /// Retrieve the tag of an already tagged component.
  static crp::CityObjectLabel GetTagOfTaggedComponent(const UPrimitiveComponent &Component)
  {
    return static_cast<crp::CityObjectLabel>(Component.CustomDepthStencilValue);
  }

  /// Retrieve the tags of an already tagged actor. CityObjectLabel::None is
  /// not added to the array.
  static void GetTagsOfTaggedActor(const AActor &Actor, TSet<crp::CityObjectLabel> &Tags);

  /// Return true if @a Component has been tagged with the given @a Tag.
  static bool MatchComponent(const UPrimitiveComponent &Component, crp::CityObjectLabel Tag)
  {
    return (Tag == GetTagOfTaggedComponent(Component));
  }

  /// Retrieve the tags of an already tagged actor. CityObjectLabel::None is
  /// not added to the array.
  static FString GetTagAsString(crp::CityObjectLabel Tag);

  /// Method that computes the label corresponding to a folder path
  static crp::CityObjectLabel GetLabelByFolderName(const FString &String);

  /// Method that computes the label corresponding to an specific object
  /// using the folder path in which it is stored
  template <typename T>
  static crp::CityObjectLabel GetLabelByPath(const T *Object) {
    const FString Path = Object->GetPathName();
    TArray<FString> StringArray;
    Path.ParseIntoArray(StringArray, TEXT("/"), false);
    return (StringArray.Num() > 4 ? GetLabelByFolderName(StringArray[4]) : crp::CityObjectLabel::None);
  }

  static void SetStencilValue(UPrimitiveComponent &Component,
    const crp::CityObjectLabel &Label, const bool bSetRenderCustomDepth);

  ATagger();

protected:

#if WITH_EDITOR
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

private:

  UPROPERTY(Category = "Tagger", EditAnywhere)
  bool bTriggerTagObjects = false;

  UPROPERTY(Category = "Tagger", EditAnywhere)
  bool bTagForSemanticSegmentation = false;
};
