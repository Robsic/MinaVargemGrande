// Copyright (c) 2023 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#include "Sensor/Sensor.h"
#include "Sirv.h"

#include "Actor/ActorDescription.h"
#include "Actor/ActorBlueprintFunctionLibrary.h"

ASensor::ASensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  auto *Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CamMesh"));
  Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
  Mesh->bHiddenInGame = true;
  Mesh->CastShadow = false;
  RootComponent = Mesh;
}

void ASensor::BeginPlay()
{
  Super::BeginPlay();
  OnPostTickDelegate = FWorldDelegates::OnWorldPostActorTick.AddUObject(
      this, &ASensor::PostPhysTick);
}

void ASensor::Set(const FActorDescription &Description)
{
  // set the tick interval of the sensor
  UE_LOG(LogSirv, Warning, TEXT("::::: ASensor : BEFORE SetActorTickInterval"));

  if (Description.Variations.Contains("sensor_tick"))
  {
    UE_LOG(LogSirv, Warning, TEXT("::::: ASensor : ANTES SetActorTickInterval"));
    SetActorTickInterval(
        UActorBlueprintFunctionLibrary::ActorAttributeToFloat(Description.Variations["sensor_tick"],
        0.0f));
        UE_LOG(LogSirv, Warning, TEXT("::::: ASensor : DEPOIS SetActorTickInterval"));
  }
  
  check(Ros != nullptr);
    
  topic_id = Ros->advertiseSensorTopic(Description);
  is_publishing = true;

  
  buf = sirv::Buffer();
}

void ASensor::Tick(const float DeltaTime)
{
  Super::Tick(DeltaTime);
  PrePhysTick(DeltaTime);
}

void ASensor::SetSeed(const int32 InSeed)
{
  check(RandomEngine != nullptr);
  Seed = InSeed;
  RandomEngine->Seed(InSeed);
}

void ASensor::PostActorCreated()
{
  Super::PostActorCreated();

#if WITH_EDITOR
  auto *StaticMeshComponent = Cast<UStaticMeshComponent>(RootComponent);
  if (StaticMeshComponent && !IsRunningCommandlet() && !StaticMeshComponent->GetStaticMesh())
  {
    UStaticMesh *CamMesh = LoadObject<UStaticMesh>(
        NULL,
        TEXT("/Engine/EditorMeshes/MatineeCam_SM.MatineeCam_SM"),
        NULL,
        LOAD_None,
        NULL);
    StaticMeshComponent->SetStaticMesh(CamMesh);
  }
#endif // WITH_EDITOR
}

void ASensor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);
  //Stream = FDataStream();

  FWorldDelegates::OnWorldPostActorTick.Remove(OnPostTickDelegate);
}
