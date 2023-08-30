#pragma once

namespace sauna {

class Exciter {
public:
    Exciter();
    ~Exciter();
    
    // No copy and no move
    Exciter(const Exciter&) = delete;
    Exciter(Exciter&&) = delete;
    const Exciter& operator=(const Exciter&) = delete;
    const Exciter& operator=(Exciter&&) = delete;
    
    void prepare(double sampleRate);
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);
    
private:
    double sampleRate {1.0};
};
} // sauna namespace
