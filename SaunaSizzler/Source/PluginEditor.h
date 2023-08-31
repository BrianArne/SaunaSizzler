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
{
public:
    SaunaSizzlerAudioProcessorEditor (SaunaSizzlerAudioProcessor&);
    ~SaunaSizzlerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SaunaSizzlerAudioProcessor& audioProcessor;
    
    juce::Component header;
    BigBoyDial bigBoyDial;
    
    juce::Component footer;
    StandardDial lfoDial;
    StandardDial reverbDial;
    StandardDial steamDial;
    
    StandardDial smallLeftDial;
    StandardDial smallRightDial;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaunaSizzlerAudioProcessorEditor)
};
