// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#include "Vehicle/SirvWheeledVehicle.h"
#include "Sirv.h"

#include "Game/SirvStatics.h"

#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"
//#include "PhysXPublic.h"
//#include "PhysXVehicleManager.h"

//#include "TireConfig.h"

//#include "VehicleWheel.h"
#include "ChaosVehicleWheel.h"

#include "Util/ActorAttacher.h"
#include "Util/EmptyActor.h"

#include "Rendering/SkeletalMeshRenderData.h"

//------------------------------------------------------------------------------

FVector ASirvWheeledVehicle::GetVehicleAngularVelocity() const{
const auto RootComponent_ = Cast<UPrimitiveComponent>(GetRootComponent());
  return RootComponent_ != nullptr ?
          RootComponent_->GetPhysicsAngularVelocityInDegrees() :
          FVector{0.0f, 0.0f, 0.0f};
}

//FVector ASirvWheeledVehicle::GetAcceleration(
//    const FActorView &View,
//    const FVector &Velocity,
//    const float DeltaSeconds)
//{
//  FVector &PreviousVelocity = View.GetActorInfo()->Velocity;
//  const FVector Acceleration = (Velocity - PreviousVelocity) / DeltaSeconds;
//  PreviousVelocity = Velocity;
//  return {Acceleration.X, Acceleration.Y, Acceleration.Z};
//}



//------------------------------------------------------------------------------
static auto CAMERA_LAUNCH = 0u;
// =============================================================================
// -- Constructor and destructor -----------------------------------------------
// =============================================================================

ASirvWheeledVehicle::ASirvWheeledVehicle(const FObjectInitializer& ObjectInitializer) :
  Super(ObjectInitializer)
{
  VehicleBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("VehicleBounds"));
  VehicleBounds->SetupAttachment(RootComponent);
  VehicleBounds->SetHiddenInGame(true);
  VehicleBounds->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

  VelocityControl = CreateDefaultSubobject<UVehicleVelocityControl>(TEXT("VelocityControl"));
  VelocityControl->Deactivate();

  #ifdef WITH_CARSIM
  // ExternalMovementComponent = CreateDefaultSubobject<UCarSimMovementComponent>(TEXT("CarSimMovement"));
  // CarSimMovementComponent = Cast<UCarSimMovementComponent>(ExternalMovementComponent);
  // CarSimMovementComponent->DisableVehicle = true;
  #endif
 

  // InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("CAMERA_BASE_%d"),CAMERA_LAUNCH);
  // //InternalCameraBase->SetRelativeLocation(CameraPosition);
  // InternalCameraBase->SetupAttachment(GetMesh());
       
  // InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA_%d"),CAMERA_LAUNCH);
  // InternalCamera->bUsePawnControlRotation = false;
  // //InternalCamera->FieldOfView = 90.0f;
  // InternalCamera->SetupAttachment(InternalCameraBase);

  // CAMERA_LAUNCH++;

  GetVehicleMovementComponent()->bReverseAsBrake = false;
}

ASirvWheeledVehicle::~ASirvWheeledVehicle() {}

void ASirvWheeledVehicle::SetWheelCollision(UChaosWheeledVehicleMovementComponent *Vehicle,
    const FVehiclePhysicsControl &PhysicsControl ) {

  #ifdef WHEEL_SWEEP_ENABLED
    
    const bool IsEqual = Vehicle->UseSweepWheelCollision == PhysicsControl.UseSweepWheelCollision;

    if (IsEqual)
      return;

    Vehicle->UseSweepWheelCollision = PhysicsControl.UseSweepWheelCollision;

  #else

    if (PhysicsControl.UseSweepWheelCollision)
      UE_LOG(LogSirv, Warning, TEXT("Error: Sweep for wheel collision is not available. \
      Make sure you have installed the required patch.") );

  #endif

}

