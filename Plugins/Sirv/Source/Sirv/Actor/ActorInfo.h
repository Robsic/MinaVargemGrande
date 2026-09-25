// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Actor/ActorDescription.h"
#include "Game/Tagger.h"

#include "Util/BoundingBox.h"

#include <compiler/disable-ue4-macros.h>
#include <sirv/rpc/Actor.h>
#include <sirv/rpc/ObjectLabel.h>
#include <compiler/enable-ue4-macros.h>

namespace crp = sirv::rpc;

/// A view over an actor and its properties.
struct FActorInfo
{
public:

  FActorDescription Description;

  TSet<crp::CityObjectLabel> SemanticTags;

  FBoundingBox BoundingBox;

  crp::Actor SerializedData;

  /// @todo To be used solely by the FWorldObserver.
  mutable FVector Velocity = {0.0f, 0.0f, 0.0f};
};
