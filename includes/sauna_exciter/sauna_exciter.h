/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 sauna_exciter
  vendor:             Sauna Audio Oy
  version:            0.0.1
  name:               Sauna Exciter
  description:        Exciter classes.
  license:            GPL
  minimumCppStandard: 17
  dependencies:       juce_audio_basics, juce_dsp

 END_JUCE_MODULE_DECLARATION
*/

#pragma once
#include <algorithm>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace sauna {

class Saturator {
public:
    Saturator(): preGain {juce::Decibels::decibelsToGain(6.0f)}, tubeQ {-0.2f}, tubeDist {8.0f} {
        setSaturation(SaturationType::Tube);
    };
    
    ~Saturator() {};
    
    // This enum is the only thing that is necessary to hook up to the front end
    enum SaturationType {
        Tanh,
        ASinh,
        HardClipping,
        SoftClipping,
        Tube
    };
    
    // No copy semantics
    Saturator(const Saturator&) = delete;
    const Saturator& operator=(const Saturator&) = delete;
    
    // No move semantics
    Saturator(Saturator&&) = delete;
    const Saturator& operator=(Saturator&&) = delete;
    
    void setSaturation(SaturationType type) {
        if (type == SaturationType::Tanh) {
            saturation = [this] (float x) {return applyTanh(x);};
        }
        
        else if (type == SaturationType::ASinh) {
            saturation = [this] (float x) {return applyASinh(x);};
        }
        
        else if (type == SaturationType::HardClipping) {
            saturation = [this] (float x) {return applyHardClipping(x);};
        }
        
        else if (type == SaturationType::SoftClipping) {
            saturation = [this] (float x) {return applySoftClipping(x);};
        }
        
        else if (type == SaturationType::Tube) {
            saturation = [this] (float x) {return applyTubeSaturator(x);};
        }
        
        else {
            // If you hit this assertion is because you selected an invalid saturation type
            jassert(false);
        }
    }
    
    float applyTanh(float x) {
        return std::tanhf(x);
    }
    
    float applyASinh(float x) {
        return std::asinhf(x);
    }
    
    float applySoftClipping(float x){
        if (x > 1.0f) {
            return 2.0f / 3.0f;
        }
        
        if (x < -1.0f) {
            return -2.0f / 3.0f;
        }
        
        return x - (x * x * x) / 3.0f;
    }
    
    float applyHardClipping(float x) {
        if (x > 1.0f) {
            return 1.0f;
        }
        
        if (x < -1.0f) {
            return -1.0f;
        }
        
        return x;
    }
    
    float applyTubeSaturator(float x) {
        if (x == tubeQ) {
            return (1.0f / tubeDist) + (tubeQ / (1.0f - expf(tubeDist * tubeQ)));
        } else {
            return ((x - tubeQ) / (1.0f - (expf(-1.0f * tubeDist * (x - tubeQ))))) + (tubeQ / (1.0f - expf(tubeDist * tubeQ)));
        }
    }

    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples) {
        
        // To avoid using it in more than 2 channels
        numChannels = std::min(numChannels, 2u);
        
        for (unsigned int channel = 0; channel < numChannels; channel++)
        {
            for (unsigned int sample = 0; sample < numSamples; sample++)
            {
                // Unity gain for testing
                output[channel][sample] = saturation(preGain * input[channel][sample]);
            }
        }
    }
    
private:
    float preGain;
    float tubeQ;
    float tubeDist;
    std::function<float(float)> saturation;
};


class SaturatorProcessor: public juce::dsp::ProcessorBase
{
public:
    SaturatorProcessor(): saturator() {};
    ~SaturatorProcessor() {};
    
    void prepare(const juce::dsp::ProcessSpec& spec) override {
    }
    
    void reset() override {
        
    }
    
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override {
        // Get input blocks
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        
        const auto inputNumChannels = inputBlock.getNumChannels();
        const auto outputNumChannels = outputBlock.getNumChannels();
        
        const float* input[inputNumChannels];
        
        // Get channel pointers
        for (size_t i = 0; i < inputNumChannels; i++) {
            input[i] = inputBlock.getChannelPointer(i);
        }
        
        float* output[outputNumChannels];
        
        for (size_t i = 0; i < outputNumChannels; i++) {
            output[i] = outputBlock.getChannelPointer(i);
        }
        
        // Do processing
        saturator.process(output, input, static_cast<unsigned int>(inputNumChannels), static_cast<unsigned int>(inputBlock.getNumSamples()));
        
    }
    
private:
    Saturator saturator;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaturatorProcessor)
};


class Steamer {
public:
    Steamer() {}
    ~Steamer() {}
    
    // No copy semantics
    Steamer(const Steamer&) = delete;
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
