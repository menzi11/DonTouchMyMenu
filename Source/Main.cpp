/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <windows.h>
#include <JuceHeader.h>
#include "MainComponent.h"

static bool elevateToAdminRights( String commandLineArgs )
{
    char szPath[MAX_PATH];
    if( GetModuleFileName( NULL , szPath , ARRAYSIZE( szPath ) ) )
    {
        // Re-launch ourselves with admin rights
        SHELLEXECUTEINFO sei = { sizeof( sei ) };
        sei.lpVerb = "runas";
        sei.lpFile = szPath;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;
        sei.lpParameters = commandLineArgs.getCharPointer();
        if( !ShellExecuteEx( &sei ) )
        {
            DWORD dwError = GetLastError();
            if( dwError == ERROR_CANCELLED )
            {
                // User refused to allow privileges elevation.
                return false;
            }
        }
        else
        {
            //_exit( 1 );
            return true;
        }
    }
    return false;
}


class DonTouchMyMenuApplication  : public juce::JUCEApplication
{
public:
    
    DonTouchMyMenuApplication() 
    {
        //elevateToAdminRights( "" );
    }

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return false; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        mainWindow.reset (new MainWindow (getApplicationName()));
        myTrayIcon.reset( new MySystemTrayIconComponent( *this ) );
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        myTrayIcon = nullptr;
    }

    void toSystemTray()
    {
        mainWindow = nullptr;
    }

    void show()
    {
        if( mainWindow == nullptr )
        {
            mainWindow.reset( new MainWindow( getApplicationName() ) );
            mainWindow->setAlwaysOnTop( true );
            mainWindow->setAlwaysOnTop( false );
        }
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        mainWindow = nullptr;
        quit();
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow( juce::String name )
            : DocumentWindow( name , juce::Desktop::getInstance().getDefaultLookAndFeel().findColour( juce::ResizableWindow::backgroundColourId ) , DocumentWindow::allButtons )
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
            setVisible (true);
        }

        void closeButtonPressed() override
        {
            dynamic_cast<DonTouchMyMenuApplication*>( JUCEApplication::getInstance() )->toSystemTray();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    class MySystemTrayIconComponent : public juce::SystemTrayIconComponent
    {
    public:

        MySystemTrayIconComponent( DonTouchMyMenuApplication& app )
            : mApp( app )
        {
            auto icon = juce::ImageFileFormat::loadFrom( BinaryData::icon_png , BinaryData::icon_pngSize );
            setIconImage( icon , icon );
            setIconTooltip( "DonTouchMyMenu" );
        }

        void mouseDown( const juce::MouseEvent& e ) override
        {
            if( e.mods.isLeftButtonDown() )
            {
                mApp.show();
            }
            else if( e.mods.isRightButtonDown() )
            {
                PopupMenu menu;
                menu.addItem( 1 , "Show" );
                menu.addItem( 2 , "Close" );
                menu.showMenuAsync( PopupMenu::Options() , ModalCallbackFunction::create( []( int result ) {
                    if( result == 1 )
                    {
                        dynamic_cast<DonTouchMyMenuApplication*>( JUCEApplication::getInstance() )->show();
                    }
                    else if( result == 2 )
                    {
                        JUCEApplication::getInstance()->systemRequestedQuit();
                    }
                } ) );
            }
        }

    private:
        DonTouchMyMenuApplication& mApp;
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<MySystemTrayIconComponent> myTrayIcon;

    void anotherInstanceStarted( const juce::String& ) override
    {}
};

////==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (DonTouchMyMenuApplication)
