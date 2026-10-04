#include "PluginProcessor.h"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"
#include <juce_dsp/juce_dsp.h>

juce::AudioProcessorEditor* TwoPoleLowPassProcessor::createEditor() {
    struct GainEditor : juce::AudioProcessorEditor {
        explicit GainEditor(TwoPoleLowPassProcessor& p) : AudioProcessorEditor(p), proc(p) {
            titleLabel.setText("Cutoff Hz", juce::dontSendNotification);
            titleLabel.setJustificationType(juce::Justification::centred);
            titleLabel.setFont(juce::Font(16.0f).withExtraKerningFactor(0.0f));
            addAndMakeVisible(titleLabel);
            
            slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
            slider.setRange(800.0, 6400.0, 0.1);
            slider.setSkewFactorFromMidPoint(1600.0);
            addAndMakeVisible(slider);
            attachment = std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                    proc.apvts, "freq", slider);
            setSize(300, 300);
        }
        void resized() override { 
            titleLabel.setBounds(50,25,200,25);
            slider.setBounds(50, 50, 200, 200); 
        }
        void paint(juce::Graphics& g) override { 
            g.fillAll(juce::Colours::darkgrey); 
        }

        TwoPoleLowPassProcessor& proc;
        juce::Label titleLabel;
        juce::Slider slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    return new GainEditor(*this);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new TwoPoleLowPassProcessor();
}