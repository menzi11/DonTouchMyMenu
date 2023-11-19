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
            _exit( 1 );
        }
    }
    return false;
}


class DonTouchMyMenuApplication  : public juce::JUCEApplication
{
public:
    
    DonTouchMyMenuApplication() 
    {
        elevateToAdminRights( "" );
    }

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..

        mainWindow.reset (new MainWindow (getApplicationName()));
        myTrayIcon.reset( new MySystemTrayIconComponent( *this ) );
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        myTrayIcon = nullptr;
    }

    void show()
    {
        if( mainWindow == nullptr )
        {
            mainWindow.reset( new MainWindow( getApplicationName() ) );
            mainWindow->setAlwaysOnTop( true );
            mainWindow->setAlwaysOnTop( false );
        }
        else
        {
            mainWindow = nullptr;
        }
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        mainWindow = nullptr;
        //quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
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
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

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
            mApp.show();
        }

    private:
        DonTouchMyMenuApplication& mApp;
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<MySystemTrayIconComponent> myTrayIcon;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (DonTouchMyMenuApplication)
