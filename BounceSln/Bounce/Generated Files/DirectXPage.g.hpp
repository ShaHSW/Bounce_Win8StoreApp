﻿

//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"
#include "DirectXPage.xaml.h"




void ::Bounce::DirectXPage::InitializeComponent()
{
    if (_contentLoaded)
        return;

    _contentLoaded = true;

    // Call LoadComponent on ms-appx:///DirectXPage.xaml
    ::Windows::UI::Xaml::Application::LoadComponent(this, ref new ::Windows::Foundation::Uri(L"ms-appx:///DirectXPage.xaml"), ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);

    // Get the SwapChainBackgroundPanel named 'SwapChainPanel'
    SwapChainPanel = safe_cast<::Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"SwapChainPanel"));
    // Get the Grid named 'MainSelectionGrid'
    MainSelectionGrid = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"MainSelectionGrid"));
    // Get the Button named 'BtnPlayFriendsVeryEasy'
    BtnPlayFriendsVeryEasy = safe_cast<::Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"BtnPlayFriendsVeryEasy"));
    // Get the Button named 'BtnPlayFriendsEasy'
    BtnPlayFriendsEasy = safe_cast<::Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"BtnPlayFriendsEasy"));
    // Get the Button named 'BtnPlayFriendsNormal'
    BtnPlayFriendsNormal = safe_cast<::Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"BtnPlayFriendsNormal"));
    // Get the Button named 'BtnPlayFriendsHard'
    BtnPlayFriendsHard = safe_cast<::Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"BtnPlayFriendsHard"));
    // Get the AppBar named 'AppBarBottom'
    AppBarBottom = safe_cast<::Windows::UI::Xaml::Controls::AppBar^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"AppBarBottom"));
    // Get the Button named 'BtnDonate'
    BtnDonate = safe_cast<::Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"BtnDonate"));
}

void ::Bounce::DirectXPage::Connect(int connectionId, Platform::Object^ target)
{
    switch (connectionId)
    {
    case 1:
        (safe_cast<::Windows::UI::Xaml::UIElement^>(target))->KeyDown +=
            ref new ::Windows::UI::Xaml::Input::KeyEventHandler(this, (void (::Bounce::DirectXPage::*)(Platform::Object^, Windows::UI::Xaml::Input::KeyRoutedEventArgs^))&DirectXPage::OnKeyDown);
        (safe_cast<::Windows::UI::Xaml::UIElement^>(target))->KeyUp +=
            ref new ::Windows::UI::Xaml::Input::KeyEventHandler(this, (void (::Bounce::DirectXPage::*)(Platform::Object^, Windows::UI::Xaml::Input::KeyRoutedEventArgs^))&DirectXPage::OnKeyUp);
        break;
    case 2:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Bounce::DirectXPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&DirectXPage::BtnPlayFriendsVeryEasy_Click);
        break;
    case 3:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Bounce::DirectXPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&DirectXPage::BtnPlayFriendsEasy_Click);
        break;
    case 4:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Bounce::DirectXPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&DirectXPage::BtnPlayFriendsNormal_Click);
        break;
    case 5:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Bounce::DirectXPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&DirectXPage::BtnPlayFriendsHard_Click);
        break;
    case 6:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Bounce::DirectXPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&DirectXPage::OnDonatePressed);
        break;
    }
    (void)connectionId; // Unused parameter
    (void)target; // Unused parameter
    _contentLoaded = true;
}