void ASirvWheeledVehicle::BeginPlay()
{
  Super::BeginPlay();


  //InternalCameraBase->SetRelativeLocation(CameraPosition);
  
  float FrictionScale = 3.5f;

  UChaosWheeledVehicleMovementComponent *Vehicle = Cast<UChaosWheeledVehicleMovementComponent>(
      GetVehicleMovement());
  check(Vehicle != nullptr);

  // Setup Tire Configs with default value. This is needed to avoid getting
  // friction values of previously created TireConfigs for the same vehicle
  // blueprint.
  TArray<float> OriginalFrictions;
  OriginalFrictions.Init(FrictionScale, Vehicle->Wheels.Num());
  //SetWheelsFrictionScale(OriginalFrictions);

  /* // Check if it overlaps with a Friction trigger, if so, update the friction
  // scale.
  TArray<AActor *> OverlapActors;
  GetOverlappingActors(OverlapActors, AFrictionTrigger::StaticClass());
  for (const auto &Actor : OverlapActors)
  {
    AFrictionTrigger *FrictionTrigger = Cast<AFrictionTrigger>(Actor);
    if (FrictionTrigger)
    {
      FrictionScale = FrictionTrigger->Friction;
    }
  } */
/* 
  // Set the friction scale to Wheel CDO and update wheel setups
  TArray<FWheelSetup> NewWheelSetups = Vehicle->WheelSetups;

  for (const auto &WheelSetup : NewWheelSetups)
  {
    UChaosVehicleWheel *Wheel = WheelSetup.WheelClass.GetDefaultObject();
    check(Wheel != nullptr);

    // Assigning new tire config
    //Wheel->TireConfig = NewObjectNewObject<UTireConfig>();

    // Setting a new value to friction
    Wheel->TireConfig->SetFrictionScale(FrictionScale);
  }

  Vehicle->WheelSetups = NewWheelSetups;
 */
}

void ASirvWheeledVehicle::AdjustVehicleBounds()
{
  FBoundingBox BoundingBox = UBoundingBoxCalculator::GetVehicleBoundingBox(this);

  const FTransform& CompToWorldTransform = RootComponent->GetComponentTransform();
  const FRotator Rotation = CompToWorldTransform.GetRotation().Rotator();
  const FVector Translation = CompToWorldTransform.GetLocation();
  const FVector Scale = CompToWorldTransform.GetScale3D();

  // Invert BB origin to local space
  BoundingBox.Origin -= Translation;
  BoundingBox.Origin = Rotation.UnrotateVector(BoundingBox.Origin);
  BoundingBox.Origin /= Scale;

  // Prepare Box Collisions
  FTransform Transform;
  Transform.SetTranslation(BoundingBox.Origin);
  VehicleBounds->SetRelativeTransform(Transform);
  VehicleBounds->SetBoxExtent(BoundingBox.Extent);
}

// =============================================================================
// -- Get functions ------------------------------------------------------------
// =============================================================================

float ASirvWheeledVehicle::GetVehicleForwardSpeed() const
{
  #ifdef WITH_CARSIM
  if (bCarSimEnabled)
  {
    return CarSimMovementComponent->GetForwardSpeed();
  }
  #endif
  return GetVehicleMovementComponent()->GetForwardSpeed() * 0.036; // Convert to centimeters/sec to kilometers/hour
}

FVector ASirvWheeledVehicle::GetVehicleOrientation() const
{
  return GetVehicleTransform().GetRotation().GetForwardVector();
}

int32 ASirvWheeledVehicle::GetVehicleCurrentGear() const
{
  #ifdef WITH_CARSIM
  if (bCarSimEnabled)
  {
    return CarSimMovementComponent->GetCurrentGear();
  }
  #endif
  return GetVehicleMovementComponent()->GetCurrentGear();
}

FTransform ASirvWheeledVehicle::GetVehicleBoundingBoxTransform() const
{
  return VehicleBounds->GetRelativeTransform();
}

