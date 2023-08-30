/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 sauna_exciter
  vendor:             Sauna Audio Oy
  version:            0.0.1
  name:               Sauna Exciter
  description:        Exciter classes.
  license:            GPL
  minimumCppStandard: 17

 END_JUCE_MODULE_DECLARATION
*/

#pragma once
#include <algorithm>

namespace sauna {

class Exciter {
public:
    Exciter();
    ~Exciter();
    
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
                output[channel][sample] = 1.0 * input[channel][number];
            }
        }
    }
    
private:
    double sampleRate {1.0};
};
} // end sauna namespace
