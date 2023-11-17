
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================

class SCConvolverAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    SCConvolverAudioProcessorEditor (SCConvolverAudioProcessor&);
    ~SCConvolverAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void fileBrowser(std::unique_ptr<juce::FileChooser>& fileChooser);
    void sliderValueChanged(juce::Slider* slider);

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SCConvolverAudioProcessor& audioProcessor;

    juce::TextButton loadButton;
    // unique pointer for filechooser
    std::unique_ptr<juce::FileChooser> fileChooser;

    // gainslider
    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainSliderAttachment;
    juce::Label gainLabel;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SCConvolverAudioProcessorEditor)
};
