// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once


#include "Util/NonCopyable.h"
#include "Logging/LogMacros.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSirv, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSirvServer, Log, All);

// DisplayName, GroupName, Third param is always Advanced.
// DECLARE_STATS_GROUP(TEXT("Sirv"), STATGROUP_Sirv, STATCAT_Advanced);
DECLARE_STATS_GROUP(TEXT("SirvSensor"), STATGROUP_SirvSensor, STATCAT_Advanced);

//DECLARE_MEMORY_STAT(TEXT("SIRVMEMORY"), STATGROUP_SIRVMEMORY, STATCAT_Advanced)

DECLARE_CYCLE_STAT(TEXT("Read RT"),     STAT_SirvSensorReadRT,     STATGROUP_SirvSensor);
DECLARE_CYCLE_STAT(TEXT("Copy Text"),   STAT_SirvSensorCopyText,   STATGROUP_SirvSensor);
DECLARE_CYCLE_STAT(TEXT("Buffer Copy"), STAT_SirvSensorBufferCopy, STATGROUP_SirvSensor);
DECLARE_CYCLE_STAT(TEXT("Stream Send"), STAT_SirvSensorStreamSend, STATGROUP_SirvSensor);

// Options to compile with extra debug log.
#if WITH_EDITOR
// #define SIRV_AI_VEHICLES_EXTRA_LOG
// #define SIRV_AI_WALKERS_EXTRA_LOG
// #define SIRV_ROAD_GENERATOR_EXTRA_LOG
// #define SIRV_SERVER_EXTRA_LOG
// #define SIRV_TAGGER_EXTRA_LOG
// #define SIRV_WEATHER_EXTRA_LOG
#endif // WITH_EDITOR

class FSirvModule : public IModuleInterface
{
	void RegisterSettings();
	void UnregisterSettings();
	bool HandleSettingsSaved();

	void LoadSirvDependency();

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void* SirvRosNodeDynamicLibHandle;
	void* SirvDevDynamicLibHandle;

};
