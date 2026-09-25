#pragma once

#include "Core.h"
#include "Widgets/SCompoundWidget.h" //#include "SCompoundWidget.h"
#include "UnrealClient.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"

#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"

class SWindow;
class SViewport;
class FSceneViewport;

/**
* Custom Viewport client, where we have access to canvas on Draw function.
* This is the place where we issue canvas draw call to present SceneCaptureComponent Texture to custom window
*/
class FPlaySceneViewportClient : public FViewportClient
{
public:
	/** FViewportClient interface */
	/**
	* Execute Draw each tick
	* This is the place for issue draw cumments and start rendering
	*/
	virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;
	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.0f, bool bGamepad = false) override;
	virtual bool InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;
	virtual bool InputGesture(FViewport* Viewport, EGestureEvent GestureType, const FVector2D& GestureDelta, bool bIsDirectionInvertedFromDevice) override;
	virtual UWorld* GetWorld() const override { return nullptr; }
	virtual void RedrawRequested(FViewport* Viewport) override { Viewport->Draw(); }

	//TWeakObjectPtr<UTextureRenderTarget2D> GetTextureRenderTarget2D();
	void SetTextureRenderTarget2D(UTextureRenderTarget2D* TextureRenderTarget2D);

private:

	TWeakObjectPtr<UTextureRenderTarget2D>  TextRenderTarget2D;
};

/**
 * Implements the Play Capture viewport
 */
class SPlaySceneViewport : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SPlaySceneViewport) { }
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<SViewport> Viewport;
	TSharedPtr<FSceneViewport> SceneViewport;
	TSharedPtr<FPlaySceneViewportClient> PlaySceneViewportClient;

public:
	// SWidget overrides
	/**
	* Tick each engine tick, this is where we call FViewport::Draw() --> FPlaySceneViewportClient::Draw(...)
	*/
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void SetTextureRenderTarget2D(UTextureRenderTarget2D* TexRenderTarget2D);

};

/** 
* This is main container class, which is create Play Capture Window
* And it assign rendering Viewport for the Window
*/
class FPlaySceneSlate
{


using Shared_Ptr = TSharedPtr<FPlaySceneSlate>;

public:
	FPlaySceneSlate();
	~FPlaySceneSlate();

public:
	static void Initialize();
	static void Shutdown();
	static Shared_Ptr Get();

	void SetLeftTextureRenderTarget2D(UTextureRenderTarget2D* TexRenderTarget2D);
	void SetRightTextureRenderTarget2D(UTextureRenderTarget2D* TexRenderTarget2D);

private:
	TSharedPtr<SWindow> PlaySceneWindowLeft;
	TSharedPtr<SWindow> PlaySceneWindowRight;

	int PlaySceneWindowWidth;
	int PlaySceneWindowHeight;

	FOnWindowClosed InDelegateLeft;
	FOnWindowClosed InDelegateRight;
private:
	/**
	* Destroy window and all rendering viewports when window has been closed
	*/
	void OnWindowClosed(const TSharedRef<SWindow>& Window);

	void createLeftWindow();
	void createRightWindow();

private:
	TSharedPtr<SPlaySceneViewport> PlaySceneViewportLeft;
	TSharedPtr<SPlaySceneViewport> PlaySceneViewportRight;

	static TSharedPtr<FPlaySceneSlate> PlaySceneSlate;
	//static TSharedPtr<FPlaySceneSlate> PlaySceneSlateRight;
	
	
};
