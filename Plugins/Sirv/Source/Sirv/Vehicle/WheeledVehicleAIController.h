// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include <queue>

#include "GameFramework/Controller.h"

//#include "Traffic/TrafficLightState.h"
#include "Vehicle/VehicleControl.h"

#include "WheeledVehicleAIController.generated.h"

class ASirvWheeledVehicle;
class URandomEngine;
//class URoadMap;

/// Wheeled vehicle controller with optional AI.
UCLASS()
class SIRV_API AWheeledVehicleAIController final : public AController
{
  GENERATED_BODY()

  // ===========================================================================
  /// @name Constructor and destructor
  // ===========================================================================
  /// @{

public:

  AWheeledVehicleAIController(const FObjectInitializer &ObjectInitializer);

  ~AWheeledVehicleAIController();

  /// @}
  // ===========================================================================
  /// @name Controller overrides
  // ===========================================================================
  /// @{

public:

  void OnPossess(APawn *aPawn) override;

  void OnUnPossess() override;

  void Tick(float DeltaTime) override;

  /// @}
  // ===========================================================================
  /// @name Possessed vehicle
  // ===========================================================================
  /// @{

public:

  UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  bool IsPossessingAVehicle() const
  {
    return Vehicle != nullptr;
  }

  UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  ASirvWheeledVehicle *GetPossessedVehicle()
  {
    return Vehicle;
  }

  const ASirvWheeledVehicle *GetPossessedVehicle() const
  {
    return Vehicle;
  }

  /// @}
  // ===========================================================================
  /// @name Control options
  // ===========================================================================
  /// @{

  UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  void SetStickyControl(bool bEnabled)
  {
    bControlIsSticky = bEnabled;
  }

  /// @}
  // ===========================================================================
  /// @name Road map
  // ===========================================================================
  /// @{

public:

  //void SetRoadMap(URoadMap *InRoadMap)
  //{
  //  RoadMap = InRoadMap;
  //}

  //UFUNCTION(Category = "Road Map", BlueprintCallable)
  //URoadMap *GetRoadMap()
  //{
  //  return RoadMap;
  //}

  /// @}
  // ===========================================================================
  /// @name Random engine
  // ===========================================================================
  /// @{

public:

  UFUNCTION(Category = "Random Engine", BlueprintCallable)
  URandomEngine *GetRandomEngine()
  {
    check(RandomEngine != nullptr);
    return RandomEngine;
  }

  /// @}
  // ===========================================================================
  /// @name Autopilot
  // ===========================================================================
  /// @{

public:

  UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  bool IsAutopilotEnabled() const
  {
    return bAutopilotEnabled;
  }

  UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  void SetAutopilot(bool Enable, bool KeepState = false)
  {
    if (IsAutopilotEnabled() != Enable)
    {
      ConfigureAutopilot(Enable, KeepState);
    }
  }

  UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  void ToggleAutopilot()
  {
    ConfigureAutopilot(!bAutopilotEnabled);
  }

private:

  void ConfigureAutopilot(const bool Enable, const bool KeepState = false);

  /// @}
  // ===========================================================================
  /// @name Traffic
  // ===========================================================================
  /// @{

public:

  /// Get current speed limit in km/h.
  UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  float GetSpeedLimit() const
  {
    return SpeedLimit;
  }

  /// Set vehicle's speed limit in km/h.
  UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  void SetSpeedLimit(float InSpeedLimit)
  {
    SpeedLimit = InSpeedLimit;
  }

  /// Get traffic light state currently affecting this vehicle.
  //UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  //ETrafficLightState GetTrafficLightState() const
  //{
  //  return TrafficLightState;
  //}

  /// Set traffic light state currently affecting this vehicle.
  //UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  //void SetTrafficLightState(ETrafficLightState InTrafficLightState)
  //{
  //  TrafficLightState = InTrafficLightState;
  //}

  /// Get traffic light currently affecting this vehicle.
  //UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  //ATrafficLightBase *GetTrafficLight() const
  //{
  //  return TrafficLight;
  //}

  /// Set traffic light currently affecting this vehicle.
  //UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  //void SetTrafficLight(ATrafficLightBase *InTrafficLight)
  //{
  //  TrafficLight = InTrafficLight;
  //}

  /// Set a fixed route to follow if autopilot is enabled.
  UFUNCTION(Category = "Wheeled Vehicle Controller", BlueprintCallable)
  void SetFixedRoute(const TArray<FVector> &Locations, bool bOverwriteCurrent = true);

  /// @}

private:

  UPROPERTY()
  ASirvWheeledVehicle *Vehicle = nullptr;

  //UPROPERTY()
  //URoadMap *RoadMap = nullptr;

  UPROPERTY()
  URandomEngine *RandomEngine = nullptr;

  UPROPERTY(VisibleAnywhere)
  bool bAutopilotEnabled = false;

  UPROPERTY(VisibleAnywhere)
  bool bControlIsSticky = true;

  UPROPERTY(VisibleAnywhere)
  float SpeedLimit = 50.0f;

  //UPROPERTY(VisibleAnywhere)
  //ETrafficLightState TrafficLightState = ETrafficLightState::Green;

  UPROPERTY(VisibleAnywhere)
  float MaximumSteerAngle = -1.0f;

  //UPROPERTY()
  //ATrafficLightBase *TrafficLight;

  std::queue<FVector> TargetLocations;
};
