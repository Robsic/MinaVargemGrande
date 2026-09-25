#include "PlaySceneSlate.h"

//#include "PlayScene.h"

#include "Framework/Application/SlateApplication.h" //#include "SlateApplication.h"

#include "Engine.h"

#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Public/Slate/SceneViewport.h"
#include "Widgets/SViewport.h" //#include "SViewport.h"
#include "Widgets/SToolTip.h"
#include "Widgets/SWindow.h"

//#include "IPlayScene.h"


void FPlaySceneViewportClient::Draw(FViewport * Viewport, FCanvas * Canvas)
{
	// Clear entire canvas
	Canvas->Clear(FLinearColor::Black);

	// Draw SceenCaptureComponent texture to entire canvas
	//auto TextRenderTarget2D = FPlaySceneModule::GetTextureRenderTarget2D();
	
	if (TextRenderTarget2D.IsValid() && TextRenderTarget2D->Resource != nullptr)
	{
		FCanvasTileItem TileItem(FVector2D(0, 0), TextRenderTarget2D->Resource, 
			FVector2D(Viewport->GetRenderTargetTexture()->GetSizeX(), Viewport->GetRenderTargetTexture()->GetSizeY()),
			FLinearColor::White);
		TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Opaque;
		Canvas->DrawItem(TileItem);
	}
}

bool FPlaySceneViewportClient::InputKey(FViewport * Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	return false;
}

bool FPlaySceneViewportClient::InputAxis(FViewport * Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	return false;
}

bool FPlaySceneViewportClient::InputGesture(FViewport * Viewport, EGestureEvent GestureType, const FVector2D & GestureDelta, bool bIsDirectionInvertedFromDevice)
{
	return false;
}

void FPlaySceneViewportClient::SetTextureRenderTarget2D(UTextureRenderTarget2D * TextureRenderTarget2D)
{
	TextRenderTarget2D = TextureRenderTarget2D;
}


void SPlaySceneViewport::Construct(const FArguments& InArgs)
{
	// Create Viewport Widget
	Viewport = SNew(SViewport)
		.IsEnabled(true)
		.EnableGammaCorrection(false)
		.ShowEffectWhenDisabled(false)
		.EnableBlending(true)
		.ToolTip(SNew(SToolTip).Text(FText::FromString("SPlaySceneViewport")));


	// Create Viewport Client
	PlaySceneViewportClient = MakeShareable(new FPlaySceneViewportClient());

	// Create Scene Viewport
	SceneViewport = MakeShareable(new FSceneViewport(PlaySceneViewportClient.Get(), Viewport));

	// Assign SceneViewport to Viewport widget. It needed for rendering
	Viewport->SetViewportInterface(SceneViewport.ToSharedRef());

	// Assing Viewport widget for our custom PlayScene Viewport
	this->ChildSlot
		[
			Viewport.ToSharedRef()
		];
}

void SPlaySceneViewport::Tick(const FGeometry & AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// Call FViewport each engine tick
	SceneViewport->Draw();
}

void SPlaySceneViewport::SetTextureRenderTarget2D(UTextureRenderTarget2D * TexRenderTarget2D)
{
	if(PlaySceneViewportClient.IsValid())
		PlaySceneViewportClient->SetTextureRenderTarget2D(TexRenderTarget2D);
}


TSharedPtr<FPlaySceneSlate> FPlaySceneSlate::PlaySceneSlate = nullptr;


