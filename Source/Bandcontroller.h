#pragma once

#include <JuceHeader.h>
#include "Filter.h"
#include "Optionbox.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void updateFilter(int bandIndex);
    void showOptionBox();

    // Filter 및 옵션 관리 클래스
    Filter filter;
    Optionbox optionBox;

    // UI 요소들
    juce::TextButton optionBoxButton;
    juce::Slider frequencySlider, qSlider, gainSlider;
    juce::ComboBox filterTypeComboBox;
    juce::Label filterTypeLabel, frequencyLabel, qLabel, gainLabel;

    static constexpr int numBands = 6;
    juce::TextButton bandButtons[numBands];
    int selectedBandIndex = 0; // 현재 선택된 밴드 인덱스

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

