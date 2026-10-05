#pragma once
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors_headless/juce_audio_processors_headless.h"
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

class TwoPoleLowPassProcessor : public juce::AudioProcessor {
public:
    TwoPoleLowPassProcessor()
        : AudioProcessor(BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

    using AudioProcessor::processBlock;
    using AudioProcessor::processBlockBypassed;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "cutoff", 1 },          // param ID, version hint
            "Cutoff Hz",                                     // human-readable name
            juce::NormalisableRange<float>(80.0f, 6400.0f, 0.1f, 0.5f),  // hertz
            2000.0f));                                    // default: unity
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID {"q", 1},
            "Q",
            juce::NormalisableRange<float>(0.1f, 2.0f, 0.01f, 0.4f),
            0.797f));
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

        wetMix.reset(sampleRate, 0.02);
        wetMix.setCurrentAndTargetValue(1.0f);
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        wetMix.setTargetValue(1.0f);
        processCommon(buffer);
    }

    void processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        wetMix.setTargetValue(0.0f);
        processCommon(buffer);
    }

    void processCommon(juce::AudioBuffer<float>& buffer) {
        auto cutoff = apvts.getRawParameterValue("cutoff")->load();      
        auto q      = apvts.getRawParameterValue("q")->load();
        if(std::abs(lastCutoff - cutoff) > 0.5f || std::abs(lastQ - q) > 0.02f) {
            auto coeffs  = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoff, q);
            filterL.coefficients = coeffs;
            filterR.coefficients = coeffs;
            lastCutoff = cutoff;
            lastQ = q;
        }

        auto* L = buffer.getWritePointer(0);
        auto* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            auto w = wetMix.getNextValue();
            L[i] = L[i] * (1.0f - w) + filterL.processSample(L[i]) * w;
            if (R != nullptr)
                    R[i] = R[i] * (1.0f - w) + filterR.processSample(R[i]) * w;
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
    juce::SmoothedValue<float> wetMix {1.0f};
    double sampleRate = 44100.0;
    float lastCutoff = -1.0f;
    float lastQ = 0.707f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TwoPoleLowPassProcessor)
    
};