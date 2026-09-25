// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#include "Sirv.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
#include "Settings/SirvSettings.h"

#define LOCTEXT_NAMESPACE "FSirvModule"

DEFINE_LOG_CATEGORY(LogSirv);
DEFINE_LOG_CATEGORY(LogSirvServer);

void FSirvModule::StartupModule()
{
	RegisterSettings();

	LoadSirvDependency();
}

void FSirvModule::LoadSirvDependency()
{
	
	const FString BaseDir = FPaths::Combine(*FPaths::ProjectPluginsDir(), TEXT("Sirv"));
	const FString soDir = FPaths::Combine(*BaseDir, TEXT("SirvDependencies"), TEXT("lib"));
	FString SirvRosNodeSO = FPaths::Combine(*soDir, TEXT("libsirv_ros_node.so"));
	FString SirvRosDevSO = FPaths::Combine(*soDir, TEXT("libsirv_dev.so"));
	
	UE_LOG(LogSirv, Log, TEXT("Loading Dlls from: %s"), *soDir);
	SirvRosNodeDynamicLibHandle = FPlatformProcess::GetDllHandle(*SirvRosNodeSO);
	if (!SirvRosNodeDynamicLibHandle)
	{
		UE_LOG(LogSirv, Warning, TEXT("Error: libsirv_ros_node.so could not be loaded"));
	}
	SirvDevDynamicLibHandle = FPlatformProcess::GetDllHandle(*SirvRosDevSO);
	if (!SirvDevDynamicLibHandle)
	{
		UE_LOG(LogSirv, Warning, TEXT("Error: libsirv_dev.so could not be loaded"));
	}
	

}

void FSirvModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		UnregisterSettings();
	}

	if(SirvRosNodeDynamicLibHandle)
	{
		FPlatformProcess::FreeDllHandle(SirvRosNodeDynamicLibHandle);
		SirvRosNodeDynamicLibHandle = nullptr;

	}

	if(SirvDevDynamicLibHandle)
	{
		FPlatformProcess::FreeDllHandle(SirvDevDynamicLibHandle);
		SirvDevDynamicLibHandle = nullptr;

	}

}

void FSirvModule::RegisterSettings()
{
	// Registering some settings is just a matter of exposing the default UObject of
	// your desired class, add here all those settings you want to expose
	// to your LDs or artists.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Create the new category
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

		SettingsContainer->DescribeCategory("SIRVSettings",
			LOCTEXT("RuntimeWDCategoryName", "SIRV Settings"),
			LOCTEXT("RuntimeWDCategoryDescription", "SIRV plugin settings"));

		// Register the settings
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "SIRVSettings", "General",
			LOCTEXT("RuntimeGeneralSettingsName", "General"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "General configuration for the SIRV plugin"),
			GetMutableDefault<USirvSettings>()
		);

		// Register the save handler to your settings, you might want to use it to
		// validate those or just act to settings changes.
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FSirvModule::HandleSettingsSaved);
		}
	}
}

void FSirvModule::UnregisterSettings()
{
	// Ensure to unregister all of your registered settings here, hot-reload would
	// otherwise yield unexpected results.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "CustomSettings", "General");
	}
}

bool FSirvModule::HandleSettingsSaved()
{
	USirvSettings* Settings = GetMutableDefault<USirvSettings>();
	bool ResaveSettings = false;

	// Put any validation code in here and resave the settings in case an invalid
	// value has been entered

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSirvModule, Sirv)

// =============================================================================
// -- Implement sirv throw_exception ------------------------------------------
// =============================================================================

#include <compiler/disable-ue4-macros.h>
#include <sirv/Exception.h>
#include <compiler/enable-ue4-macros.h>

#include <exception>

namespace sirv {

  void throw_exception(const std::exception &e) {
    UE_LOG(LogSirv, Fatal, TEXT("Exception thrown: %s"), UTF8_TO_TCHAR(e.what()));
    // It should never reach this part.
    std::terminate();
  }

} // namespace sirv


