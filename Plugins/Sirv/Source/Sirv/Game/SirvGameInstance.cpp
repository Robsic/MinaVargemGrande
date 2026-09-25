// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#include "Game/SirvGameInstance.h"
#include "Sirv.h"

#include "Settings/SirvSettings.h"

USirvGameInstance::USirvGameInstance() {
  SirvSettings = CreateDefaultSubobject<USirvSettings>(TEXT("SirvSettings"));
  //Recorder = CreateDefaultSubobject<ASirvRecorder>(TEXT("Recorder"));
  //SirvEngine.SetRecorder(Recorder);

  SirvEngine = CreateDefaultSubobject<USirvEngine>(TEXT("SirvEngine"));
  
  check(SirvSettings != nullptr);
  SirvSettings->LoadSettings();
  SirvSettings->LogSettings();
}

USirvGameInstance::~USirvGameInstance() = default;
