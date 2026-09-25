// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

 	

#include "Camera/CameraComponent.h"

#include "Screen/SirvScreenSceneCaptureComponent2D.h"

//#include "WheeledVehicle.h"
#include "WheeledVehiclePawn.h"

#include "Vehicle/SirvWheeledVehicleState.h"
#include "Vehicle/VehicleControl.h"
#include "Vehicle/VehicleLightState.h"
#include "Vehicle/VehicleSoundState.h"
#include "Vehicle/VehicleEffectsState.h"
#include "Vehicle/VehicleTelemetry.h"
#include "Vehicle/VehicleInputPriority.h"
#include "Vehicle/VehiclePhysicsControl.h"
#include "VehicleVelocityControl.h"

#include "ChaosWheeledVehicleMovementComponent.h"
//#include "WheeledVehicleMovementComponent4W.h"

#include "CoreMinimal.h"


#include "SirvWheeledVehicle.generated.h"

class UBoxComponent;
class UCameraComponent;

/// Base class for SIRV wheeled vehicles.
UCLASS()
class SIRV_API ASirvWheeledVehicle : public AWheeledVehiclePawn
{
  GENERATED_BODY()

  // ===========================================================================
  /// @name Constructor and destructor
  // ===========================================================================
  /// @{
public:

  ASirvWheeledVehicle(const FObjectInitializer &ObjectInitializer);

  ~ASirvWheeledVehicle();

  /// @}
  // ===========================================================================
  /// @name Get functions
  // ===========================================================================
  /// @{
public:

  /// Vehicle control currently applied to this vehicle.
  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  const FVehicleControl &GetVehicleControl() const
  {
    return LastAppliedControl;
  }

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FVector GetVehicleAngularVelocity() const;


  /// Transform of the vehicle. Location is shifted so it matches center of the
  /// vehicle bounds rather than the actor's location.
  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FTransform GetVehicleTransform() const
  {
    return GetActorTransform();
  }

  /// Forward speed in cm/s. Might be negative if goes backwards.
  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  float GetVehicleForwardSpeed() const;

  /// Orientation vector of the vehicle, pointing forward.
  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FVector GetVehicleOrientation() const;

  /// Active gear of the vehicle.
  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  int32 GetVehicleCurrentGear() const;

  /// Transform of the vehicle's bounding box relative to the vehicle.
  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FTransform GetVehicleBoundingBoxTransform() const;

  /// Extent of the vehicle's bounding box.
  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FVector GetVehicleBoundingBoxExtent() const;

  /// Get vehicle's bounding box component.
  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  UBoxComponent *GetVehicleBoundingBox() const
  {
    return VehicleBounds;
  }

  
  /// Get the maximum angle at which the front wheel can steer.
  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  float GetMaximumSteerAngle() const;

  /// @}
  // ===========================================================================
  /// @name AI debug state
  // ===========================================================================
  /// @{
public:

  /// @todo This function should be private to AWheeledVehicleAIController.
  void SetAIVehicleState(ESirvWheeledVehicleState InState)
  {
    State = InState;
  }

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  ESirvWheeledVehicleState GetAIVehicleState() const
  {
    return State;
  }

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FVehiclePhysicsControl GetVehiclePhysicsControl() const;

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FVehicleLightState GetVehicleLightState() const;

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FVehicleSoundState GetVehicleSoundState() const;

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FVehicleEffectsState GetVehicleEffectsState() const;

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  FVehicleTelemetry GetVehicleTelemetry() const;


  void ApplyVehiclePhysicsControl(const FVehiclePhysicsControl &PhysicsControl);


  //void SetWheelCollision(UWheeledVehicleMovementComponent4W *Vehicle4W, const FVehiclePhysicsControl &PhysicsControl);
  void SetWheelCollision(UChaosWheeledVehicleMovementComponent *Vehicle, const FVehiclePhysicsControl &PhysicsControl);


  void SetVehicleLightState(const FVehicleLightState &LightState);

  void SetVehicleSoundState(const FVehicleSoundState &SoundState);

