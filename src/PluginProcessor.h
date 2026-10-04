#pragma once
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

class GainExampleProcessor : public juce::AudioProcessor {
public:
    GainExampleProcessor()
        : AudioProcessor(BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

    using AudioProcessor::processBlock;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "freq", 1 },          // param ID, version hint
            "Freq",                                     // human-readable name
            juce::NormalisableRange<float>(800.0f, 6400.0f, 0.1f),  // hertz
            2000.0f));                                    // default: unity
        return layout;
    }

    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& l) const override {
        return l.getMainOutputChannelSet() == l.getMainInputChannelSet();
    }

    void prepareToPlay(double sampleRate_, int samplesPerBlock) override {
        //define metadata for the process
        sampleRate = sampleRate_;
        juce::dsp::ProcessSpec spec;
        spec.maximumBlockSize = juce::uint32(samplesPerBlock);
        spec.sampleRate = sampleRate;
        spec.numChannels = 2;

        filterL.prepare(spec);
        filterR.prepare(spec);
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        auto freq = apvts.getRawParameterValue("freq")->load();

        if(std::abs(lastFreq - freq) > 0.5f) {
            auto coeffs  = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq, 0.707f);
            filterL.coefficients = coeffs;
            filterR.coefficients = coeffs;
            lastFreq = freq;
        }

        auto* L = buffer.getWritePointer(0);
        auto* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            L[i] = filterL.processSample(L[i]);
        }
        if (R != nullptr)
            for (int i = 0; i < buffer.getNumSamples(); ++i) {
                R[i] = filterR.processSample(R[i]);
            }

    }

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "GainExample"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock& dest) override {
        juce::MemoryOutputStream mo(dest, false);
        apvts.state.writeToStream(mo);
    }
    
    void setStateInformation(const void* data, int size) override {
        auto tree = juce::ValueTree::readFromData(data, size_t(size));
        if (tree.isValid())
        apvts.replaceState(tree);
    }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::dsp::IIR::Filter<float> filterL, filterR;
    double sampleRate = 44100.0;
    float lastFreq = -1.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainExampleProcessor)
    
};