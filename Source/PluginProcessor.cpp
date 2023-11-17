#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SCConvolverAudioProcessor::SCConvolverAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    treeState{*this, nullptr,"PARAMETERS",createParameterLayout()}
#endif


{
    treeState.addParameterListener("GAIN", this);
}

SCConvolverAudioProcessor::~SCConvolverAudioProcessor()
{
    treeState.removeParameterListener("GAIN", this);
}



//==============================================================================
const juce::String SCConvolverAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SCConvolverAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SCConvolverAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SCConvolverAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SCConvolverAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SCConvolverAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SCConvolverAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SCConvolverAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SCConvolverAudioProcessor::getProgramName (int index)
{
    return {};
}

void SCConvolverAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SCConvolverAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // initialise spec for DSP modules

    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    irLoader.reset();
    irLoader.prepare(spec);
}

void SCConvolverAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SCConvolverAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

// ****************************************************************************
// **********************PROCESS BLOCK ****************************************
// ****************************************************************************

void SCConvolverAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::dsp::AudioBlock<float>block{ buffer };

    if (irLoader.getCurrentIRSize() > 0)
    {
        irLoader.process(juce::dsp::ProcessContextReplacing<float>(block));
    }

    // for output gain
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* data = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            auto input = data[sample];
            float output;

            data[sample] = buffer.getSample(channel, sample) * rawVolume;

        }
    }
}

//==============================================================================
bool SCConvolverAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SCConvolverAudioProcessor::createEditor()
{
    return new SCConvolverAudioProcessorEditor (*this);
}

//==============================================================================
void SCConvolverAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SCConvolverAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(treeState.state.getType()))
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout SCConvolverAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto gainParam = std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", -48.0f, 0.0f, -10.0f);
    params.push_back(std::move(gainParam));

    return { params.begin(), params.end() };

}

void SCConvolverAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SCConvolverAudioProcessor();
}
