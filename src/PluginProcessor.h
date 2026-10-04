#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class GainExampleProcessor : public juce::AudioProcessor {
public:
    GainExampleProcessor()
        : AudioProcessor(BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "gain", 1 },          // param ID, version hint
            "Gain",                                     // human-readable name
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),  // dB
            0.0f));                                    // default: unity
        return layout;
    }

    void prepareToPlay(double sampleRate, int) override {
        gainSmoother.reset(sampleRate, 0.02);   // 20 ms ramp
        gainSmoother.setCurrentAndTargetValue(getDbToGain(apvts.getRawParameterValue("gain")->load()));
    }

    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& l) const override {
        return l.getMainOutputChannelSet() == l.getMainInputChannelSet();
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        // Set ramp target from the parameter (atomic read, realtime-safe)
        gainSmoother.setTargetValue(getDbToGain(apvts.getRawParameterValue("gain")->load()));

        auto* L = buffer.getWritePointer(0);
        auto* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            auto g = gainSmoother.getNextValue();   // advance once per sample
            L[i] *= g;
            if (R != nullptr) R[i] *= g;            // same factor both channels
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

    juce::AudioProcessorValueTreeState apvts;

private:
    static float getDbToGain(float db) { return juce::Decibels::decibelsToGain(db); }
    juce::SmoothedValue<float> gainSmoother { 1.0f };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainExampleProcessor)
    
};