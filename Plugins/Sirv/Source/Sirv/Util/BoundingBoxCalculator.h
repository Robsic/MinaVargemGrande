// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Util/BoundingBox.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "BoundingBoxCalculator.generated.h"

class AActor;

UCLASS()
class SIRV_API UBoundingBoxCalculator : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:

  /// Compute the bounding box of the given Sirv actor.
  ///
  /// @warning If the actor type is not supported a default initialized bounding
  /// box is returned.
  ///
  /// @warning Traffic signs return its trigger box instead.
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static FBoundingBox GetActorBoundingBox(
    const AActor *Actor,
    uint8 InTagQueried = 0xFF);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static FBoundingBox GetVehicleBoundingBox(
    const ASirvWheeledVehicle* Vehicle,
    uint8 InTagQueried = 0xFF);
 
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static FBoundingBox GetCharacterBoundingBox(
    const ACharacter* Character,
    uint8 InTagQueried = 0xFF);
 */
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void GetTrafficLightBoundingBox(
    const ATrafficLightBase* TrafficLight,
    TArray<FBoundingBox>& OutBB,
    TArray<uint8>& OutTag,
    uint8 InTagQueried = 0xFF);
 */
  UFUNCTION(Category = "Sirv Util", BlueprintCallable)
  static FBoundingBox GetSkeletalMeshBoundingBox(const USkeletalMesh* SkeletalMesh);

  UFUNCTION(Category = "Sirv Util", BlueprintCallable)
  static FBoundingBox GetStaticMeshBoundingBox(const UStaticMesh* StaticMesh);
/* 
  UFUNCTION(Category = "Sirv Util", BlueprintCallable)
  static void GetISMBoundingBox(
    UInstancedStaticMeshComponent* ISMComp,
    TArray<FBoundingBox>& OutBoundingBox);
 */
  UFUNCTION(Category = "Sirv Util", BlueprintCallable)
  static void GetBBsOfStaticMeshComponents(
    const TArray<UStaticMeshComponent*>& StaticMeshComps,
    TArray<FBoundingBox>& OutBB,
    TArray<uint8>& OutTag,
    uint8 InTagQueried = 0xFF);

  UFUNCTION(Category = "Sirv Util", BlueprintCallable)
  static void GetBBsOfSkeletalMeshComponents(
    const TArray<USkeletalMeshComponent*>& SkeletalMeshComps,
    TArray<FBoundingBox>& OutBB,
    TArray<uint8>& OutTag,
    uint8 InTagQueried = 0xFF);

   UFUNCTION(Category = "Sirv Util", BlueprintCallable)
  static TArray<FBoundingBox> GetBoundingBoxOfActors(
    const TArray<AActor*>& Actors,
    uint8 InTagQueried = 0xFF);
 
   UFUNCTION(Category = "Sirv Util", BlueprintCallable)
  static TArray<FBoundingBox> GetBBsOfActor(
    const AActor* Actor,
    uint8 InTagQueried = 0xFF);
 
  // Combines the BBs of an actor based on the distance and type of the BB
  // The BBs not combined are included too (ie: TL BBs and pole)
  // DistanceThreshold is the maximum distance between BBs to combine, if 0.0 the distance is ignored
  // TagToCombine defines the type of the BBs to combine, if 0 the type is ignored
  UFUNCTION(Category = "Sirv Util", BlueprintCallable)
  static void CombineBBsOfActor(
    const AActor* Actor,
    TArray<FBoundingBox>& OutBB,
    TArray<uint8>& OutTag,
    const float DistanceThreshold = 0.0f,
    uint8 TagToCombine = 0xFF);

  UFUNCTION(Category = "Sirv Util", BlueprintCallable)
  static FBoundingBox CombineBBs(const TArray<FBoundingBox>& BBsToCombine);

  // Returns Static Mesh Components that generate the InBB of the Actor
  // ie: the SMComps that creates the BB of the TL light box
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void GetMeshCompsFromActorBoundingBox(
    const AActor* Actor,
    const FBoundingBox& InBB,
    TArray<UStaticMeshComponent*>& OutStaticMeshComps);

};
