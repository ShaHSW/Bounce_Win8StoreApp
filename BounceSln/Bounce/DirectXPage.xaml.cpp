//
// DirectXPage.xaml.cpp
// Implementation of the DirectXPage.xaml class.
//

#include "pch.h"
#include "DirectXPage.xaml.h"

using namespace Bounce;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Input;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::System::Threading;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::ApplicationModel::Resources;

DirectXPage::DirectXPage()
{
	InitializeComponent();

	m_renderer = ref new GamePlayRenderer();

	m_renderer->Initialize(
		Window::Current->CoreWindow,
		SwapChainPanel,
		DisplayProperties::LogicalDpi
		);

    m_renderer->GameOver += 
        ref new EventHandler<Object^>(this, &DirectXPage::OnGameOver);

    m_renderer->RenderNeeded();

    Window::Current->CoreWindow->SizeChanged += 
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &DirectXPage::OnWindowSizeChanged);

	DisplayProperties::LogicalDpiChanged +=
		ref new DisplayPropertiesEventHandler(this, &DirectXPage::OnLogicalDpiChanged);

	DisplayProperties::OrientationChanged +=
        ref new DisplayPropertiesEventHandler(this, &DirectXPage::OnOrientationChanged);

	DisplayProperties::DisplayContentsInvalidated +=
		ref new DisplayPropertiesEventHandler(this, &DirectXPage::OnDisplayContentsInvalidated);
	
	m_eventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &DirectXPage::OnRendering));

	m_timer = ref new BasicTimer();
    m_globlizationResource = ref new ResourceLoader();

    ToolTipService::SetToolTip(BtnDonate, m_globlizationResource->GetString("BtnDonate_tool_tip"));

    m_dispatcherTimer = ref new DispatcherTimer();

    TimeSpan span;
    span.Duration = 10000000; // Ticks per second
    m_dispatcherTimer->Interval = span;
    m_dispatcherTimer->Tick +=
        ref new EventHandler<Object^>(this, &DirectXPage::OnTick); 
}

void DirectXPage::OnGameOver(Object^ sender, Object^ args)
{
    MainSelectionGrid->Visibility = Windows::UI::Xaml::Visibility::Visible;
    AppBarBottom->Visibility = Windows::UI::Xaml::Visibility::Visible;
    m_renderer->SetGamePlayScreen(GamePlayScreen::DEFAULT_SCREEN);
    m_renderer->RenderNeeded();
    m_dispatcherTimer->Stop();
}

void DirectXPage::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	m_renderer->UpdateForWindowSizeChange();
	m_renderer->RenderNeeded();
}

void DirectXPage::OnLogicalDpiChanged(Object^ sender)
{
	m_renderer->SetDpi(DisplayProperties::LogicalDpi);
	m_renderer->RenderNeeded();
}

void DirectXPage::OnOrientationChanged(Object^ sender)
{
	m_renderer->UpdateForWindowSizeChange();
	m_renderer->RenderNeeded();
}

void DirectXPage::OnDisplayContentsInvalidated(Object^ sender)
{
	m_renderer->ValidateDevice();
	m_renderer->RenderNeeded();
}

void DirectXPage::OnRendering(Object^ sender, Object^ args)
{
	if (m_renderer->GetRenderState())
	{
		m_timer->Update();
		m_renderer->Update(m_timer->Total, m_timer->Delta);
		m_renderer->Render();
		m_renderer->Present();
	}
}

void DirectXPage::OnPreviousColorPressed(Object^ sender, RoutedEventArgs^ args)
{
	//m_renderer->BackgroundColorPrevious();
	//m_renderNeeded = true;
}

void DirectXPage::OnNextColorPressed(Object^ sender, RoutedEventArgs^ args)
{
    //m_renderer->BackgroundColorNext();
	//m_renderNeeded = true;
	
}

void DirectXPage::OnDonatePressed(Object^ sender, RoutedEventArgs^ args)
{
	auto licenseInformation = CurrentAppSimulator::LicenseInformation;
    if (licenseInformation->ProductLicenses->Lookup(BOUNCE_DONATE_STRING)->IsActive)
    {
        BtnDonate->IsEnabled = false;
    }
    else
    {
        create_task(CurrentAppSimulator::RequestProductPurchaseAsync(BOUNCE_DONATE_STRING, true)).then([this](task<String^> currentTask)
        {
            try
            {
                currentTask.get();
                auto licenseInformation = CurrentAppSimulator::LicenseInformation;
                if (licenseInformation->ProductLicenses->Lookup(BOUNCE_DONATE_STRING)->IsActive)
                {
                    BtnDonate->IsEnabled = false;
                }
                else
                {
                    BtnDonate->IsEnabled = true;
                }
            }
            catch(Platform::Exception^ exception)
            {
                BtnDonate->IsEnabled = true;
             }
        });
    }
}

