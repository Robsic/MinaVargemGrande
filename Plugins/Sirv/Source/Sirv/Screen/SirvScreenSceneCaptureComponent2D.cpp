// Fill out your copyright notice in the Description page of Project Settings.

#include "SirvScreenSceneCaptureComponent2D.h"

#include "PlaySceneSlate.h"

USirvScreenSceneCaptureComponent2D::USirvScreenSceneCaptureComponent2D(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  
  
  UE_LOG(LogTemp, Warning, TEXT("::::::::::::::::::: USirvScreenSceneCaptureComponent2D CONSTRUCTOR...") );
  //PlaySceneSlate = MakeShareable(new FPlaySceneSlate());

}

USirvScreenSceneCaptureComponent2D::~USirvScreenSceneCaptureComponent2D()
{
  
  
  UE_LOG(LogTemp, Warning, TEXT("::::::::::::::::::: USirvScreenSceneCaptureComponent2D DESTRUCTOR...") );
  
  

}

 void USirvScreenSceneCaptureComponent2D::BeginPlay()
{
	Super::BeginPlay();

	//PlaySceneSlate = MakeShareable(new FPlaySceneSlate());
	FPlaySceneSlate::Initialize();

	UE_LOG(LogTemp, Warning, TEXT("::::::::::::::::::: USirvScreenSceneCaptureComponent2D BeginPlay...") );

	if(monitor.Equals(TEXT("LEFT")))
	{
		UE_LOG(LogTemp, Warning, TEXT("::::::::::::::::::: USirvScreenSceneCaptureComponent2D LEFT MONITOR...") );
		if(FPlaySceneSlate::Get().IsValid())
			FPlaySceneSlate::Get()->SetLeftTextureRenderTarget2D(this->TextureTarget);
	}
	
	if (monitor.Equals(TEXT("RIGHT")))
	{
		UE_LOG(LogTemp, Warning, TEXT("::::::::::::::::::: USirvScreenSceneCaptureComponent2D RIGHT MONITOR...") );
		if(FPlaySceneSlate::Get().IsValid())
			FPlaySceneSlate::Get()->SetRightTextureRenderTarget2D(this->TextureTarget);

	}
}

void USirvScreenSceneCaptureComponent2D::Activate(bool bReset)
{
	Super::Activate();

	UE_LOG(LogTemp, Warning, TEXT("::::::::::::::::::: USirvScreenSceneCaptureComponent2D ACTIVE...") );

	// if(!PlaySceneSlate.IsValid())
	// 	PlaySceneSlate = MakeShareable(new FPlaySceneSlate());

	// // Assign Render texture to shared module variale
	// PlaySceneSlate->SetTextureRenderTarget2D(this->TextureTarget);
}

#if WITH_EDITOR
void USirvScreenSceneCaptureComponent2D::OnRegister()
{
	Super::OnRegister();

	UE_LOG(LogTemp, Warning, TEXT("::::::::::::::::::: USirvScreenSceneCaptureComponent2D OnRegister...") );
	// Assign Render texture to shared module variale
	
	// if(!PlaySceneSlate.IsValid())
	// 	PlaySceneSlate = MakeShareable(new FPlaySceneSlate());

	// PlaySceneSlate->SetTextureRenderTarget2D(this->TextureTarget);
}
#endif


void USirvScreenSceneCaptureComponent2D::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  
  Super::EndPlay(EndPlayReason);

  UE_LOG(LogTemp, Warning, TEXT("::::::::::::::::::: USirvScreenSceneCaptureComponent2D EndPlay...") );


  FPlaySceneSlate::Shutdown();

//   if ((PlaySceneSlate != nullptr) && (EndPlayReason != EEndPlayReason::EndPlayInEditor))
//   {
//     PlaySceneSlate.Reset();
// 	PlaySceneSlate = nullptr;
//   }
}