void FPlaySceneSlate::createLeftWindow()
{
	// Create SWindow
	PlaySceneWindowLeft = SNew(SWindow)
		.Title(FText::FromString("SIRV-SIMULATOR LEFT WINDOW"))
		.ScreenPosition(FVector2D(1920, 0))
		.ClientSize(FVector2D(PlaySceneWindowWidth, PlaySceneWindowHeight))
		.AutoCenter(EAutoCenter::None)//(EAutoCenter::PreferredWorkArea)
		.UseOSWindowBorder(true)
		.SaneWindowPlacement(false)
		.SizingRule(ESizingRule::UserSized)
		.IsInitiallyMaximized(true);

	FSlateApplication::Get().AddWindow(PlaySceneWindowLeft.ToSharedRef());

	// Assign window events delegator
	InDelegateLeft.BindRaw(this, &FPlaySceneSlate::OnWindowClosed);
	PlaySceneWindowLeft->SetOnWindowClosed(InDelegateLeft);

	// Create and assign viewport to window
	PlaySceneViewportLeft = SNew(SPlaySceneViewport);
	PlaySceneWindowLeft->SetContent(PlaySceneViewportLeft.ToSharedRef());

}

void FPlaySceneSlate::createRightWindow()
{
	// Create SWindow
	PlaySceneWindowRight = SNew(SWindow)
		.Title(FText::FromString("SIRV-SIMULATOR RIGHT WINDOW"))
		.ScreenPosition(FVector2D(5760, 0))
		.ClientSize(FVector2D(PlaySceneWindowWidth, PlaySceneWindowHeight))
		.AutoCenter(EAutoCenter::None)//(EAutoCenter::PreferredWorkArea)
		.UseOSWindowBorder(true)
		.SaneWindowPlacement(false)
		.SizingRule(ESizingRule::UserSized)
		.IsInitiallyMaximized(true);

	FSlateApplication::Get().AddWindow(PlaySceneWindowRight.ToSharedRef());

	// Assign window events delegator
	InDelegateRight.BindRaw(this, &FPlaySceneSlate::OnWindowClosed);
	PlaySceneWindowRight->SetOnWindowClosed(InDelegateRight);

	// Create and assign viewport to window
	PlaySceneViewportRight = SNew(SPlaySceneViewport);
	PlaySceneWindowRight->SetContent(PlaySceneViewportRight.ToSharedRef());

}

FPlaySceneSlate::FPlaySceneSlate()
	: PlaySceneWindowWidth(1280)
	, PlaySceneWindowHeight(720)
{

	this->createLeftWindow();
	this->createRightWindow();

}

FPlaySceneSlate::~FPlaySceneSlate()
{
	PlaySceneWindowLeft->RequestDestroyWindow();
	PlaySceneWindowRight->RequestDestroyWindow();
	
	InDelegateLeft.Unbind();
	InDelegateRight.Unbind();
}

 void FPlaySceneSlate::Initialize()
 {
 	// Create single instance of PlaySceneSlate
 	if (!PlaySceneSlate.IsValid())
 	{
 		FPlaySceneSlate::PlaySceneSlate = MakeShareable(new FPlaySceneSlate());
 	}
 }

void FPlaySceneSlate::Shutdown()
{
	if (PlaySceneSlate.IsValid())
	{
		FPlaySceneSlate::PlaySceneSlate.Reset();
		FPlaySceneSlate::PlaySceneSlate = nullptr;;
	}
}

void FPlaySceneSlate::OnWindowClosed(const TSharedRef<SWindow>& Window)
{
	FPlaySceneSlate::Shutdown();	
}

//TWeakObjectPtr<UTextureRenderTarget2D> FPlaySceneSlate::GetTextureRenderTarget2D()
//{
//	return TextureRenderTarget2D;
//}

void FPlaySceneSlate::SetLeftTextureRenderTarget2D(UTextureRenderTarget2D * TexRenderTarget2D)
{
	if(PlaySceneViewportLeft.IsValid())
		PlaySceneViewportLeft->SetTextureRenderTarget2D(TexRenderTarget2D);
}

void FPlaySceneSlate::SetRightTextureRenderTarget2D(UTextureRenderTarget2D * TexRenderTarget2D)
{
	if(PlaySceneViewportRight.IsValid())
		PlaySceneViewportRight->SetTextureRenderTarget2D(TexRenderTarget2D);
}


FPlaySceneSlate::Shared_Ptr FPlaySceneSlate::Get()
{
	FPlaySceneSlate::Initialize();
	return FPlaySceneSlate::PlaySceneSlate;
}
