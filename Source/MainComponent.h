#pragma once

#include <JuceHeader.h>

using namespace juce;

class MainComponent  : public juce::Component
                     , public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;

    void resized() override;

private:
    StringArray x;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
