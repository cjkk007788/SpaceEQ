#include "Filter.h"
#include <JuceHeader.h>
#include "juce_dsp/juce_dsp.h"


Filter::Filter()

    : currentEQMode(Optionbox::BasicEQ)
{
    filterTypes = { FilterType::LowShelf, FilterType::Peak,FilterType::Peak, FilterType::Peak, FilterType::Peak, FilterType::HighShelf };
    cutoffFrequencies = { 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f };
    qValues = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    //band class완성되면 band에 있는 값 집어 넣는 코드 작성
}

Filter::~Filter()
{
}

void Filter::setFilterType(FilterType type, size_t index) {
    if (index < filterTypes.size()) {
        filterTypes[index] = type;
        if (currentEQMode == Optionbox::BasicEQ) {
            updateIIRFilter();
        }
        else if (currentEQMode == Optionbox::LinearPhase) {
            updateFIRFilter();
        }
    }
}

Filter::FilterType Filter::getFilterType(size_t index) const {
    if (index < filterTypes.size()) {
        return filterTypes[index];
    }
    return LowPass; // 기본값 반환
}

void Filter::setEQMode(Optionbox::EQMode mode) {
    currentEQMode = mode;

    if (mode == Optionbox::BasicEQ) {
        updateIIRFilter();
    }
    else if (mode == Optionbox::LinearPhase) {
        updateFIRFilter();
    }
}

Optionbox::EQMode Filter::getEQMode() const {
    return currentEQMode;
}


void Filter::setCutoffFrequency(float frequency, size_t index) {
    if (index < cutoffFrequencies.size()) {
        cutoffFrequencies[index] = frequency;
        if (currentEQMode == Optionbox::BasicEQ) {
            updateIIRFilter();
        }
        else if (currentEQMode == Optionbox::LinearPhase) {
            updateFIRFilter();
        }
    }
}

float Filter::getCutoffFrequency(size_t index) const {
    if (index < cutoffFrequencies.size()) {
        return cutoffFrequencies[index];
    }
    return 0.0f;
}

void Filter::setQ(float qValue, size_t index) {
    if (index < qValues.size()) {
        qValues[index] = qValue;
        if (currentEQMode == Optionbox::BasicEQ) {
            updateIIRFilter();
        }
        else if (currentEQMode == Optionbox::LinearPhase) {
            updateFIRFilter();
        }
    }
}

float Filter::getQ(size_t index) const {
    if (index < qValues.size()) {
        return qValues[index];
    }
    return 1.0f;
}

void Filter::prepareToPlay(double sampleRate, int samplesPerBlock) {
    if (currentEQMode == Optionbox::BasicEQ) {
        updateIIRFilter();
    }
    else if (currentEQMode == Optionbox::LinearPhase) {
        updateFIRFilter();
    }
}

void Filter::process(juce::AudioBuffer<float>& buffer) {
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    if (currentEQMode == Optionbox::BasicEQ) {
        iirFilterChain.process(context);
    }
    else if (currentEQMode == Optionbox::LinearPhase) {
        firFilterChain.process(context);
    }
}
void Filter::updateIIRFilter() {
    for (size_t i = 0; i < filterTypes.size(); ++i) {
        auto frequency = cutoffFrequencies[i];
        auto qValue = qValues[i];
        juce::dsp::IIR::Coefficients<float>::Ptr coefficients;

        switch (filterTypes[i]) {
        case LowPass:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0, frequency, qValue);
            break;
        case HighPass:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(44100.0, frequency, qValue);
            break;
        case BandPass:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(44100.0, frequency, qValue);
            break;
        case Notch:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(44100.0, frequency, qValue);
            break;
        case Peak:
            coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(44100.0, frequency, qValue, juce::Decibels::decibelsToGain(0.0f)); // 0.0f를 원하시는 게인 값으로 변경
            break;
        case LowShelf:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(44100.0, frequency, qValue, juce::Decibels::decibelsToGain(0.0f)); // 0.0f를 원하시는 게인 값으로 변경
            break;
        case HighShelf:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(44100.0, frequency, qValue, juce::Decibels::decibelsToGain(0.0f)); // 0.0f를 원하시는 게인 값으로 변경
            break;
        default:
            break;
        }

        switch (i) {
        case 0:
            *iirFilterChain.get<0>().state = *coefficients;
            break;
        case 1:
            *iirFilterChain.get<1>().state = *coefficients;
            break;
        case 2:
            *iirFilterChain.get<2>().state = *coefficients;
            break;
        case 3:
            *iirFilterChain.get<3>().state = *coefficients;
            break;
        case 4:
            *iirFilterChain.get<4>().state = *coefficients;
            break;
        case 5:
            *iirFilterChain.get<5>().state = *coefficients;
            break;
        default:
            break;
        }
    }
}