// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#include "SirvHUD.h"

void ASirvHUD::DrawHUD()
{
  Super::DrawHUD();

  auto Player = GetOwningPlayerController();
  if (Player == nullptr)
  {
    UE_LOG(LogSirv, Error, TEXT("Can't find player controller!"));
    return;
  }

  double Now = FPlatformTime::Seconds();
  int i = 0;
  while (i < StringList.Num())
  {
    // project position from camera
    FVector2D Screen;
    if (Player->ProjectWorldLocationToScreen(StringList[i].Location, Screen, true))
    {
      // draw text
      DrawText(StringList[i].Str, StringList[i].Color, Screen.X, Screen.Y);
    }

    // check to remove the string
    if (Now >= StringList[i].TimeToDie)
    {
      StringList.RemoveAt(i);
    }
    else
      ++i;
  }
}

void ASirvHUD::AddHUDString(const FString Str, const FVector Location, const FColor Color, double LifeTime)
{
  double Now = FPlatformTime::Seconds();
  HUDString Obj { Str, Location, Color, Now + LifeTime };
  StringList.Add(std::move(Obj));
}