FVector ASirvWheeledVehicle::GetVehicleBoundingBoxExtent() const
{
  return VehicleBounds->GetScaledBoxExtent();
}

float ASirvWheeledVehicle::GetMaximumSteerAngle() const
{
  UChaosWheeledVehicleMovementComponent *Vehicle = Cast<UChaosWheeledVehicleMovementComponent>(
      GetVehicleMovement());
  check(Vehicle != nullptr);
  //const auto &Wheels = GetVehicleMovementComponent()->Wheels;
  //const auto &Wheels = GetVehicleMovementComponent()->WheelSetups[0];
  
  //FName teste = Vehicle->GetClass()->GetFName();
  //UE_LOG(LogSirv, Warning, TEXT("Vehicle->Wheels.Num(): %d class:%s "),Vehicle->Wheels.Num() , *teste.ToString() );

  check(Vehicle->Wheels.Num() > 0);
  const auto *FrontWheel = Vehicle->Wheels[0];
  check(FrontWheel != nullptr);
  return FrontWheel->MaxSteerAngle;
  
}

// =============================================================================
// -- Set functions ------------------------------------------------------------
// =============================================================================

void ASirvWheeledVehicle::FlushVehicleControl()
{

    //auto *MovementComponent = GetVehicleMovementComponent();
    UChaosWheeledVehicleMovementComponent *MovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(
      GetVehicleMovement());
    check(MovementComponent != nullptr);
    
    MovementComponent->SetThrottleInput(InputControl.Control.Throttle);
    MovementComponent->SetSteeringInput(InputControl.Control.Steer);
    MovementComponent->SetBrakeInput(InputControl.Control.Brake);
    MovementComponent->SetHandbrakeInput(InputControl.Control.bHandBrake);

    MovementComponent->SetTargetGear(InputControl.Control.Gear,true);

    Telemetry.gear = this->GetVehicleCurrentGear();
    Telemetry.speed = this->GetVehicleForwardSpeed(); // Speed in kilometers/hour
    Telemetry.enginerpm = MovementComponent->GetEngineRotationSpeed();
    Telemetry.torque = MovementComponent->EngineSetup.GetTorqueFromRPM(Telemetry.enginerpm);
    Telemetry.rollingresistance = 0.01; // @todo not implemented yet
    
    constexpr float TO_METERS = 1e-2;
    Telemetry.position        = TO_METERS * this->GetActorLocation();
    Telemetry.orientation     = this->GetVehicleTransform().GetRotation().Euler();
    Telemetry.angularvelocity = this->GetVehicleAngularVelocity();

    
    //Telemetry.acceleration = this->GetAcceleration(View, Velocity, DeltaSeconds);

    // Light impact...
    //InputControl.LightState.Brake = (InputControl.Control.Brake >= 0.1 || InputControl.Control.bHandBrake) ? true:false;  
    //InputControl.LightState.Reverse = (InputControl.Control.Gear < 0) ? true:false;  
    
    //RefreshLightState(InputControl.LightState);

    //UE_LOG(LogSirv, Log, TEXT("FlushVehicleControl() EXECUTING..."));

    /* 
    if (LastAppliedControl.bReverse != InputControl.Control.bReverse)
    {
      MovementComponent->SetUseAutoGears(!InputControl.Control.bReverse);
      MovementComponent->SetTargetGear(InputControl.Control.bReverse ? -1 : 1, true);
    }
    else
    {
      MovementComponent->SetUseAutoGears(!InputControl.Control.bManualGearShift);
      if (InputControl.Control.bManualGearShift)
      {
        MovementComponent->SetTargetGear(InputControl.Control.Gear, true);
      }
    }
    InputControl.Control.Gear = MovementComponent->GetCurrentGear();
   */
  
  //InputControl.Control.bReverse = InputControl.Control.Gear < 0;
  
  LastAppliedControl = InputControl.Control;
  InputControl.Priority = EVehicleInputPriority::INVALID;
}
/* 
TArray<float> ASirvWheeledVehicle::GetWheelsFrictionScale()
{
  UChaosWheeledVehicleMovementComponent *Vehicle = Cast<UChaosWheeledVehicleMovementComponent>(
      GetVehicleMovement());
  check(Vehicle != nullptr);

  TArray<float> WheelsFrictionScale;
  for (auto &Wheel : Vehicle->Wheels)
  {
    WheelsFrictionScale.Add(Wheel->TireConfig->GetFrictionScale());
  }
  return WheelsFrictionScale;
}
 */
