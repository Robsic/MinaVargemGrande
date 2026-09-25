// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#include "EmptyActor.h"

AEmptyActor::AEmptyActor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = false;
  RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
  RootComponent = RootSceneComponent;
  RootComponent->SetMobility(EComponentMobility::Movable);
}
