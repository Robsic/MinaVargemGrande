// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "CoreMinimal.h"

#include "VehicleVelocityControl.generated.h"

/// Component that controls that the velocity of an actor is constant.
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIRV_API UVehicleVelocityControl : public UActorComponent
{
  GENERATED_BODY()

  // ===========================================================================
  /// @name Constructor and destructor
  // ===========================================================================
  /// @{
public:
  UVehicleVelocityControl();

  /// @}
  // ===========================================================================
  /// @name Get functions
  // ===========================================================================
  /// @{
public:

  void BeginPlay() override;

  virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

  // Activate the component setting the target velocity
  virtual void Activate(bool bReset=false) override;

  // Activate the component setting the target velocity
  virtual void Activate(FVector Velocity, bool bReset=false);

  // Deactivate the component
  virtual void Deactivate() override;

private:
  ///
  UPROPERTY(Category = "Vehicle Velocity Control", VisibleAnywhere)
  FVector TargetVelocity;

  UPrimitiveComponent* PrimitiveComponent;
  AActor* OwnerVehicle;

};
