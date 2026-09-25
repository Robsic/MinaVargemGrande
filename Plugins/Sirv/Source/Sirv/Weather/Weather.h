// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "GameFramework/Actor.h"

#include "Sirv/Weather/WeatherParameters.h"

#include "Weather.generated.h"

UCLASS(Abstract)
class SIRV_API AWeather : public AActor
{
  GENERATED_BODY()

public:

  AWeather(const FObjectInitializer& ObjectInitializer);

  /// Update the weather parameters and notifies it to the blueprint's event
  UFUNCTION(BlueprintCallable)
  void ApplyWeather(const FWeatherParameters &WeatherParameters);

  /// Notifing the weather to the blueprint's event
  void NotifyWeather();

  /// Update the weather parameters without notifing it to the blueprint's event
  UFUNCTION(BlueprintCallable)
  void SetWeather(const FWeatherParameters &WeatherParameters);

  /// Returns the current WeatherParameters
  UFUNCTION(BlueprintCallable)
  const FWeatherParameters &GetCurrentWeather() const
  {
    return Weather;
  }

protected:

  UFUNCTION(BlueprintImplementableEvent)
  void RefreshWeather(const FWeatherParameters &WeatherParameters);

private:

  UPROPERTY(VisibleAnywhere)
  FWeatherParameters Weather;
};