/* 
void ASirvWheeledVehicle::SetWheelsFrictionScale(TArray<float> &WheelsFrictionScale)
{
  UChaosWheeledVehicleMovementComponent *Vehicle = Cast<UChaosWheeledVehicleMovementComponent>(
      GetVehicleMovement());
  check(Vehicle != nullptr);
  check(Vehicle->Wheels.Num() == WheelsFrictionScale.Num());

  for (int32 i = 0; i < Vehicle->Wheels.Num(); ++i)
  {
    Vehicle->Wheels[i]->TireConfig->SetFrictionScale(WheelsFrictionScale[i]);
  }
}
 */
FVehiclePhysicsControl ASirvWheeledVehicle::GetVehiclePhysicsControl() const
{
  UChaosWheeledVehicleMovementComponent *Vehicle = Cast<UChaosWheeledVehicleMovementComponent>(
      GetVehicleMovement());
  check(Vehicle != nullptr);

  FVehiclePhysicsControl PhysicsControl;
/* 
  // Engine Setup
  PhysicsControl.TorqueCurve = Vehicle->EngineSetup.TorqueCurve.EditorCurveData;
  PhysicsControl.MaxRPM = Vehicle->EngineSetup.MaxRPM;
  PhysicsControl.MOI = Vehicle->EngineSetup.MOI;
  PhysicsControl.DampingRateFullThrottle = Vehicle->EngineSetup.DampingRateFullThrottle;
  PhysicsControl.DampingRateZeroThrottleClutchEngaged =
      Vehicle->EngineSetup.DampingRateZeroThrottleClutchEngaged;
  PhysicsControl.DampingRateZeroThrottleClutchDisengaged =
      Vehicle->EngineSetup.DampingRateZeroThrottleClutchDisengaged;

  // Transmission Setup
  PhysicsControl.bUseGearAutoBox = Vehicle->TransmissionSetup.bUseGearAutoBox;
  PhysicsControl.GearSwitchTime = Vehicle->TransmissionSetup.GearSwitchTime;
  PhysicsControl.ClutchStrength = Vehicle->TransmissionSetup.ClutchStrength;
  PhysicsControl.FinalRatio = Vehicle->TransmissionSetup.FinalRatio;

  TArray<FGearPhysicsControl> ForwardGears;

  for (const auto &Gear : Vehicle->TransmissionSetup.ForwardGears)
  {
    FGearPhysicsControl GearPhysicsControl;

    GearPhysicsControl.Ratio = Gear.Ratio;
    GearPhysicsControl.UpRatio = Gear.UpRatio;
    GearPhysicsControl.DownRatio = Gear.DownRatio;

    ForwardGears.Add(GearPhysicsControl);
  }

  PhysicsControl.ForwardGears = ForwardGears;

  // Vehicle Setup
  PhysicsControl.Mass = Vehicle->Mass;
  PhysicsControl.DragCoefficient = Vehicle->DragCoefficient;

  // Center of mass offset (Center of mass is always zero vector in local
  // position)
  UPrimitiveComponent *UpdatedPrimitive = Cast<UPrimitiveComponent>(Vehicle->UpdatedComponent);
  check(UpdatedPrimitive != nullptr);

  PhysicsControl.CenterOfMass = UpdatedPrimitive->BodyInstance.COMNudge;

  // Transmission Setup
  PhysicsControl.SteeringCurve = Vehicle->SteeringCurve.EditorCurveData;

  // Wheels Setup
  TArray<FWheelPhysicsControl> Wheels;

  for (int32 i = 0; i < Vehicle->WheelSetups.Num(); ++i)
  {
    FWheelPhysicsControl PhysicsWheel;

    PxVehicleWheelData PWheelData = Vehicle->PVehicle->mWheelsSimData.getWheelData(i);

    PhysicsWheel.TireFriction = Vehicle->Wheels[i]->TireConfig->GetFrictionScale();
    PhysicsWheel.DampingRate = Cm2ToM2(PWheelData.mDampingRate);
    PhysicsWheel.MaxSteerAngle = FMath::RadiansToDegrees(PWheelData.mMaxSteer);
    PhysicsWheel.Radius = PWheelData.mRadius;
    PhysicsWheel.MaxBrakeTorque = Cm2ToM2(PWheelData.mMaxBrakeTorque);
    PhysicsWheel.MaxHandBrakeTorque = Cm2ToM2(PWheelData.mMaxHandBrakeTorque);

    PhysicsWheel.Position = Vehicle->Wheels[i]->Location;

    Wheels.Add(PhysicsWheel);
  }

  PhysicsControl.Wheels = Wheels;
 */
  return PhysicsControl;
}

