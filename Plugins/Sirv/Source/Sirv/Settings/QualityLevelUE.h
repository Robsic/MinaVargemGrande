// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

//#include <compiler/disable-ue4-macros.h>
//#include <sirv/rpc/QualityLevel.h>
//#include <compiler/enable-ue4-macros.h>

#include "QualityLevelUE.generated.h"

namespace sirv {
namespace rpc {

  enum class QualityLevel : uint8_t {
    Low,
    // Medium,
    // High,
    Epic,

    SIZE,
    INVALID
  };

} // namespace rpc
} // namespace carla
  
  
#define SIRV_ENUM_FROM_RPC(e) static_cast<uint8>(sirv::rpc::QualityLevel:: e)

UENUM(BlueprintType)
enum class EQualityLevel : uint8
{
  Null = 0, // Workarround for UE4.24 issue with enums
  Low    = SIRV_ENUM_FROM_RPC(Low)    UMETA(DisplayName = "Low"),
  // Medium = SIRV_ENUM_FROM_RPC(Medium) UMETA(DisplayName = "Medium"),
  // High   = SIRV_ENUM_FROM_RPC(High)   UMETA(DisplayName = "High"),
  Epic   = SIRV_ENUM_FROM_RPC(Epic)   UMETA(DisplayName = "Epic"),

  SIZE      UMETA(Hidden),
  INVALID   UMETA(Hidden)
};

static_assert(
    static_cast<uint8>(EQualityLevel::SIZE) == static_cast<uint8>(sirv::rpc::QualityLevel::SIZE),
    "Please keep these two enums in sync.");

#undef SIRV_ENUM_FROM_RPC
