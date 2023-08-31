/*
  ==============================================================================

    BigBoyDial.h
    Created: 31 Aug 2023 12:08:37pm
    Author:  Sercan Atlı

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class DialLookAndFeel: public juce::LookAndFeel_V4
{
public:
    DialLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colours::red);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour (juce::Colours::black);
        g.setOpacity(0.45);
        g.fillEllipse (rx, ry, rw, rw);

        g.setOpacity(1);
        // outline
        g.setColour (juce::Colours::grey);
        g.drawEllipse (rx, ry, rw, rw, 2.0f);

        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 6.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));

        // pointer
        g.setColour (juce::Colours::green);
        g.fillPath (p);
        
        g.setOpacity(0.3);
        g.setColour(juce::Colours::white);
        const auto imageSize = height * 0.3;
        const auto imageOrigin = (width / 2) - (imageSize / 2);
        const auto imageBucket = juce::ImageCache::getFromMemory(BinaryData::bucket_png, BinaryData::bucket_pngSize);
        g.drawImageWithin(imageBucket, imageOrigin, imageOrigin, imageSize, imageSize, juce::RectanglePlacement::stretchToFit, true);
    }
};


class StandardDial: public juce::Slider
{
public:
    StandardDial()
    {
        setSliderStyle (juce::Slider::RotaryVerticalDrag);
        setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    }
};



class BigBoyDial : public StandardDial
{
public:
    BigBoyDial()
    {
        setLookAndFeel(&dialLookAndFeel);
    }
    
    ~BigBoyDial()
    {
        setLookAndFeel(nullptr);
    }

private:
    DialLookAndFeel dialLookAndFeel;
};