  void SetVehicleEffectsState(const FVehicleEffectsState &EffectsState);

  //UFUNCTION(BlueprintNativeEvent)
  //bool IsTwoWheeledVehicle();
  //virtual bool IsTwoWheeledVehicle_Implementation() {
  //  return false;
  //}

  /// @}
  // ===========================================================================
  /// @name Vehicle input control
  // ===========================================================================
  /// @{
public:

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  void ApplyVehicleControl(const FVehicleControl &Control, EVehicleInputPriority Priority)
  {
    if (InputControl.Priority <= Priority)
    {
      InputControl.Control = Control;
      InputControl.Priority = Priority;
    }
  }

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  void ActivateVelocityControl(const FVector &Velocity);

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  void DeactivateVelocityControl();

  /// @todo This function should be private to AWheeledVehicleAIController.
  void FlushVehicleControl();

 
public:


  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  void HoldHandbrake()
  {
    //SetHandbrakeInput(true);
    GetVehicleMovementComponent()->SetHandbrakeInput(true);
  }

  UFUNCTION(Category = "SIRV Wheeled Vehicle", BlueprintCallable)
  void ReleaseHandbrake()
  {
    //SetHandbrakeInput(false);
    GetVehicleMovementComponent()->SetHandbrakeInput(false);
  }


 

  /// @}
  // ===========================================================================
  /// @name Overriden from AActor
  // ===========================================================================
  /// @{

protected:

  virtual void BeginPlay() override;

  UFUNCTION(BlueprintImplementableEvent)
  void RefreshLightState(const FVehicleLightState &VehicleLightState);

  UFUNCTION(BlueprintImplementableEvent)
  void RefreshSoundState(const FVehicleSoundState &VehicleSoundState);

  UFUNCTION(BlueprintImplementableEvent)
  void RefreshEffectsState(const FVehicleEffectsState &VehicleEffectsState);

  UFUNCTION(BlueprintCallable, CallInEditor)
  void AdjustVehicleBounds();
  
  UPROPERTY(Category = "Vehicle Control", VisibleAnywhere, BlueprintReadWrite)
  FVehicleControl LastAppliedControl;

private:

  
  /// Current state of the vehicle controller (for debugging purposes).
  UPROPERTY(Category = "AI Controller", VisibleAnywhere)
  ESirvWheeledVehicleState State = ESirvWheeledVehicleState::UNKNOWN;

  UPROPERTY(Category = "SIRV Wheeled Vehicle", EditAnywhere)
  UBoxComponent *VehicleBounds;

  UPROPERTY(Category = "SIRV Wheeled Vehicle", EditAnywhere)
  UVehicleVelocityControl* VelocityControl;

 
  UPROPERTY(Category = "SIRV Camera Vehicle", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  TObjectPtr<class USceneComponent> InternalCameraBase;
  //class USceneComponent* InternalCameraBase;

  UPROPERTY(Category = "SIRV Camera Vehicle", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  TObjectPtr<class UCameraComponent> InternalCamera;
  //UCameraComponent* InternalCamera;
  
  UPROPERTY(Category = "SIRV Camera Vehicle", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  TObjectPtr<class USirvScreenSceneCaptureComponent2D> InternalCameraLeft;
  //USirvScreenSceneCaptureComponent2D* InternalCameraLeft;

  UPROPERTY(Category = "SIRV Camera Vehicle", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  TObjectPtr<class USirvScreenSceneCaptureComponent2D> InternalCameraRight;
  //USirvScreenSceneCaptureComponent2D* InternalCameraRight;

  //UPROPERTY(Category = "SIRV Camera Vehicle", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  //FVector CameraPosition;

  UPROPERTY(Category = "SIRV Telemetry Vehicle", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  FVehicleTelemetry Telemetry;
  
  struct
  {
    EVehicleInputPriority Priority = EVehicleInputPriority::INVALID;
    FVehicleControl Control;
    FVehicleLightState LightState;
    FVehicleSoundState SoundState;
    FVehicleEffectsState EffectsState;
  }
  InputControl;

  

};
