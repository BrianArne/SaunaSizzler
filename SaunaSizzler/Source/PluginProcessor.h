/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <sauna_exciter/sauna_exciter.h>

//==============================================================================
/**
*/

class SaunaSizzlerAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SaunaSizzlerAudioProcessor();
    ~SaunaSizzlerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void updatePhaseIncrement (float modRate);
    
    juce::AudioProcessorValueTreeState apvts;

private:
    enum ProcessorIndex
    {
        saturatorIndex = 0,
        steamerIndex,
        reverbIndex
    };
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
    sauna::Saturator saturator;
    sauna::SteamerReverb steamerReverb;
    sauna::Steamer steamer;

    // LFO states
    float phaseState[2] { 0.f, 0.f };
    float phaseInc { 0.f };
    float modRate { 100.0f };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaunaSizzlerAudioProcessor)
};
