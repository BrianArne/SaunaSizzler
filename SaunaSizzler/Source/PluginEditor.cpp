/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SaunaSizzlerAudioProcessorEditor::SaunaSizzlerAudioProcessorEditor (SaunaSizzlerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    // Add all UI components
    addAndMakeVisible(bigBoyDial);
    addAndMakeVisible(lfoDial);
    addAndMakeVisible(reverbDial);
    addAndMakeVisible(steamDial);
    addAndMakeVisible(smallLeftDial);
    addAndMakeVisible(smallRightDial);
    
    // Header only contains big boy dial
    header.addChildComponent(bigBoyDial);
    
    // Footer
    footer.addChildComponent(lfoDial);
    footer.addChildComponent(reverbDial);
    footer.addChildComponent(steamDial);
    footer.addChildComponent(smallLeftDial);
    footer.addChildComponent(smallRightDial);

    addAndMakeVisible(header);
    addAndMakeVisible(footer);
    
    addAndMakeVisible(magicButton);
    setSize (720, 405);
    
    magicButton.addListener(this);
    updateUIMode();
    
    // Attach components
    saturatorPreGainDecibelsSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,
                                                                                                                      "SATURATOR_PREGAINDB", smallLeftDial);
    
}

SaunaSizzlerAudioProcessorEditor::~SaunaSizzlerAudioProcessorEditor()
{
}

//==============================================================================
void SaunaSizzlerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    g.setOpacity(0.4);
    auto saunaBackground = juce::ImageCache::getFromMemory(BinaryData::saunaBackground2_jpg, BinaryData::saunaBackground2_jpgSize);
    g.drawImageWithin(saunaBackground, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit, false);
}

void SaunaSizzlerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();
    
    // We divide window into half, top and bottom
    const auto halfHeight = getHeight() / 2;
    header.setBounds (area.removeFromTop    (halfHeight));
    footer.setBounds (area.removeFromBottom (halfHeight));
    
    // Positioning header elements
    bigBoyDial.centreWithSize(bigBoyDial.getParentHeight() * 0.9, bigBoyDial.getParentHeight() * 0.9);
    
    // Positioning footer elements
    auto footerArea = footer.getLocalBounds();
    const auto contentItemWidth = lfoDial.getParentWidth() / 4;

    lfoDial.setBounds(footerArea.removeFromLeft (contentItemWidth));
    reverbDial.setBounds(footerArea.removeFromLeft (contentItemWidth));
    steamDial.setBounds(footerArea.removeFromLeft (contentItemWidth));
    
    const auto smallKnobHeight = contentItemWidth / 2;
    smallLeftDial.setBounds(getWidth() - contentItemWidth, (footerArea.getHeight() - contentItemWidth) / 2, contentItemWidth / 2, contentItemWidth / 2);
    smallRightDial.setBounds(getWidth() - contentItemWidth / 2,
                             footerArea.getHeight() / 2 - (footerArea.getHeight() / 2 - contentItemWidth / 2) / 2,
                             smallKnobHeight, smallKnobHeight);
    
    magicButton.setSize(150, 50);
    updateUIMode();
}

void SaunaSizzlerAudioProcessorEditor::updateUIMode()
{
    header.setVisible(!advancedModeEnabled);
    footer.setVisible(advancedModeEnabled);
    
    if (advancedModeEnabled)
    {
        magicButton.setBounds(getWidth() / 2 - magicButton.getWidth() / 2,                          // x
                              header.getY() + header.getHeight() / 2 - magicButton.getHeight() / 2, // y
                              200, 45);                                                             // width and height
        magicButton.setButtonText("Bring back the big knob!");
    }
    else
    {
        magicButton.setBounds(getWidth() / 2 - magicButton.getWidth() / 2,                          // x
                              footer.getY() + footer.getHeight() / 2 - magicButton.getHeight() / 2, // y
                              200, 45);                                                             // width and height
        magicButton.setButtonText("Not enough controls?");
    }
}

void SaunaSizzlerAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &magicButton) {
        advancedModeEnabled = !advancedModeEnabled;
        updateUIMode();
    }
}
