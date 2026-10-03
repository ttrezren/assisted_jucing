cmake_minimum_required(VERSION 3.22)
project(RealtimeDSP VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(JUCE)

juce_add_plugin(GainExample
    COMPANY_NAME "TimRenner"
    PLUGIN_MANUFACTURER_CODE Yxyz
    PLUGIN_CODE Gain
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    FORMATS VST3 Standalone
    PRODUCT_NAME "GainExample")

target_sources(GainExample PRIVATE
    src/PluginProcessor.cpp
    src/PluginProcessor.h)

target_compile_definitions(GainExample PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(GainExample
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

