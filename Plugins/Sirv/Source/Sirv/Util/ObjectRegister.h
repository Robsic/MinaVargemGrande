// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once


#include "Sirv/Vehicle/SirvWheeledVehicle.h"
//#include "Sirv/Traffic/TrafficLightBase.h"
#include "Sirv/Util/EnvironmentObject.h"

#include "ObjectRegister.generated.h"

class AActor;

UCLASS()
class SIRV_API UObjectRegister : public UObject
{
  GENERATED_BODY()

public:

  UObjectRegister() {}
  ~UObjectRegister() {}

  UFUNCTION(Category = "Sirv Object Register", BlueprintCallable, CallInEditor)
  TArray<FEnvironmentObject> GetEnvironmentObjects(uint8 InTagQueried = 0xFF) const;

  UFUNCTION(Category = "Sirv Object Register")
  void RegisterObjects(TArray<AActor*> Actors);

  UFUNCTION(Category = "Sirv Object Register")
  void EnableEnvironmentObjects(const TSet<uint64>& EnvObjectIds, bool Enable);

private:

  void RegisterEnvironmentObject(
    AActor* Actor,
    FBoundingBox& BoundingBox,
    EnvironmentObjectType Type,
    uint8 Tag);

  void RegisterVehicle(ASirvWheeledVehicle* Vehicle);

  //void RegisterCharacter(ACharacter* Character);

  //void RegisterTrafficLight(ATrafficLightBase* TrafficLight);

  //void RegisterISMComponents(AActor* Actor);

  //void RegisterSMComponents(AActor* Actor);

  //void RegisterSKMComponents(AActor* Actor);

  void EnableEnvironmentObject(FEnvironmentObject& EnvironmentObject, bool Enable);

  void EnableActor(FEnvironmentObject& EnvironmentObject, bool Enable);

  //void EnableTrafficLight(FEnvironmentObject& EnvironmentObject, bool Enable);

  //void EnableISMComp(FEnvironmentObject& EnvironmentObject, bool Enable);

  TMultiMap<uint64, const UStaticMeshComponent*> ObjectIdToComp;

  UPROPERTY(Category = "Sirv Object Register", EditAnywhere)
  TArray<FEnvironmentObject> EnvironmentObjects;

  int FoliageActorInstanceCount = 0;

};
