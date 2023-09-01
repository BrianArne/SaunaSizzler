/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SaunaSizzlerAudioProcessor::SaunaSizzlerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), exciter()
#else
: exciter()
#endif
{
}

SaunaSizzlerAudioProcessor::~SaunaSizzlerAudioProcessor()
{
}

//==============================================================================
const juce::String SaunaSizzlerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SaunaSizzlerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SaunaSizzlerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SaunaSizzlerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SaunaSizzlerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SaunaSizzlerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SaunaSizzlerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SaunaSizzlerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SaunaSizzlerAudioProcessor::getProgramName (int index)
{
    return {};
}

void SaunaSizzlerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SaunaSizzlerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    exciter.prepare(sampleRate);
    
    // Prepare specs
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    
    // Testing Params
    juce::Reverb::Parameters reverbParams{0.3f, 0.5f, 0.5f, 0.4f, 1.0f, 0.0f};
    steamerReverb.setParameters(reverbParams);
    
    // LFO Initilization
    phaseState[0] = 0.f;
    phaseState[1] = static_cast<float>(M_PI / 2.0);
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
    
    // Prepare processors
    exciter.prepare(sampleRate);
    steamerReverb.setSampleRate(sampleRate);
    steamerReverb.reset();
    steamer.prepare();
}

void SaunaSizzlerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SaunaSizzlerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SaunaSizzlerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    //audio buffer has the input that should be replaced by the output
    
    juce::ScopedNoDenormals noDenormals;
    
    // LFO MOD
    float lfo[2] { 0.f, 0.f };
    lfo[0] = 0.5f + 0.5f * std::sin(phaseState[0]);
    lfo[1] = 0.5f + 0.5f * std::sin(phaseState[1]);

    // Increment and wrap phase states
    phaseState[0] = std::fmod(phaseState[0] + phaseInc, static_cast<float>(2 * M_PI));
    phaseState[1] = std::fmod(phaseState[1] + phaseInc, static_cast<float>(2 * M_PI));
    
    // Process single samples through each processor
    // Audio buffer has the input that should be replaced by the output
    for (int i = 0; i < buffer.getNumSamples(); i++){
        //exciter.process()
        
        auto write = buffer.getArrayOfWritePointers(); // Pointer to a Pointer
        auto read = buffer.getArrayOfReadPointers(); // Pointer to a Pointer
        float* writeSampleArrayLeft = write[0];
        float* writeSampleArrayRight = write[1];
        float* x[2];
        x[0] = writeSampleArrayLeft;
        x[1] = writeSampleArrayRight;
        
        exiter.process(&(x[0][i]), &(x[1][i]), phaseState, buffer.getNumChannels(), 1);
        steamerReverb.process(&(x[0][i]), &(x[1][i]), phaseState, buffer.getNumChannels(), 1);
        steamer.process(&(x[0][i]), &(x[1][i]), phaseState, buffer.getNumChannels(), 1);
    }
}

//==============================================================================
bool SaunaSizzlerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SaunaSizzlerAudioProcessor::createEditor()
{
    return new SaunaSizzlerAudioProcessorEditor (*this);
}

//==============================================================================
void SaunaSizzlerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SaunaSizzlerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SaunaSizzlerAudioProcessor();
}
