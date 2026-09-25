// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

// Workaround to fix Windows conflict: Windows changes the name of some functions (DrawText, LoadLibrary...)
// with Unicode / ANSI versions for his own API (DrawTextW, DrawTextA, LoadLibraryW, LoadLibraryA...).
// But the changes are global for the compiler. Deep in headers, Windows has something like:
// #ifdef UNICODE
//   #define DrawText  DrawTextW
//   #define LoadLibrary LoadLibraryW
// #else
//   #define DrawText  DrawTextA
//   #define LoadLibrary LoadLibraryA
// #endif
// Then the linker tries to find the function DrawTextW on an external DLL and an unresolved external error happens because
// Unreal has no function DrawTextW, it has just DrawText.
// We can fix that by just undefining the function that conflicts with the name of the Windows API in Unicode.
#undef DrawText

#include "Containers/Array.h"
#include "GameFramework/HUD.h"

#include "SirvHUD.generated.h"

struct HUDString
{
  FString Str { "" };
  FVector Location;
  FColor Color;
  double TimeToDie;
};

/// Class to draw on HUD
UCLASS()
class SIRV_API ASirvHUD : public AHUD
{
  GENERATED_BODY()

public:

  ASirvHUD(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
  {
    PrimaryActorTick.bCanEverTick = false;
  }

  virtual void DrawHUD() override;

  void AddHUDString(const FString Str, const FVector Location, const FColor Color, double LifeTime);

private:
  TArray<HUDString> StringList;
};
