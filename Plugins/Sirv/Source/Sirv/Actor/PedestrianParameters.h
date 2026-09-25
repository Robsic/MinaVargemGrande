// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "GameFramework/Character.h"

#include "PedestrianParameters.generated.h"

UENUM(BlueprintType)
enum class EPedestrianGender : uint8
{
  Other       UMETA(DisplayName = "Other"),
  Female      UMETA(DisplayName = "Female"),
  Male        UMETA(DisplayName = "Male"),

  SIZE        UMETA(Hidden),
  INVALID     UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EPedestrianAge : uint8
{
  Child       UMETA(DisplayName = "Child"),
  Teenager    UMETA(DisplayName = "Teenager"),
  Adult       UMETA(DisplayName = "Adult"),
  Elderly     UMETA(DisplayName = "Elderly"),

  SIZE        UMETA(Hidden),
  INVALID     UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct SIRV_API FPedestrianParameters
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Id;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<ACharacter> Class;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EPedestrianGender Gender = EPedestrianGender::Other;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EPedestrianAge Age = EPedestrianAge::Adult;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<float> Speed;
};
