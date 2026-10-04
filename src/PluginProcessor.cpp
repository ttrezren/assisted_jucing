#include "PluginProcessor.h"

juce::AudioProcessorEditor* GainExampleProcessor::createEditor() {
    struct GainEditor : juce::AudioProcessorEditor {
        explicit GainEditor(GainExampleProcessor& p) : AudioProcessorEditor(p), proc(p) {
            slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
            slider.setRange(-24.0, 24.0, 0.1);
            addAndMakeVisible(slider);
            attachment = std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                    proc.apvts, "gain", slider);
            setSize(300, 300);
        }
        void resized() override { slider.setBounds(50, 50, 200, 200); }
        void paint(juce::Graphics& g) override { g.fillAll(juce::Colours::darkgrey); }

        GainExampleProcessor& proc;
        juce::Slider slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    return new GainEditor(*this);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new GainExampleProcessor();
}