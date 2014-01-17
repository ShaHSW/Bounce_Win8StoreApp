//
// BlankPage.xaml.h
// Declaration of the BlankPage.xaml class.
//

#pragma once

#include "DirectXPage.g.h"
#include "GamePlayRenderer.h"
#include "BasicTimer.h"

#define BOUNCE_DONATE_STRING "donate"

namespace Bounce
{
	/// <summary>
	/// A DirectX page that can be used on its own.  Note that it may not be used within a Frame.
	/// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
	public ref class DirectXPage sealed
	{
	public:
		DirectXPage();

		void OnPreviousColorPressed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnNextColorPressed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnDonatePressed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void BtnPlayFriendsVeryEasy_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void BtnPlayFriendsEasy_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void BtnPlayFriendsNormal_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void BtnPlayFriendsHard_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void SaveInternalState(Windows::Foundation::Collections::IPropertySet^ state);
		void LoadInternalState(Windows::Foundation::Collections::IPropertySet^ state);

    private:
		void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
		void OnLogicalDpiChanged(Platform::Object^ sender);
		void OnOrientationChanged(Platform::Object^ sender);
		void OnDisplayContentsInvalidated(Platform::Object^ sender);
		void OnRendering(Object^ sender, Object^ args);
        void OnKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
        void OnKeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
        void OnTick(Object^ sender, Object^ args);
        void OnGameOver(Object^ sender, Object^ args);

        void InitializeGamePlay(const int difficulty);

		Windows::Foundation::EventRegistrationToken m_eventToken;
        Windows::ApplicationModel::Resources::ResourceLoader^ m_globlizationResource;

		GamePlayRenderer^ m_renderer;
		BasicTimer^ m_timer;
        Windows::UI::Xaml::DispatcherTimer^ m_dispatcherTimer;
    };
}
