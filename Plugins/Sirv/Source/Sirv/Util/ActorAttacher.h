// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

//#include <compiler/disable-ue4-macros.h>
#include <sirv/rpc/AttachmentType.h>
//#include <compiler/enable-ue4-macros.h>

#include <cstdint>

// namespace sirv {
// namespace rpc {

//   enum class AttachmentType : uint8_t {
//     Rigid,
//     SpringArm,

//     SIZE,
//     INVALID
//   };

// } // namespace rpc
// } // namespace sirv

#include "Kismet/BlueprintFunctionLibrary.h"

#include "ActorAttacher.generated.h"

#define SIRV_ENUM_FROM_RPC(e) static_cast<uint8>(sirv::rpc::AttachmentType:: e)

UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
  Null = 0, // Workarround for UE4.24 issue with enums
  Rigid      = SIRV_ENUM_FROM_RPC(Rigid)      UMETA(DisplayName = "Rigid"),
  SpringArm  = SIRV_ENUM_FROM_RPC(SpringArm)  UMETA(DisplayName = "SpringArm"),

  SIZE      UMETA(Hidden),
  INVALID   UMETA(Hidden)
};

static_assert(
    static_cast<uint8>(EAttachmentType::SIZE) == static_cast<uint8>(sirv::rpc::AttachmentType::SIZE),
    "Please keep these two enums in sync.");

#undef SIRV_ENUM_FROM_RPC

UCLASS()
class SIRV_API UActorAttacher : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:

  UFUNCTION(BlueprintCallable, Category="SIRV|Actor Attacher")
  static void AttachActors(AActor *Child, AActor *Parent, EAttachmentType AttachmentType);
};
