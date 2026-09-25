// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "SirvWheeledVehicleState.generated.h"

/// State of a ASirvWheeledVehicle, to be displayed in editor for debugging
/// purposes.
UENUM(BlueprintType)
enum class ESirvWheeledVehicleState : uint8
{
  AutopilotOff UMETA(DisplayName        = "Autopilot Off"),
  FreeDriving UMETA(DisplayName         = "Free driving"),
  FollowingFixedRoute UMETA(DisplayName = "Following fixed route"),
  WaitingForRedLight UMETA(DisplayName  = "Waiting for red light"),
  ObstacleAhead UMETA(DisplayName       = "Obstacle ahead"),
  UNKNOWN UMETA(DisplayName             = "Unknown")
};
