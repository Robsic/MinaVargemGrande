// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "BoundingBox.generated.h"

USTRUCT(BlueprintType)
struct SIRV_API FBoundingBox
{
  GENERATED_BODY()


  /// Object label number (Building = 1; Vehicles = 10; Pedestrians = 4; ... ObjectLabel.h)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int Utype = 0xff;

  /// Origin of the bounding box relative to its owner.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector Origin = {0.0f, 0.0f, 0.0f};

  /// Radii extent of the bounding box.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector Extent = {0.0f, 0.0f, 0.0f};

  /// Rotation of the bounding box.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FRotator Rotation = {0.0f, 0.0f, 0.0f};

  /// Class name of object
  //UPROPERTY(EditAnywhere, BlueprintReadWrite)
  //FString ClassName = TEXT("");
};
