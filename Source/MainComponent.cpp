#include "MainComponent.h"
#include "reg_op.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);
    startTimer( 1000 );// *60 * 10 );
    timerCallback();
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll( Colours::white );
    g.setFont( juce::Font( 9.0f ) );
    g.setColour( juce::Colours::black );
    //g.drawText( x.joinIntoString( "\n\t" ) , getLocalBounds() , juce::Justification::centred , true );
    g.drawMultiLineText( x.joinIntoString( " \n\t" ) , 0 , 10 , getWidth() );
}

void MainComponent::resized()
{}

void MainComponent::timerCallback()
{
    x = readConfigFile();
    repaint();
}
