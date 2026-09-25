// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "ProceduralCustomMesh.generated.h"

/// A definition of a Sirv Mesh.
USTRUCT()
struct SIRV_API FProceduralCustomMesh
{
  GENERATED_BODY()

  UPROPERTY()
  TArray<FVector> Vertices;

  UPROPERTY()
  TArray<int32> Triangles;

  UPROPERTY()
  TArray<FVector> Normals;

  UPROPERTY()
  TArray<FVector2D> UV0;

  UPROPERTY()
  TArray<FLinearColor> VertexColor;

  // This is commented due to an strange bug including ProceduralMeshComponent.h
  // UPROPERTY()
  // TArray<FProcMeshTangent> Tangents;
};
