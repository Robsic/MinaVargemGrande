// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.




#include "Sirv/Weather/Weather.h"
#include "Sirv.h"

AWeather::AWeather(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = false;
  RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));
}

void AWeather::ApplyWeather(const FWeatherParameters &InWeather)
{
  SetWeather(InWeather);

#ifdef SIRV_WEATHER_EXTRA_LOG
  UE_LOG(LogSirv, Log, TEXT("Changing weather:"));
  UE_LOG(LogSirv, Log, TEXT("  - Cloudiness = %.2f"), Weather.Cloudiness);
  UE_LOG(LogSirv, Log, TEXT("  - Precipitation = %.2f"), Weather.Precipitation);
  UE_LOG(LogSirv, Log, TEXT("  - PrecipitationDeposits = %.2f"), Weather.PrecipitationDeposits);
  UE_LOG(LogSirv, Log, TEXT("  - WindIntensity = %.2f"), Weather.WindIntensity);
  UE_LOG(LogSirv, Log, TEXT("  - SunAzimuthAngle = %.2f"), Weather.SunAzimuthAngle);
  UE_LOG(LogSirv, Log, TEXT("  - SunAltitudeAngle = %.2f"), Weather.SunAltitudeAngle);
  UE_LOG(LogSirv, Log, TEXT("  - FogDensity = %.2f"), Weather.FogDensity);
  UE_LOG(LogSirv, Log, TEXT("  - FogDistance = %.2f"), Weather.FogDistance);
  UE_LOG(LogSirv, Log, TEXT("  - FogFalloff = %.2f"), Weather.FogFalloff);
  UE_LOG(LogSirv, Log, TEXT("  - Wetness = %.2f"), Weather.Wetness);
#endif // SIRV_WEATHER_EXTRA_LOG

  // Call the blueprint that actually changes the weather.
  RefreshWeather(Weather);
}

void AWeather::NotifyWeather()
{
  // Call the blueprint that actually changes the weather.
  RefreshWeather(Weather);
}

void AWeather::SetWeather(const FWeatherParameters &InWeather)
{
  Weather = InWeather;
}
