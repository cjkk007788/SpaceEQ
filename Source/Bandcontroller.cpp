#include "Bandcontroller.h"

MainComponent::MainComponent()
    : optionBoxButton("Option Box")
{
    // Band Buttons
    for (int i = 0; i < numBands; ++i) {
        addAndMakeVisible(bandButtons[i]);
        bandButtons[i].setButtonText("Band " + juce::String(i + 1));
        bandButtons[i].onClick = [this, i] { selectedBandIndex = i; updateFilter(i); };
    }

    // Option Box Button
    addAndMakeVisible(optionBoxButton);
    optionBoxButton.onClick = [this] { showOptionBox(); };

    // Filter Controls
    addAndMakeVisible(frequencySlider);
    frequencySlider.setRange(20.0, 20000.0, 1.0);
    frequencySlider.setSkewFactorFromMidPoint(1000.0);
    frequencySlider.onValueChange = [this] { filter.setCutoffFrequency(frequencySlider.getValue(), selectedBandIndex); };
    
    addAndMakeVisible(qSlider);
    qSlider.setRange(0.1, 10.0, 0.1);
    qSlider.onValueChange = [this] { filter.setQ(qSlider.getValue(), selectedBandIndex); };
    
    addAndMakeVisible(gainSlider);
    gainSlider.setRange(-24.0, 24.0, 0.1);
    gainSlider.onValueChange = [this] { filter.setGain(gainSlider.getValue(), selectedBandIndex); };

    addAndMakeVisible(filterTypeComboBox);
    filterTypeComboBox.addItem("Low Pass", Filter::LowPass + 1);
    filterTypeComboBox.addItem("High Pass", Filter::HighPass + 1);
    filterTypeComboBox.addItem("Band Pass", Filter::BandPass + 1);
    filterTypeComboBox.addItem("Notch", Filter::Notch + 1);
    filterTypeComboBox.addItem("Peak", Filter::Peak + 1);
    filterTypeComboBox.addItem("Low Shelf", Filter::LowShelf + 1);
    filterTypeComboBox.addItem("High Shelf", Filter::HighShelf + 1);
    filterTypeComboBox.onChange = [this] {
        filter.setFilterType(static_cast<Filter::FilterType>(filterTypeComboBox.getSelectedId() - 1), selectedBandIndex);
    };

    addAndMakeVisible(filterTypeLabel);
    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    addAndMakeVisible(frequencyLabel);
    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    addAndMakeVisible(qLabel);
    qLabel.setText("Q Factor", juce::dontSendNotification);
    addAndMakeVisible(gainLabel);
    gainLabel.setText("Gain", juce::dontSendNotification);

    // Set audio channels
    setAudioChannels(2, 2);
    setSize(800, 600);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    filter.prepareToPlay(sampleRate, samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    filter.process(*bufferToFill.buffer);
    // 스펙트럼 분석을 여기서 수행할 수 있음
}

void MainComponent::releaseResources()
{
    // 리소스 해제
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);

    g.setColour(juce::Colours::black);
    g.drawText("Spectrum Analyzer", getLocalBounds().reduced(40), juce::Justification::centredBottom, true);
    g.setColour(juce::Colours::grey);
    juce::Line<float> line(0.0f, 200.0f, static_cast<float>(getWidth()), 200.0f);
    const float dashLengths[] = { 5.0f, 5.0f };
    g.drawDashedLine(line, dashLengths, 2);


    // 각 밴드 버튼의 위치 설정
    for (int i = 0; i < numBands; ++i) {
        bandButtons[i].setBounds(100 + i * 100, 100, 80, 80);
    }
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    optionBoxButton.setBounds(area.removeFromTop(40).removeFromRight(100).reduced(10));

    auto controlsArea = area.removeFromBottom(200).reduced(20);
    filterTypeComboBox.setBounds(controlsArea.removeFromTop(40));
    filterTypeLabel.setBounds(filterTypeComboBox.getBounds().translated(-100, 0));

    frequencySlider.setBounds(controlsArea.removeFromTop(40));
    frequencyLabel.setBounds(frequencySlider.getBounds().translated(-100, 0));

    qSlider.setBounds(controlsArea.removeFromTop(40));
    qLabel.setBounds(qSlider.getBounds().translated(-100, 0));

    gainSlider.setBounds(controlsArea.removeFromTop(40));
    gainLabel.setBounds(gainSlider.getBounds().translated(-100, 0));
}

void MainComponent::updateFilter(int bandIndex)
{
    // 선택된 밴드에 대한 필터 파라미터를 UI에 반영
    frequencySlider.setValue(filter.getCutoffFrequency(bandIndex));
    qSlider.setValue(filter.getQ(bandIndex));
    gainSlider.setValue(filter.getGain(bandIndex));
    filterTypeComboBox.setSelectedId(filter.getFilterType(bandIndex) + 1);
}

void MainComponent::showOptionBox()
{
    // OptionBox 열기
    juce::PopupMenu menu;
    menu.addItem(1, "Basic EQ", true, optionBox.getEQMode() == Optionbox::BasicEQ);
    menu.addItem(2, "Linear Phase EQ", true, optionBox.getEQMode() == Optionbox::LinearPhase);
    
    menu.showMenuAsync(juce::PopupMenu::Options(), [this](int result) {
        if (result == 1)
            optionBox.setEQMode(Optionbox::BasicEQ);
        else if (result == 2)
            optionBox.setEQMode(Optionbox::LinearPhase);
    });
}


