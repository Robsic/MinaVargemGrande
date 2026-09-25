// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#include "Sirv/Game/SirvStatics.h"
#include "Sirv.h"

#include "Runtime/Core/Public/HAL/FileManagerGeneric.h"

TArray<FString> USirvStatics::GetAllMapNames()
{
  TArray<FString> TmpStrList, MapNameList;
  IFileManager::Get().FindFilesRecursive(
      MapNameList, *FPaths::ProjectContentDir(), TEXT("*.umap"), true, false, false);
  MapNameList.RemoveAll( [](const FString &Name) { return Name.Contains("TestMaps");});
  MapNameList.RemoveAll( [](const FString &Name) { return Name.Contains("OpenDriveMap");});
  for (int i = 0; i < MapNameList.Num(); i++) {
      MapNameList[i].ParseIntoArray(TmpStrList, TEXT("Content/"), true);
      MapNameList[i] = TmpStrList[1];
      MapNameList[i] = MapNameList[i].Replace(TEXT(".umap"), TEXT(""));
      MapNameList[i] = "/Game/" + MapNameList[i];
  }
  return MapNameList;
}
