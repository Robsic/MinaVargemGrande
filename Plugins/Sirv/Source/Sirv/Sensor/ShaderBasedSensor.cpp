// Copyright (c) 2023 Laboratory of Robotics, Intelligent and Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#include "Sensor/ShaderBasedSensor.h"
#include "Sirv.h"

#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

bool AShaderBasedSensor::AddPostProcessingMaterial(const FString &Path)
{
  ConstructorHelpers::FObjectFinder<UMaterial> Loader(*Path);
  if (Loader.Succeeded())
  {
    MaterialsFound.Add(Loader.Object);
  }
  return Loader.Succeeded();
}

void AShaderBasedSensor::SetUpSceneCaptureComponent(USceneCaptureComponent2D &SceneCapture)
{
  for (const auto &MaterialFound : MaterialsFound)
  {
    // Create a dynamic instance of the Material (Shader)
    AddShader({UMaterialInstanceDynamic::Create(MaterialFound, this), 1.0});
  }

  for (const auto &Shader : Shaders)
  {
    // Attach the instance into the blendables
    SceneCapture.PostProcessSettings.AddBlendable(Shader.PostProcessMaterial, Shader.Weight);
  }

  // Set the value for each Float parameter in the shader
  for (const auto &ParameterValue : FloatShaderParams)
  {
    Shaders[ParameterValue.ShaderIndex].PostProcessMaterial->SetScalarParameterValue(
        ParameterValue.ParameterName,
        ParameterValue.Value);
  }
}

void AShaderBasedSensor::Set(const FActorDescription &Description)
{
  Super::Set(Description);
  UActorBlueprintFunctionLibrary::SetCamera(Description, this);
}

void AShaderBasedSensor::SetFloatShaderParameter(
    uint8_t ShaderIndex,
    const FName &ParameterName,
    float Value)
{
  FloatShaderParams.Add({ShaderIndex, ParameterName, Value});
}
