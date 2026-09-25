// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "GameFramework/Pawn.h"
#include "EmptyActor.generated.h"

UCLASS()
class SIRV_API AEmptyActor : public APawn
{
  GENERATED_BODY()

public:
  AEmptyActor(const FObjectInitializer &ObjectInitializer);

private:
  UPROPERTY(Category="Empty Actor", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  USceneComponent* RootSceneComponent;
};
