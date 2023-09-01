/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 sauna_exciter
  vendor:             Sauna Audio Oy
  version:            0.0.1
  name:               Sauna Exciter
  description:        Exciter classes.
  license:            GPL
  minimumCppStandard: 17
  dependencies:       juce_dsp

 END_JUCE_MODULE_DECLARATION
*/

#pragma once
#include <algorithm>
#include <juce_dsp/juce_dsp.h>

namespace sauna {

class ExciterProcessor: public juce::dsp::ProcessorBase
{
public:
    ExciterProcessor() {};
    ~ExciterProcessor() {};
    
    void prepare(const juce::dsp::ProcessSpec& spec) override
    {
        // Prepare specs here
    }
    
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override
    {
        // Do processing here
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();
        
        if (context.isBypassed)
        {
            outputBlock.copyFrom(inputBlock);
            return;
        }
        
        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (size_t i = 0; i < numSamples; ++i)
            {
                outputSamples[i] = 1.0 * inputSamples[i];
            }
        }
        
    }
    
    void reset() override {
        
    }
    
private:
    
};

class Exciter {
public:
    Exciter() {};
    ~Exciter() {};
    
    // No copy semantics
    Exciter(const Exciter&) = delete;
    const Exciter& operator=(const Exciter&) = delete;
    
    // No move semantics
    Exciter(Exciter&&) = delete;
    const Exciter& operator=(Exciter&&) = delete;
    
    void prepare(double newSampleRate) {
        sampleRate = newSampleRate;
    }

    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples) {
        
        // To avoid using it in more than 2 channels
        numChannels = std::min(numChannels, 2u);
        
        for (unsigned int channel = 0; channel < numChannels; channel++)
        {
            for (unsigned int sample = 0; sample < numSamples; sample++)
            {
                // Unity gain for testing
                output[channel][sample] = 1.0 * input[channel][sample];
            }
        }
    }
    
private:
    double sampleRate {1.0};
};

class Steamer {
public:
    Steamer() {}
    ~Steamer() {}
    
    // No copy semantics
    Steamer(const Exciter&) = delete;
    const Steamer& operator=(const Steamer&) = delete;
    
    // No move semantics
    Steamer(Steamer&&) = delete;
    const Steamer& operator=(Steamer&&) = delete;
    
    void prepare() { }

    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples) {
        
        // To avoid using it in more than 2 channels
        numChannels = std::min(numChannels, 2u);
        
        for (unsigned int channel = 0; channel < numChannels; channel++)
        {
            for (unsigned int sample = 0; sample < numSamples; sample++)
            {
                // Unity gain for testing
                output[channel][sample] += random.nextFloat() * 0.25f - 0.125f;
            }
        }
    }
    
    void process(float*  left,
                 float*  right,
                 const float*  modInput,
                 unsigned int numChannels,
                 unsigned int numSamples)
    {
        // To avoid using it in more than 2 channels
        numChannels = std::min(numChannels, 2u);
        *left += random.nextFloat() * 0.25f - 0.125f;
        *right += random.nextFloat() * 0.25f - 0.125f;
    }
    
private:
    juce::Random random;
};


class SteamerProcessor : public juce::dsp::ProcessorBase {
public:
    SteamerProcessor() {};
    ~SteamerProcessor() {};
    
    void prepare(const juce::dsp::ProcessSpec& spec) override
    {
    }
    
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override
    {
        // Do processing here
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        
        const auto inputNumChannels = inputBlock.getNumChannels();
        const auto outputNumChannels = outputBlock.getNumChannels();
        
        
        const float* input[inputNumChannels];
        for (int i = 0; i < inputBlock.getNumChannels(); i++){
            input[i] = inputBlock.getChannelPointer(i);
        }
        
        float* output[outputNumChannels];
        for (int i = 0; i < outputBlock.getNumChannels(); i++){
            output[i] = outputBlock.getChannelPointer(i);
        }
        
        steamer.process(output, input, static_cast<unsigned int>(inputBlock.getNumChannels()), static_cast<unsigned int>(inputBlock.getNumSamples()));
    }
    
    void reset() override {
        
    }
    
private:
    Steamer steamer;
};


class SteamerReverb : public juce::Reverb
{
public:
    SteamerReverb() : juce::Reverb() {};
    
    void process(float*  left, //readArray
                 float*  right, //writeArray
                 const float*  modInput,
                 unsigned int numChannels,
                 unsigned int numSamples)
    {
        processStereo (left, right, numSamples);
    }
private:
};

} // end sauna namespace
