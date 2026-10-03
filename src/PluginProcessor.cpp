#include "PluginProcessor.h"

juce::AudioProcessorEditor* GainExampleProcessor::createEditor() {
    struct EmptyEditor : juce::AudioProcessorEditor {
        explicit EmptyEditor(GainExampleProcessor& p) : AudioProcessorEditor(p) {
            setSize(300, 200);
        }
        void paint(juce::Graphics& g) override { g.fillAll(juce::Colours::darkgrey); }
    };
    return new EmptyEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new GainExampleProcessor();
}