FVehicleLightState ASirvWheeledVehicle::GetVehicleLightState() const
{
  return InputControl.LightState;
}

FVehicleSoundState ASirvWheeledVehicle::GetVehicleSoundState() const
{
  return InputControl.SoundState;
}

FVehicleEffectsState ASirvWheeledVehicle::GetVehicleEffectsState() const
{
  return InputControl.EffectsState;
}

FVehicleTelemetry ASirvWheeledVehicle::GetVehicleTelemetry() const
{
  return Telemetry;
}

void ASirvWheeledVehicle::ApplyVehiclePhysicsControl(const FVehiclePhysicsControl &PhysicsControl)
{
  UChaosWheeledVehicleMovementComponent *Vehicle = Cast<UChaosWheeledVehicleMovementComponent>(
      GetVehicleMovement());
  check(Vehicle != nullptr);
 
  // Engine Setup
  //Vehicle->EngineSetup.TorqueCurve.EditorCurveData = PhysicsControl.TorqueCurve;
  //Vehicle->EngineSetup.MaxRPM = PhysicsControl.MaxRPM;

  //Vehicle->EngineSetup.EngineRevUpMOI = PhysicsControl.EngineRevUpMOI;

  //Vehicle->EngineSetup.DampingRateFullThrottle = PhysicsControl.DampingRateFullThrottle;
  //Vehicle->EngineSetup.DampingRateZeroThrottleClutchEngaged =
  //    PhysicsControl.DampingRateZeroThrottleClutchEngaged;
  //Vehicle->EngineSetup.DampingRateZeroThrottleClutchDisengaged =
  //    PhysicsControl.DampingRateZeroThrottleClutchDisengaged;

  // Transmission Setup
  //Vehicle->TransmissionSetup.bUseAutomaticGears = PhysicsControl.bUseAutomaticGears;
  //Vehicle->TransmissionSetup.GearChangeTime = PhysicsControl.GearChangeTime;
  //Vehicle->TransmissionSetup.ClutchStrength = PhysicsControl.ClutchStrength;
  //Vehicle->TransmissionSetup.FinalRatio = PhysicsControl.FinalRatio;

  //TArray<FVehicleGearData> ForwardGears;

  // for (const auto &Gear : PhysicsControl.ForwardGears)
  // {
  //   FVehicleGearData GearData;

  //   GearData.Ratio = Gear.Ratio;
  //   GearData.UpRatio = Gear.UpRatio;
  //   GearData.DownRatio = Gear.DownRatio;

  //   ForwardGears.Add(GearData);
  // }

  // Vehicle->TransmissionSetup.ForwardGears = ForwardGears;



  // Vehicle Setup
  Vehicle->Mass = PhysicsControl.Mass;
  Vehicle->DragCoefficient = PhysicsControl.DragCoefficient;
  
  UE_LOG(LogSirv, Log, TEXT("Vehicle Mass changed to %f "), Vehicle->Mass);

  // Center of mass
  UPrimitiveComponent *UpdatedPrimitive = Cast<UPrimitiveComponent>(Vehicle->UpdatedComponent);
  check(UpdatedPrimitive != nullptr);

  //UpdatedPrimitive->BodyInstance.COMNudge = PhysicsControl.CenterOfMass;

  // // Transmission Setup
  //Vehicle->SteeringSetup.SteeringCurve.EditorCurveData = PhysicsControl.SteeringCurve;
           
  // Wheels Setup
  //const int PhysicsWheelsNum = PhysicsControl.Wheels.Num();
  // if (PhysicsWheelsNum != 4)
  // {
  //   UE_LOG(LogSirv, Error, TEXT("Number of WheelPhysicsControl is not 4."));
  //   return;
  // }

  // // Change, if required, the collision mode for wheels
  // SetWheelCollision(Vehicle, PhysicsControl);

  // for (int32 i = 0; i < PhysicsWheelsNum; ++i)
  // {
  //   PxVehicleWheelData PWheelData = Vehicle->PVehicle->mWheelsSimData.getWheelData(i);

  //   PWheelData.mRadius = PhysicsControl.Wheels[i].Radius;
  //   PWheelData.mMaxSteer = FMath::DegreesToRadians(PhysicsControl.Wheels[i].MaxSteerAngle);
  //   PWheelData.mDampingRate = M2ToCm2(PhysicsControl.Wheels[i].DampingRate);
  //   PWheelData.mMaxBrakeTorque = M2ToCm2(PhysicsControl.Wheels[i].MaxBrakeTorque);
  //   PWheelData.mMaxHandBrakeTorque = M2ToCm2(PhysicsControl.Wheels[i].MaxHandBrakeTorque);

  //   Vehicle->PVehicle->mWheelsSimData.setWheelData(i, PWheelData);
  //   Vehicle->Wheels[i]->TireConfig->SetFrictionScale(PhysicsControl.Wheels[i].TireFriction);
  // }
 

  // // Recreate Physics State for vehicle setup
  // GetWorld()->GetPhysicsScene()->GetPxScene()->lockWrite();
   Vehicle->RecreatePhysicsState();
  // GetWorld()->GetPhysicsScene()->GetPxScene()->unlockWrite();
 
  // auto * Recorder = USirvStatics::GetRecorder(GetWorld());
  // if (Recorder && Recorder->IsEnabled())
  // {
  //   Recorder->AddPhysicsControl(*this);
  // }
}

void ASirvWheeledVehicle::ActivateVelocityControl(const FVector &Velocity)
{
  VelocityControl->Activate(Velocity);
}

void ASirvWheeledVehicle::DeactivateVelocityControl()
{
  VelocityControl->Deactivate();
}

void ASirvWheeledVehicle::SetVehicleLightState(const FVehicleLightState &LightState)
{
  InputControl.LightState = LightState;
  RefreshLightState(LightState);
}


void ASirvWheeledVehicle::SetVehicleSoundState(const FVehicleSoundState &SoundState)
{
  InputControl.SoundState = SoundState;
  RefreshSoundState(SoundState);
}

void ASirvWheeledVehicle::SetVehicleEffectsState(const FVehicleEffectsState &EffectsState)
{
  InputControl.EffectsState = EffectsState;
  RefreshEffectsState(EffectsState);
}

//-------------------------------------------
