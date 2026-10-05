#include "PluginProcessor.h"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"
#include <juce_dsp/juce_dsp.h>

juce::AudioProcessorEditor* TwoPoleLowPassProcessor::createEditor() {
    struct GainEditor : juce::AudioProcessorEditor {
        explicit GainEditor(TwoPoleLowPassProcessor& p) : AudioProcessorEditor(p), proc(p) {
            titleLabel.setText("Cutoff Hz", juce::dontSendNotification);
            titleLabel.setJustificationType(juce::Justification::centred);
            titleLabel.setFont(juce::Font(juce::FontOptions(16.0f)).withExtraKerningFactor(0.0f));
            addAndMakeVisible(titleLabel);
            
            cutoffSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
            cutoffSlider.setRange(80.0, 6400.0, 0.1);
            cutoffSlider.setSkewFactorFromMidPoint(1600.0);
            addAndMakeVisible(cutoffSlider);
            
            qLabel.setText("Resonance", juce::dontSendNotification);
            qLabel.setJustificationType(juce::Justification::centred);
            qLabel.setFont(juce::Font(juce::FontOptions(16.0f)));
            addAndMakeVisible(qLabel);

            qSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
            qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
            qSlider.setRange(0.1, 2.0, 0.01);
            addAndMakeVisible(qSlider);

            cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, "cutoff", cutoffSlider);
            qAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, "q", qSlider);


            setSize(320, 400);
        }
        void resized() override { 
            titleLabel.setBounds(60,20,200,20);
            cutoffSlider.setBounds(60, 40, 200, 200);
            qLabel.setBounds(60,250,200,20);
            qSlider.setBounds(60,270,200,100); 
        }
        void paint(juce::Graphics& g) override { 
            g.fillAll(juce::Colours::darkgrey); 
        }

        TwoPoleLowPassProcessor& proc;
        juce::Label titleLabel, qLabel;
        juce::Slider cutoffSlider, qSlider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment, qAttachment;
    };
    return new GainEditor(*this);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new TwoPoleLowPassProcessor();
}