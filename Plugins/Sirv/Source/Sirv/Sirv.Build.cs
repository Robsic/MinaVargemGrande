// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

using System;
using System.IO;
using System.Diagnostics;
using System.Text;
using System.Linq;
using System.Collections.Generic;
using UnrealBuildTool;

public class Sirv : ModuleRules
{  
  //bool UsingCarSim = false;
  
  private bool IsWindows(ReadOnlyTargetRules Target)
  {
    return (Target.Platform == UnrealTargetPlatform.Win64);
  }

  public Sirv(ReadOnlyTargetRules Target) : base(Target)
  {

    PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
    //PrivatePCHHeaderFile = "Sirv.h";
    CppStandard = CppStandardVersion.Latest;

    if (IsWindows(Target))
    {
      bEnableExceptions = true;
    }


    // Read config about carsim
    //string SirvPluginPath = Path.GetFullPath( ModuleDirectory );
    //string ConfigDir =  Path.GetFullPath(Path.Combine(SirvPluginPath, "../../../../Config/"));
    //string CarSimConfigFile = Path.Combine(ConfigDir, "CarSimConfig.ini");
    // string[] text = System.IO.File.ReadAllLines(CarSimConfigFile);
    // Console.WriteLine("----------------------------------------------");
    // foreach (string line in text)
    // {
    //   Console.WriteLine(line);
    //   if (line.Contains("CarSim ON"))
    //   {
    //     Console.WriteLine("Enabling carsim-----------");
    //     UsingCarSim = true;
    //     PublicDefinitions.Add("WITH_CARSIM");
    //     PrivateDefinitions.Add("WITH_CARSIM");
    //     Definitions.Add("WITH_CARSIM");
    //   }
    // }

    PublicIncludePaths.AddRange(
      new string[] {
        // ... add public include paths required here ...
      }
      );
    
    PrivateIncludePaths.AddRange(
      new string[] {
        // ... add other private include paths required here ...
      }
      );

    PublicDependencyModuleNames.AddRange(
      new string[]
      {
        "Core",
        "RenderCore",
	      "CoreUObject",
        "Engine",                    
        "RHI",
        "ProceduralMeshComponent"
        // ... add other public dependencies that you statically link with here ...
      }
      );
    //if (UsingCarSim)
    //{
    //  PublicDependencyModuleNames.AddRange(new string[] { "CarSim" });
    //}

	 if (Target.Type == TargetType.Editor)
	 {
		PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
	 }

     PrivateDependencyModuleNames.AddRange(
      new string[]
      {
        "AIModule",
        "AssetRegistry",
        "CoreUObject",
        //"Engine",
        "Foliage",
        "ImageWriteQueue",
        "Json",
        "JsonUtilities",
        "Landscape",
        //"PhysX",
        //"PhysXVehicles",
        //"PhysXVehicleLib",
        "InputCore", 
        "ChaosVehicles", 
        "HeadMountedDisplay",
        "Slate",
        "SlateCore"
        //"Ros2"
        // ... add private dependencies that you statically link with here ...
      }
      );
    //if (UsingCarSim)
    //{
    //  PrivateDependencyModuleNames.AddRange(new string[] { "CarSim" });
    //  PrivateIncludePathModuleNames.AddRange(new string[] { "CarSim" });
    //}


    DynamicallyLoadedModuleNames.AddRange(
      new string[]
      {
        // ... add any modules that your module loads dynamically here ...
      }
      );



     AddSirvServerDependency(Target);
     //AddRos2SirvDependency(Target);

     PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");
     
    PublicIncludePaths.Add(Path.GetFullPath( ModuleDirectory ));
    PrivateIncludePaths.Add(Path.GetFullPath( ModuleDirectory ));
    
    ShadowVariableWarningLevel = WarningLevel.Off;

  }

  private bool UseDebugLibs(ReadOnlyTargetRules Target)
  {
    if (IsWindows(Target))
    {
      // In Windows, Unreal uses the Release C++ Runtime (CRT) even in debug
      // mode, so unless we recompile the engine we cannot link the debug
      // libraries.
      return false;
    }
    else
    {
      return false;
    }
  }


  private void AddDynamicLibrary(string library)
  {
    PublicAdditionalLibraries.Add(library);
    RuntimeDependencies.Add(library);
    PublicDelayLoadDLLs.Add(library);
  }


  private void AddDllDependency(string PathToFolder, string DllName)
  {
    string Source = Path.Combine(PathToFolder, DllName);
    string Destination = Path.Combine("$(BinaryOutputDir)", DllName);
    RuntimeDependencies.Add(Destination, Source);
  }
  delegate string ADelegate(string s);

  private void AddBoostLibs(string LibPath)
  {
    string [] files = Directory.GetFiles(LibPath, "*boost*.lib");
    foreach (string file in files) PublicAdditionalLibraries.Add(file);
  }

