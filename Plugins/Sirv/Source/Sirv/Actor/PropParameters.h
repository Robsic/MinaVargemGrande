// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Engine/StaticMesh.h"

#include "PropParameters.generated.h"

UENUM(BlueprintType)
enum class EPropSize : uint8
{

  Tiny        UMETA(DisplayName = "Tiny",   ToolTip = "Smaller than a mailbox"),
  Small       UMETA(DisplayName = "Small",  ToolTip = "Size of a mailbox"),
  Medium      UMETA(DisplayName = "Medium", ToolTip = "Size of a human"),
  Big         UMETA(DisplayName = "Big",    ToolTip = "Size of a bus stop"),
  Huge        UMETA(DisplayName = "Huge",   ToolTip = "Size of a house or bigger"),

  SIZE        UMETA(Hidden),
  INVALID     UMETA(DisplayName = "INVALID")
};

USTRUCT(BlueprintType)
struct SIRV_API FPropParameters
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Name;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticMesh *Mesh;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EPropSize Size = EPropSize::INVALID;
};
