// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "SirvScreenSceneCaptureComponent2D.generated.h"

/**
 * 
 */
UCLASS(hidecategories = (Collision, Object, Physics, SceneComponent), ClassGroup = Rendering, editinlinenew, meta = (BlueprintSpawnableComponent))
class SIRV_API USirvScreenSceneCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:

	USirvScreenSceneCaptureComponent2D(const FObjectInitializer& ObjectInitializer);
	~USirvScreenSceneCaptureComponent2D();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Activate(bool bReset) override;

#if WITH_EDITOR
	virtual void OnRegister() override;
#endif

	// Defines which monitor the camera will show the image
	UPROPERTY(Category = "Screen Monitor", EditAnywhere, BlueprintReadWrite)
  	FString monitor;
private:


	//TSharedPtr<FPlaySceneSlate> PlaySceneSlate;
	
};
