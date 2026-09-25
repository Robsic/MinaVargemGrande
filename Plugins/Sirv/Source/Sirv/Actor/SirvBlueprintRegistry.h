// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Actor/PropParameters.h"
#include "ActorBlueprintFunctionLibrary.h"

#include "SirvBlueprintRegistry.generated.h"

UCLASS()
class USirvBlueprintRegistry : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:

  UFUNCTION(Category = "Sirv Blueprint Registry", BlueprintCallable)
  static void AddToSirvBlueprintRegistry(const TArray<FPropParameters> &PropParametersArray);

  UFUNCTION(Category = "Sirv Blueprint Registry", BlueprintCallable)
  static void LoadPropDefinitions(TArray<FPropParameters> &PropParametersArray);

};
