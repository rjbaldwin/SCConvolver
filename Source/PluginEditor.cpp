/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SCConvolverAudioProcessorEditor::SCConvolverAudioProcessorEditor (SCConvolverAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // load button
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Load IR");
    loadButton.onClick = [this]()
        {
            fileChooser = std::make_unique<juce::FileChooser>("Choose IR",
                                                                audioProcessor.root,
                                                                "*");

            const auto fileChooserFlags = juce::FileBrowserComponent::openMode | 
                                          juce::FileBrowserComponent::canSelectFiles | 
                                          juce::FileBrowserComponent::canSelectDirectories;
            fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
            {
                    juce::File result(chooser.getResult());

                    if (result.getFileExtension() == ".wav" || result.getFileExtension() == ".mp3")
                    {
                        audioProcessor.savedFile = result;
                        audioProcessor.root = result.getParentDirectory().getFullPathName();
                        audioProcessor.irLoader.reset();
                        audioProcessor.irLoader.loadImpulseResponse(audioProcessor.savedFile,
                                                                    juce::dsp::Convolution::Stereo::yes,
                                                                    juce::dsp::Convolution::Trim::yes,
                                                                    0);
                    }

            });
        };

    setSize (600, 400);
}

SCConvolverAudioProcessorEditor::~SCConvolverAudioProcessorEditor()
{
}

//==============================================================================
void SCConvolverAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    
}

void SCConvolverAudioProcessorEditor::resized()
{
   
    // load button 
    loadButton.setBounds(20, 20, 100, 50);
}
