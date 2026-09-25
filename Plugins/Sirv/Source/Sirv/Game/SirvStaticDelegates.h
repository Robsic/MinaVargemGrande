// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

#include "Settings/EpisodeSettings.h"

class SIRV_API FSirvStaticDelegates
{
public:

  DECLARE_MULTICAST_DELEGATE_OneParam(FOnEpisodeSettingsChange, const FEpisodeSettings &);
  static FOnEpisodeSettingsChange OnEpisodeSettingsChange;
};