  private void AddSirvServerDependency(ReadOnlyTargetRules Target)
  {

    string command = CreateCMakeBuildCommand(Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../../../build")),"Release");
    var output = ExecuteBashCommand(command);

    var libraries_name = parsePath(output,"L-");

    string LibSirvInstallPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../SirvDependencies"));
    string LibSirvIncludePath = LibSirvInstallPath + "/include";
    string LibSirvLibraryPath = LibSirvInstallPath + "/lib";

    string LibCarlaInstallPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../SirvDependencies"));

    ADelegate GetLibName = (string BaseName) => {
      if (IsWindows(Target))
      {
        return BaseName + ".lib";
      }
      else
      {
        return "lib" + BaseName + ".a";
      }
    };


    //Console.WriteLine(" >>>>>>>>>>>>>>>>>>>>> HELLLLOOOOOO <<<<<<<<<<<<<<<<<<<<<<<");
    //Console.WriteLine(Path.Combine(LibSirvInstallPath, "sirv_dev/lib", GetLibName("sirv_dev")));

    // Link dependencies.
    if (IsWindows(Target))
    {
      // TODO...
    }
    else
    {

      //PublicAdditionalLibraries.Add(Path.Combine(LibCarlaInstallPath, "lib", GetLibName("sirv_ros_node")));
      if (UseDebugLibs(Target))
      {
        //PublicAdditionalLibraries.Add(Path.Combine(LibCarlaInstallPath, "lib", GetLibName("carla_server_debug")));
      }
      else
      {
        //PublicAdditionalLibraries.Add(Path.Combine(LibCarlaInstallPath, "lib", GetLibName("carla_server")));

        Console.WriteLine("\n Add Dynamic libraries... \n");
          /* foreach (var strPath in libraries_name)
          {
              string filename = Path.GetFileName(strPath);
              Console.WriteLine(LibSirvLibraryPath+"/"+filename);
              AddDynamicLibrary( LibSirvLibraryPath+"/"+filename);
          } */

          //string filename = Path.GetFileName(strPath);
                              
          //Console.WriteLine(LibSirvLibraryPath+"/"+filename);
          AddDynamicLibrary( LibSirvLibraryPath+"/"+"libsirv_dev.so");
          AddDynamicLibrary( LibSirvLibraryPath+"/"+"libsirv_ros_node.so");

          //AddDynamicLibrary( LibSirvLibraryPath+"/"+"libimage_transport_plugins.so");

          //RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++.so"));
          //RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++.so.1"));
          //RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++.so.1.0"));
          //RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++abi.so"));
          //RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++abi.so.1"));
          //RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++abi.so.1.0"));
/* 
          PublicDelayLoadDLLs.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++.so"));
          PublicDelayLoadDLLs.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++.so.1"));
          PublicDelayLoadDLLs.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++.so.1.0"));
          PublicDelayLoadDLLs.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++abi.so"));
          PublicDelayLoadDLLs.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++abi.so.1"));
          PublicDelayLoadDLLs.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++abi.so.1.0"));
 */

          Console.WriteLine(LibSirvLibraryPath);
          bUseRTTI = false;
          bEnableExceptions = true;

      }

   
      

      /* if (UsingChrono)
      {
        RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++.so"));
        RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++.so.1"));
        RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++.so.1.0"));
        RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++abi.so"));
        RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++abi.so.1"));
        RuntimeDependencies.Add(Path.Combine(LibCarlaInstallPath, "lib", "libc++abi.so.1.0"));
        AddDynamicLibrary(Path.Combine(LibCarlaInstallPath, "lib", "libChronoEngine.so"));
        AddDynamicLibrary(Path.Combine(LibCarlaInstallPath, "lib", "libChronoEngine_vehicle.so"));
        AddDynamicLibrary(Path.Combine(LibCarlaInstallPath, "lib", "libChronoModels_vehicle.so"));
        AddDynamicLibrary(Path.Combine(LibCarlaInstallPath, "lib", "libChronoModels_robot.so"));
        bUseRTTI = true;
        bEnableExceptions = true;
      } */
      
    }
    
    // Include path.
    //string LibSirvIncludePath = Path.Combine(LibSirvInstallPath, "sirv_dev/include");

    //PublicIncludePaths.Add(LibSirvIncludePath);
    //PrivateIncludePaths.Add(LibSirvIncludePath);
    
    //PublicIncludePaths.Add(Path.GetFullPath( ModuleDirectory ));
    //PrivateIncludePaths.Add(Path.GetFullPath( ModuleDirectory ));
    // Include path.
    string LibCarlaIncludePath = Path.Combine(LibCarlaInstallPath, "include");

    PublicIncludePaths.Add(LibCarlaIncludePath);
    PrivateIncludePaths.Add(LibCarlaIncludePath);

    PublicDefinitions.Add("ASIO_NO_EXCEPTIONS");
    PublicDefinitions.Add("BOOST_NO_EXCEPTIONS");
    PublicDefinitions.Add("LIBSIRV_NO_EXCEPTIONS");
    PublicDefinitions.Add("PUGIXML_NO_EXCEPTIONS");
    

  }  
  private string CreateCMakeBuildCommand(string buildDirectory, string buildType)
  {
    const string program = //"source /home/giovani/codigos/ros2_foxy/install/setup.bash  &&" +
                           //"source /home/giovani/codigos/SIRV/install/setup.bash  &&" +
                           //"source /home/giovani/codigos/SIRV-Simulator/install/setup.bash  &&" +
                           "source /home/simulation-pc/codigos/SIRV-Simulator/install/setup.bash  &&" +
                           "/usr/bin/cmake";

    // root/ue4View/TurnBasedGame/Source/TurnBasedGame
    var currentDir = Path.GetFullPath(ModuleDirectory);
    //var rootDirectory = Path.Combine(currentDir, "..", "..", "..", "..");
    //var installPath = Path.Combine(ThirdPartyPath, "generated");
    //var sourceDir = Path.Combine(rootDirectory, "model");

    var arguments = " -S " + currentDir +
                    " -B " + buildDirectory +
                    " -DCMAKE_BUILD_TYPE=" + buildType;

    return program + arguments;
  }
            
  private void AddRos2SirvDependency(ReadOnlyTargetRules Target)
  {
    string command = CreateCMakeBuildCommand(Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../../../build")),"Release");

    var output = ExecuteBashCommand(command);
    //var include_paths  = parsePath(output,"I-");
    var libraries_name = parsePath(output,"L-");

    string LibSirvInstallPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../SirvDependencies"));
    string LibSirvIncludePath = LibSirvInstallPath + "/include";
    string LibSirvLibraryPath = LibSirvInstallPath + "/lib";
    
    Console.WriteLine(LibSirvInstallPath);
    Console.WriteLine(LibSirvIncludePath);
    Console.WriteLine(LibSirvLibraryPath);
    PublicIncludePaths.Add(LibSirvIncludePath);
    PrivateIncludePaths.Add(LibSirvIncludePath);
    
    PublicIncludePaths.Add("/usr/include/");
    PrivateIncludePaths.Add("/usr/include/");
    

    // Link dependencies.
    if (IsWindows(Target))
    {
      //ToDo
    }
    else
    {
      if (UseDebugLibs(Target))
      {
        //ToDo  
      }
      else
      {               
          //Console.WriteLine("\n Add include directories... \n");
          //foreach (var include in include_paths)
          //{
          //    Console.WriteLine(include);
          //    PublicIncludePaths.Add(include);
          //    PrivateIncludePaths.Add(include);
          //}

          Console.WriteLine("\n Add Dynamic libraries... \n");
          foreach (var strPath in libraries_name)
          {
              string filename = Path.GetFileName(strPath);
              Console.WriteLine(LibSirvLibraryPath+"/"+filename);
              AddDynamicLibrary( LibSirvLibraryPath+"/"+filename);
          }

          Console.WriteLine(LibSirvLibraryPath);
          bUseRTTI = false;
          bEnableExceptions = true;
          
      }
    }

    //-Wall -Wextra -Wpedantic -Wno-inconsistent-missing-override
    PublicDefinitions.Add("Wall");
    PublicDefinitions.Add("Werror");
    PublicDefinitions.Add("Wextra");
    PublicDefinitions.Add("Wpedantic");

    PublicDefinitions.Add("ASIO_NO_EXCEPTIONS");
    PublicDefinitions.Add("BOOST_NO_EXCEPTIONS");
    PublicDefinitions.Add("LIBSIRV_NO_EXCEPTIONS");
    PublicDefinitions.Add("PUGIXML_NO_EXCEPTIONS");

    //PublicDefinitions.Add("RCLCPP__NODE_IMPL_HPP_");
  }


  static string ExecuteBashCommand(string command)
    {
        // according to: https://stackoverflow.com/a/15262019/637142
        // thans to this we will pass everything as one command
        command = command.Replace("\"","\"\"");

        var proc = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = "/bin/bash",
                Arguments = "-c \""+ command + "\"",
                UseShellExecute = false,
                RedirectStandardOutput = true,
                CreateNoWindow = true
            }
        };

        proc.Start();
        proc.WaitForExit();

        return proc.StandardOutput.ReadToEnd();
    }


  private List<string> parsePath(string command, string type)
  {
      var list = command.Split('\n');
      
      List<string> paths = new List<string>();
      foreach (var line in list)
      {
        if(line.Contains(type))        
          paths.Add(line.Remove(0,2));
      }

    return paths;
  }
  
}