void DirectXPage::SaveInternalState(IPropertySet^ state)
{
	m_renderer->SaveInternalState(state);
}

void DirectXPage::LoadInternalState(IPropertySet^ state)
{
	m_renderer->LoadInternalState(state);
}

void DirectXPage::OnKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
    switch(e->Key)
    {
    case Windows::System::VirtualKey::W:
        m_renderer->UpdatePaddleMovementDelta(GamePlayers::PLAYER_1, -DEFAULT_PADDLE_MOVEMENT_SPEED);
        break;
    case Windows::System::VirtualKey::S:
        m_renderer->UpdatePaddleMovementDelta(GamePlayers::PLAYER_1, DEFAULT_PADDLE_MOVEMENT_SPEED);
        break;
    case Windows::System::VirtualKey::Up:
        m_renderer->UpdatePaddleMovementDelta(GamePlayers::PLAYER_2, -DEFAULT_PADDLE_MOVEMENT_SPEED);
        break;
    case Windows::System::VirtualKey::Down:
        m_renderer->UpdatePaddleMovementDelta(GamePlayers::PLAYER_2, DEFAULT_PADDLE_MOVEMENT_SPEED);
        break;
    }

    m_renderer->RenderNeeded();
}


void DirectXPage::OnKeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
    switch(e->Key)
    {
    case Windows::System::VirtualKey::W:
    case Windows::System::VirtualKey::S:
        m_renderer->UpdatePaddleMovementDelta(GamePlayers::PLAYER_1, 0.0f);
        break;
    case Windows::System::VirtualKey::Up:
    case Windows::System::VirtualKey::Down:
        m_renderer->UpdatePaddleMovementDelta(GamePlayers::PLAYER_2, 0.0f);
        break;
    }
    
    m_renderer->RenderNeeded();
}


void Bounce::DirectXPage::InitializeGamePlay(const int difficulty)
{
    m_renderer->ResetDurationTimer();
    MainSelectionGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    AppBarBottom->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    m_renderer->SetGamePlayDifficulty(difficulty);
    m_renderer->SetGamePlayScreen(GamePlayScreen::COUNT_DOWN_SCREEN);
    m_renderer->InitializeGame();
    m_renderer->RenderNeeded();
    m_dispatcherTimer->Start();
    m_renderer->PlayCountDown();
}

void DirectXPage::BtnPlayFriendsVeryEasy_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InitializeGamePlay(GamePlayDifficulty::GAME_VERY_EASY);
}


void DirectXPage::BtnPlayFriendsEasy_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InitializeGamePlay(GamePlayDifficulty::GAME_EASY);
}


void DirectXPage::BtnPlayFriendsNormal_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InitializeGamePlay(GamePlayDifficulty::GAME_NORMAL);
}


void DirectXPage::BtnPlayFriendsHard_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    InitializeGamePlay(GamePlayDifficulty::GAME_HARD);
}


void DirectXPage::OnTick(Platform::Object^ sender, Platform::Object^ e)
{
    int duration = m_renderer->IncementDurationCount();
    int screen = m_renderer->GetGamePlayScreen();

    if ((WAIT_TIME == duration) && (screen != GamePlayScreen::GAME_PLAY_SCREEN))
    {
        m_renderer->SetGamePlayScreen(GamePlayScreen::GAME_PLAY_SCREEN);
    }
    else if (screen == GamePlayScreen::GAME_OVER_SCREEN)
    {
        m_renderer->RenderNeeded();
    }
    else if (screen == GamePlayScreen::DEFAULT_SCREEN)
    {
        m_dispatcherTimer->Stop();
    }
    else if ((screen == GamePlayScreen::COUNT_DOWN_SCREEN) && (duration < (WAIT_TIME - 1)))
    {
        m_renderer->PlayCountDown();
    }
    else if ((screen == GamePlayScreen::COUNT_DOWN_SCREEN) && (duration == (WAIT_TIME - 1)))
    {
        m_renderer->PlayGameStart();
    }

    m_renderer->UpdateBallVelocities();
}