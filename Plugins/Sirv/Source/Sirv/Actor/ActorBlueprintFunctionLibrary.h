// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "Sirv/Actor/ActorDefinition.h"
#include "Sirv/Actor/ActorDescription.h"
#include "Sirv/Actor/PedestrianParameters.h"
#include "Sirv/Actor/PropParameters.h"
#include "Sirv/Actor/VehicleParameters.h"
//#include "Sirv/Sensor/GnssSensor.h"
//#include "Sirv/Sensor/Radar.h"
//#include "Sirv/Sensor/InertialMeasurementUnit.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "ActorBlueprintFunctionLibrary.generated.h"

class ASceneCaptureSensor;
class AShaderBasedSensor;
//struct FLidarDescription;

UCLASS()
class UActorBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:

  /// @}
  /// ==========================================================================
  /// @name Actor definition validators
  /// ==========================================================================
  /// @{

  /// Return whether the actor definition is valid. Prints all the errors found.
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static bool CheckActorDefinition(const FActorDefinition &ActorDefinitions);

  /// Return whether the list of actor definitions is valid. Prints all the
  /// errors found.
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static bool CheckActorDefinitions(const TArray<FActorDefinition> &ActorDefinitions);

  /// @}
  /// ==========================================================================
  /// @name Helpers to create actor definitions
  /// ==========================================================================
  /// @{

  static FActorDefinition MakeGenericDefinition(
      const FString &Category,
      const FString &Type,
      const FString &Id);

   static FActorDefinition MakeGenericSensorDefinition(
      const FString &Type,
      const FString &Id);
 
   static FActorDefinition MakeCameraDefinition(
      const FString &Id,
      bool bEnableModifyingPostProcessEffects = false);
 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakeCameraDefinition(
      const FString &Id,
      bool bEnableModifyingPostProcessEffects,
      bool &Success,
      FActorDefinition &Definition);
 
  /* 
  static FActorDefinition MakeLidarDefinition(
      const FString &Id);
 */
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakeLidarDefinition(
      const FString &Id,
      bool &Success,
      FActorDefinition &Definition);
 */
  
  //static FActorDefinition MakeGnssDefinition();
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakeGnssDefinition(
      bool &Success,
      FActorDefinition &Definition);
 */

  //static FActorDefinition MakeIMUDefinition();
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakeIMUDefinition(
      bool &Success,
      FActorDefinition &Definition);
 */
  
  //static FActorDefinition MakeRadarDefinition();
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakeRadarDefinition(
      bool &Success,
      FActorDefinition &Definition);
 */

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakeVehicleDefinition(
      const FVehicleParameters &Parameters,
      bool &Success,
      FActorDefinition &Definition);
 

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakeVehicleDefinitions(
      const TArray<FVehicleParameters> &ParameterArray,
      TArray<FActorDefinition> &Definitions);

 
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakePedestrianDefinition(
      const FPedestrianParameters &Parameters,
      bool &Success,
      FActorDefinition &Definition);
 */
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakePedestrianDefinitions(
      const TArray<FPedestrianParameters> &ParameterArray,
      TArray<FActorDefinition> &Definitions);
 */

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakeTriggerDefinitions(
      const TArray<FString> &ParameterArray,
      TArray<FActorDefinition> &Definitions);
 

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakeTriggerDefinition(
      const FString &Id,
      bool &Success,
      FActorDefinition &Definition);
 
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakePropDefinition(
      const FPropParameters &Parameters,
      bool &Success,
      FActorDefinition &Definition);
 */
/* 
  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void MakePropDefinitions(
      const TArray<FPropParameters> &ParameterArray,
      TArray<FActorDefinition> &Definitions);
 */
/* 
  UFUNCTION()
  static void MakeObstacleDetectorDefinitions(
      const FString &Type,
      const FString &Id,
      FActorDefinition &Definition);
 */
  /// @}
  /// ==========================================================================
  /// @name Helpers to retrieve attribute values
  /// ==========================================================================
  /// @{

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static bool ActorAttributeToBool(const FActorAttribute &ActorAttribute, bool Default);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static int32 ActorAttributeToInt(const FActorAttribute &ActorAttribute, int32 Default);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static float ActorAttributeToFloat(const FActorAttribute &ActorAttribute, float Default);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static FString ActorAttributeToString(const FActorAttribute &ActorAttribute, const FString &Default);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static FColor ActorAttributeToColor(const FActorAttribute &ActorAttribute, const FColor &Default);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static bool RetrieveActorAttributeToBool(
      const FString &Id,
      const TMap<FString, FActorAttribute> &Attributes,
      bool Default);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static int32 RetrieveActorAttributeToInt(
      const FString &Id,
      const TMap<FString, FActorAttribute> &Attributes,
      int32 Default);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static float RetrieveActorAttributeToFloat(
      const FString &Id,
      const TMap<FString, FActorAttribute> &Attributes,
      float Default);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static FString RetrieveActorAttributeToString(
      const FString &Id,
      const TMap<FString, FActorAttribute> &Attributes,
      const FString &Default);

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static FColor RetrieveActorAttributeToColor(
      const FString &Id,
      const TMap<FString, FActorAttribute> &Attributes,
      const FColor &Default);

  /// @}
  /// ==========================================================================
  /// @name Helpers to set Actors
  /// ==========================================================================
  /// @{

  UFUNCTION(Category = "Sirv Actor", BlueprintCallable)
  static void SetCamera(const FActorDescription &Description, ASceneCaptureSensor *Camera);
  static void SetCamera(const FActorDescription &Description, AShaderBasedSensor *Camera);

  //static void SetLidar(const FActorDescription &Description, FLidarDescription &Lidar);

  //static void SetGnss(const FActorDescription &Description, AGnssSensor *Gnss);

  //static void SetIMU(const FActorDescription &Description, AInertialMeasurementUnit *IMU);

  //static void SetRadar(const FActorDescription &Description, ARadar *Radar);
};
