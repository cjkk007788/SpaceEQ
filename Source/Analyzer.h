#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <atomic>

template<typename Type>
class Analyzer : public juce::Thread
{
public:
    Analyzer();
    ~Analyzer() override;

    void addAudioData(const juce::AudioBuffer<Type>& buffer, int startChannel, int numChannels);
    void setupAnalyzer(int audioFifoSize, Type sampleRateToUse);
    void run() override;
    bool checkForNewData();
    void createPath(juce::Path& p, const juce::Rectangle<float> bounds, float minFreq);

private:
    void performFFT();

    inline float indexToX(float index, float minFreq) const;
    inline float binToY(float bin, const juce::Rectangle<float> bounds) const;

    juce::WaitableEvent waitForData;
    juce::CriticalSection pathCreationLock;

    Type sampleRate{};

    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<Type> windowing;
    juce::AudioBuffer<float> fftBuffer;

    juce::AudioBuffer<float> averager;
    int averagerPtr = 1;

    juce::AbstractFifo abstractFifo;
    juce::AudioBuffer<Type> audioFifo;

    std::atomic<bool> newDataAvailable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Analyzer)
};