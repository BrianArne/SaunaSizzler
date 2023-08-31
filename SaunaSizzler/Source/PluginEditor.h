/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "Widgets/Dials.h"

//==============================================================================
/**
*/
class SaunaSizzlerAudioProcessorEditor  : public juce::AudioProcessorEditor
                                        , public juce::Button::Listener
{
public:
    SaunaSizzlerAudioProcessorEditor (SaunaSizzlerAudioProcessor&);
    ~SaunaSizzlerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void buttonClicked (juce::Button* button) override;

private:
    void updateUIMode();
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SaunaSizzlerAudioProcessor& audioProcessor;
    
    juce::Component header;
    BigBoyDial bigBoyDial;
    
    juce::TextButton magicButton;
    
    juce::Component footer;
    StandardDial lfoDial { "LFO" };
    StandardDial reverbDial { "Reverb" };
    StandardDial steamDial { "Steam" };
    
    StandardDial smallLeftDial;
    StandardDial smallRightDial;
    
    bool advancedModeEnabled { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaunaSizzlerAudioProcessorEditor)
};
