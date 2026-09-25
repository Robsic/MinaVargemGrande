// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once



#include <compiler/disable-ue4-macros.h>
#include <sirv/rpc/ActorAttributeType.h>
#include <compiler/enable-ue4-macros.h>

#include "ActorAttribute.generated.h"

#define SIRV_ENUM_FROM_RPC(e) static_cast<uint8>(sirv::rpc::ActorAttributeType:: e)


/// List of valid types for actor attributes.
UENUM(BlueprintType)
enum class EActorAttributeType : uint8
{
  Null = 0, // Workarround for UE4.24 issue with enums
  Bool      = SIRV_ENUM_FROM_RPC(Bool)      UMETA(DisplayName = "Bool"),
  Int       = SIRV_ENUM_FROM_RPC(Int)       UMETA(DisplayName = "Integer"),
  Float     = SIRV_ENUM_FROM_RPC(Float)     UMETA(DisplayName = "Float"),
  String    = SIRV_ENUM_FROM_RPC(String)    UMETA(DisplayName = "String"),
  RGBColor  = SIRV_ENUM_FROM_RPC(RGBColor)  UMETA(DisplayName = "RGB Color (comma separated)"),

  SIZE        UMETA(Hidden),
  INVALID     UMETA(Hidden)
};

#undef SIRV_ENUM_FROM_RPC

/// Definition of an actor variation. Variations represent attributes of the
/// actor that the user can modify in order to generate variations of the same
/// actor.
///
/// A list of recommended values is given. If bRestrictToRecommended is true,
/// only recommended values are accepted as valid.
USTRUCT(BlueprintType)
struct SIRV_API FActorVariation
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Id;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EActorAttributeType Type = EActorAttributeType::String;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FString> RecommendedValues;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bRestrictToRecommended = false;
};

/// An actor attribute, may be an intrinsic (non-modifiable) attribute of the
/// actor or an user-defined actor variation.
USTRUCT(BlueprintType)
struct SIRV_API FActorAttribute
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Id;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EActorAttributeType Type = EActorAttributeType::String;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Value;
};
