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
                       ), apvts(*this, nullptr, "params", createParameters())
#endif
: apvts(*this, nullptr, "params", createParameters())
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
    // Testing Params
    juce::Reverb::Parameters reverbParams{0.5f, 0.5f, 0.5f, 0.4f, 1.0f, 0.0f};
    steamerReverb.setParameters(reverbParams);
    
    // LFO Initilization
    phaseState[0] = 0.f;
    phaseState[1] = static_cast<float>(M_PI / 2.0);
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
    
    // Prepare processors
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
    
   // Update parameters
   auto saturatorPreGainDecibels = apvts.getRawParameterValue("SATURATOR_PREGAINDB");
   // auto& saturatorBlock = chain.get<0>();
   saturator.setPreGain(saturatorPreGainDecibels->load());
   
   auto saturatorType = apvts.getRawParameterValue("SATURATOR_TYPE");
   saturator.setSaturation(static_cast<sauna::Saturator::SaturationType>(saturatorType->load()));
    
    auto steamerGainDecibels = apvts.getRawParameterValue("STEAMER_GAINDB");
    steamer.setGain(steamerGainDecibels->load());
    
    auto reverbRoomSize = apvts.getRawParameterValue("REVERB_ROOMSIZE");
    auto steamerReverbParams = steamerReverb.getParameters();
    steamerReverbParams.roomSize = reverbRoomSize->load();
    steamerReverb.setParameters(steamerReverbParams);
    
    auto lfoRate = apvts.getRawParameterValue("LFO_RATE");
    updatePhaseIncrement(lfoRate->load());
    
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
        // We process in place, so we only need 1 sample frame of write pointers
        auto write = buffer.getArrayOfWritePointers();
        float* writeSampleArrayLeft = write[0];
        float* writeSampleArrayRight = write[1];
        float* x[2] {writeSampleArrayLeft, writeSampleArrayRight};
        x[0] = writeSampleArrayLeft;
        x[1] = writeSampleArrayRight;
        
        int left = 0;
        int right = 1;
        if (buffer.getNumChannels() == 1) {
            right = 0;
        }
        
        
        steamer.process(&(x[left][i]), &(x[right][i]), lfo, buffer.getNumChannels(), 1);
        steamerReverb.process(&(x[left][i]), &(x[right][i]), lfo, buffer.getNumChannels(), 1);
        saturator.process(&(x[left][i]), &(x[right][i]), lfo, buffer.getNumChannels(), 1);
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

void SaunaSizzlerAudioProcessor::updatePhaseIncrement(float modRate) {
    double sampleRate = getSampleRate();
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
}

juce::AudioProcessorValueTreeState::ParameterLayout SaunaSizzlerAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    // preGaindB
    params.add(std::make_unique<juce::AudioParameterFloat>("SATURATOR_PREGAINDB",
                                                           "PreGain dB",
                                                           0.0f,
                                                           12.0f,
                                                           6.0f));
    
    // Saturation type
    juce::StringArray saturatorTypes;
    saturatorTypes.add("Tanh");
    saturatorTypes.add("ASinh");
    saturatorTypes.add("HardClipping");
    saturatorTypes.add("SoftClipping");
    saturatorTypes.add("Tube");
    params.add(std::make_unique<juce::AudioParameterChoice>("SATURATOR_TYPE",
                                                            "Saturator Type",
                                                            saturatorTypes,
                                                            4));
    
    // Steamer gain
    params.add(std::make_unique<juce::AudioParameterFloat>("STEAMER_GAINDB",
                                                           "Steamer Gain dB",
                                                           juce::NormalisableRange<float>(-70.0f, 24.0f, 0.5f, 1.5f),
                                                           -40.0f));
    
    // Reverb room size
    params.add(std::make_unique<juce::AudioParameterFloat>("REVERB_ROOMSIZE",
                                                           "Reverb Room Size",
                                                           0.0f,
                                                           1.0f,
                                                           0.5f));
    
    // LFO rate
    params.add(std::make_unique<juce::AudioParameterFloat>("LFO_RATE",
                                                           "LFO Rate",
                                                           50.0f,
                                                           1000.0f,
                                                           100.0f));
    
    return params;
}
