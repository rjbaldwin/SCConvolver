#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SCConvolverAudioProcessorEditor::SCConvolverAudioProcessorEditor (SCConvolverAudioProcessor& p)
    : AudioProcessorEditor (&p), 
    audioProcessor (p)
{
    // load button
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Load IR");
    loadButton.onClick = [this]()
        {
            fileBrowser(fileChooser);
        };


    // gain slider
    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
   
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 200, 25);
    gainSlider.setValue(-10.0f);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setRange(-48.0f, 0.0f);
    gainSlider.setNumDecimalPlacesToDisplay(1);
    gainSlider.addListener(this);
    gainSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.treeState, "GAIN", gainSlider);


    // gain label
    addAndMakeVisible(gainLabel);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    juce::Font gainfont = gainLabel.getFont();
    gainfont.setHeight(20.0f);
    gainfont.setBold(true);
    gainLabel.setFont(gainfont);
    gainLabel.setJustificationType(juce::Justification::centred);


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

    // gain slider
    gainSlider.setBounds(50, 300, 80, 80);
}


//==============================================================================

// Filechooser function to simplify button click lambda.  Use this in other projects for simplicity

void SCConvolverAudioProcessorEditor::fileBrowser(std::unique_ptr<juce::FileChooser>& fileChooser)
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

}

void SCConvolverAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &gainSlider)
    {
        audioProcessor.rawVolume = pow(10, gainSlider.getValue() / 20);
        DBG("Raw Volume: " << audioProcessor.rawVolume);
    }
}


