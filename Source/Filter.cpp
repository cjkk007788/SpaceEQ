#include "Filter.h"
#include <JuceHeader.h>
#include "juce_dsp/juce_dsp.h"



void convertToMidSide(juce::AudioBuffer<float>& buffer)
{
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float mid = (leftChannel[i] + rightChannel[i]) * 0.5f;
        float side = (leftChannel[i] - rightChannel[i]) * 0.5f;

        leftChannel[i] = mid;
        rightChannel[i] = side;
    }
}

void convertToLeftRight(juce::AudioBuffer<float>& buffer)
{
    auto* midChannel = buffer.getWritePointer(0);
    auto* sideChannel = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float left = midChannel[i] + sideChannel[i];
        float right = midChannel[i] - sideChannel[i];

        midChannel[i] = left;
        sideChannel[i] = right;
    }
}

Filter::Filter()

    : currentEQMode(Optionbox::BasicEQ)
{
    filterTypes = { LowShelf, Peak, Peak, Peak, Peak, HighShelf };
    cutoffFrequencies = { 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f };
    qValues = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    gains = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
}

Filter::~Filter()
{
}

void Filter::setFilterType(FilterType type, size_t index) {
    if (index < filterTypes.size()) {
        filterTypes[index] = type;
        if (currentEQMode == Optionbox::BasicEQ) {
            updateIIRFilter(index);
        }
        else if (currentEQMode == Optionbox::LinearPhase) {

            //fir filter update

        }
    }
}

Filter::FilterType Filter::getFilterType(size_t index) const {
    if (index < filterTypes.size()) {
        return filterTypes[index];
    }
    return LowPass; //baisc
}

void Filter::setEQMode(Optionbox::EQMode mode) {

    currentEQMode = mode;

    if (mode == Optionbox::BasicEQ) {
        for (int i = 0; i < filterTypes.size(); i++) {
            updateIIRFilter(i);
        }
    }
    else if (mode == Optionbox::LinearPhase) {
        //fir filter update
    }
}

Optionbox::EQMode Filter::getEQMode() const {
    return currentEQMode;
}


void Filter::setCutoffFrequency(float frequency, size_t index) {
    if (index < cutoffFrequencies.size()) {
        cutoffFrequencies[index] = frequency;
        if (currentEQMode == Optionbox::BasicEQ) {
            updateIIRFilter(index);
        }
        else if (currentEQMode == Optionbox::LinearPhase) {
            //fir filter
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
        if (currentEQMode == Optionbox::EQMode::BasicEQ) {
            updateIIRFilter(index);
        }
        else if (currentEQMode == Optionbox::EQMode::LinearPhase) {
            //fir filter
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

    juce::dsp::ProcessSpec sepc;
    sepc.maximumBlockSize = static_cast<uint32_t>(samplesPerBlock);
    sepc.sampleRate = sampleRate;
    sepc.numChannels = 2; //stereo mid / side

    for (auto& filter : iirFilters)
    {
        filter->prepare({ sampleRate, static_cast<uint32_t>(samplesPerBlock), 1 });
    }
}

void Filter::process(juce::AudioBuffer<float>& buffer) {
    juce::dsp::AudioBlock<float> block(buffer);

    switch (currentProcessingMode) {
    case Optionbox::Basic:
        // Stereo mode
        if (currentEQMode == Optionbox::BasicEQ) {
            for (auto& filter : iirFilters) {
                filter->process(juce::dsp::ProcessContextReplacing<float>(block));
            }
        }
        else if (currentEQMode == Optionbox::LinearPhase) {
            for (auto& filter : firFilters) {
                filter->process(juce::dsp::ProcessContextReplacing<float>(block));
            }
        }
        break;
    case Optionbox::Mid:
        // Mid side mode
        convertToMidSide(buffer);

        {
            float* leftChannel[] = { buffer.getWritePointer(0) }; // mid channel pointer arrays
            juce::dsp::AudioBlock<float> midBlock(leftChannel,1, static_cast<size_t>(buffer.getNumSamples()));
            juce::dsp::ProcessContextReplacing<float> midContext(midBlock);

            if (currentEQMode == Optionbox::BasicEQ) {
                for (auto& filter : iirFilters) {
                    filter->process(midContext);
                }
            }
            else if (currentEQMode == Optionbox::LinearPhase) {
                for (auto& filter : firFilters) {
                    filter->process(midContext);
                }
            }
        }

        convertToLeftRight(buffer);
        break;

    case Optionbox::Side:
        // Side mode 
        convertToMidSide(buffer);

        {
            float* rightChannel[] = { buffer.getWritePointer(1) }; // side mode array
            juce::dsp::AudioBlock<float> sideBlock(rightChannel, 1, static_cast<size_t>(buffer.getNumSamples()));
            juce::dsp::ProcessContextReplacing<float> sideContext(sideBlock);

            if (currentEQMode == Optionbox::BasicEQ) {
                for (auto& filter : iirFilters) {
                    filter->process(sideContext);
                }
            }
            else if (currentEQMode == Optionbox::LinearPhase) {
                for (auto& filter : firFilters) {
                    filter->process(sideContext);
                }
            }
        }

        convertToLeftRight(buffer);
        break;

    case Optionbox::SideLeft:
        // SideLeft mode
        {
            float* leftChannel[] = { buffer.getWritePointer(0) }; // left channel array
            juce::dsp::AudioBlock<float> sideLeftBlock(leftChannel,1, buffer.getNumSamples());
            juce::dsp::ProcessContextReplacing<float> sideLeftContext(sideLeftBlock);

            if (currentEQMode == Optionbox::BasicEQ) {
                for (auto& filter : iirFilters) {
                    filter->process(sideLeftContext);
                }
            }
            else if (currentEQMode == Optionbox::LinearPhase) {
                for (auto& filter : firFilters) {
                    filter->process(sideLeftContext);
                }
            }
        }
        break;

    case Optionbox::SideRight:
        // SideRight mode
        {
            float* rightChannel[] = { buffer.getWritePointer(1) }; // right channel
            juce::dsp::AudioBlock<float> sideBlock(rightChannel, 1, static_cast<size_t>(buffer.getNumSamples()));
            juce::dsp::ProcessContextReplacing<float> sideRightContext(sideBlock);
            if (currentEQMode == Optionbox::BasicEQ) {
                for (auto& filter : iirFilters) {
                    filter->process(sideRightContext);
                }
            }
            else if (currentEQMode == Optionbox::LinearPhase) {
                for (auto& filter : firFilters) {
                    filter->process(sideRightContext);
                }
            }
        }
        break;

    default:
        jassertfalse; // not define
        break;
    }
}

void Filter::updateIIRFilter(size_t index) {
    if (index < iirFilters.size()) {
        juce::dsp::IIR::Coefficients<float>::Ptr coefficients;

        switch (filterTypes[index]) {
        case LowPass:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0, cutoffFrequencies[index], qValues[index]);
            break;

        case HighPass:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(44100.0, cutoffFrequencies[index], qValues[index]);
            break;

        case BandPass:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(44100.0, cutoffFrequencies[index], qValues[index]);
            break;

        case Notch:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(44100.0, cutoffFrequencies[index], qValues[index]);
            break;

        case Peak:
            coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(44100.0, cutoffFrequencies[index], qValues[index], gains[index]);
            break;

        case LowShelf:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(44100.0, cutoffFrequencies[index], qValues[index], gains[index]);
            break;

        case HighShelf:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(44100.0, cutoffFrequencies[index], qValues[index], gains[index]);
            break;

        default:
            jassertfalse; // not define
            break;
        }

        *iirFilters[index]->state = *coefficients;
    }

}

void Filter::addIIRFilter(FilterType type, float cutoffFrequency, float q, float gain, Slope slope) {

    for (int i = 0; i < static_cast<int>(slope); ++i)
    {
        auto newFilter = std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>();

        juce::dsp::IIR::Coefficients<float>::Ptr coefficients;

        switch (type)
        {
        case LowPass:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0, cutoffFrequency, q);
            break;

        case HighPass:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(44100.0, cutoffFrequency, q);
            break;

        case BandPass:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(44100.0, cutoffFrequency, q);
            break;

        case Notch:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(44100.0, cutoffFrequency, q);
            break;

        case Peak:
            coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(44100.0, cutoffFrequency, q, gain);
            break;

        case LowShelf:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(44100.0, cutoffFrequency, q, gain);
            break;

        case HighShelf:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(44100.0, cutoffFrequency, q, gain);
            break;

        default:
            jassertfalse;
            return;
        }

        *newFilter->state = *coefficients;
        iirFilters.push_back(std::move(newFilter)); // Add to vector
    }

}

void Filter::removeIIRFilter(size_t index)
{
    if (index < iirFilters.size())
    {
        iirFilters.erase(iirFilters.begin() + index);
    }
}
void Filter::removeFIRFilter(size_t index) {
    if (index < firFilters.size()) {
        firFilters.erase(firFilters.begin() + index);
    }
}

void Filter::setGain(float gainValue, size_t index) {

    if (index < gains.size()) {
        gains[index] = gainValue;
        updateIIRFilter(index); //update from index
    }
}

float Filter::getGain(size_t index) const
{
    if (index < gains.size()) {
        return gains[index];
    }
    return 1.0f;
}

juce::dsp::FIR::Coefficients<float>::Ptr Filter::convertLowpassToHighpass(const juce::dsp::FIR::Coefficients<float>::Ptr& lowpassCoefficients)
{
    size_t order = lowpassCoefficients->coefficients.size();

    juce::dsp::FIR::Coefficients<float> highpass(order);

    for (size_t i = 0; i < order; ++i)
    {
        if (i == order / 2)
            highpass.coefficients.set(i, 1.0f - lowpassCoefficients->coefficients[i]);
        else
            highpass.coefficients.set(i, -lowpassCoefficients->coefficients[i]);
    }

    return highpass;
}

juce::dsp::FIR::Coefficients<float>::Ptr Filter::designLowpassFIR(float cutoffFrequency, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType)
{
    float normalizedCutoff = cutoffFrequency / (sampleRate / 2.0);
    return juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(normalizedCutoff, sampleRate, order, windowType);
}

juce::dsp::FIR::Coefficients<float>::Ptr Filter::designHighpassfilter(float cutoffFrequency, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType)
{
    auto lowpassCoefficients = designLowpassFIR(cutoffFrequency,sampleRate, order, windowType);
    return convertLowpassToHighpass(lowpassCoefficients);
}

juce::dsp::FIR::Coefficients<float>::Ptr Filter::designBandpassFilter(float centerFrequency, float bandwidth, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType)
{
    float lowCutoff = centerFrequency - (bandwidth / 2.0f);
    float highCutoff = centerFrequency + (bandwidth / 2.0f);

    auto lowpass = juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(highCutoff / (sampleRate / 2.0),sampleRate, order, windowType);
    auto highpass = designHighpassfilter(lowCutoff / (sampleRate / 2.0),sampleRate, order, windowType);
    juce::dsp::FIR::Coefficients<float> bandpassCoefficients(order + 1);


    for (size_t i = 0; i <= order; ++i)
    {
        bandpassCoefficients.coefficients.set(i, lowpass->coefficients[i] + highpass->coefficients[i]);

    }

    return bandpassCoefficients;
}

juce::dsp::FIR::Coefficients<float>::Ptr Filter::designBandstopFIR(float centerFrequency, float bandwidth, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType) {

    auto bandpassCoefficients = designBandpassFilter(centerFrequency,bandwidth,sampleRate, order, windowType);
    juce::dsp::FIR::Coefficients<float> bandstopCoefficients(order + 1);
    for (size_t i = 0; i <= order; ++i)
    {
        if (i == order / 2)
            bandstopCoefficients.coefficients.set(i, 1.0f - bandpassCoefficients->coefficients[i]);
        else
            bandstopCoefficients.coefficients.set(i, -bandpassCoefficients->coefficients[i]);
    }

    return bandstopCoefficients;
}

juce::dsp::FIR::Coefficients<float>::Ptr Filter::designPeakFIR(float centerFrequency,float Q, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType, float gain)
{
    // 대역폭을 Q 값을 사용하여 계산
    float bandwidth = centerFrequency / Q;

    // 주파수 정규화
    // 대역 통과 필터 설계
    auto bandpassCoefficients = designBandpassFilter(centerFrequency,Q,sampleRate,order,windowType);

    // 피크 필터 계수를 저장할 객체를 생성합니다. (order + 1개의 계수)
    juce::dsp::FIR::Coefficients<float> peakCoefficients(order + 1);

    // 전체 통과 필터 계수를 설정합니다. (기본적으로 모든 계수가 0이고 중앙이 1인 전체 통과 필터)
    for (size_t i = 0; i <= order; ++i)
    {
        if (i == order / 2)
            peakCoefficients.coefficients.set(i, 1.0f + (bandpassCoefficients->coefficients[i] * (gain - 1.0f)));
        else
            peakCoefficients.coefficients.set(i, bandpassCoefficients->coefficients[i]);
    }

    return peakCoefficients;
}

juce::dsp::FIR::Coefficients<float>::Ptr Filter::designLowShelfFIR(float cutoffFrequency, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType,float gain )
{
    float normalizedCutoff = cutoffFrequency / (sampleRate / 2.0);
    auto lowpassCoefficients = juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(normalizedCutoff,sampleRate,order, windowType);


    juce::dsp::FIR::Coefficients<float> lowShelfCoefficients(order + 1);

    for (size_t i = 0; i <= order; ++i)
    {
        if (i <= order / 2) 
        {
            lowShelfCoefficients.coefficients.set(i, lowpassCoefficients->coefficients[i] * gain);
        }
        else
        {
            lowShelfCoefficients.coefficients.set(i, lowpassCoefficients->coefficients[i]);
        }
    }

    return lowShelfCoefficients;
}

juce::dsp::FIR::Coefficients<float>::Ptr Filter::designHighShelfFIR(float cutoffFrequency, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType, float gain)
{
    auto highpassCoefficients = designHighpassfilter(cutoffFrequency,sampleRate, order, windowType);

    juce::dsp::FIR::Coefficients<float> highShelfCoefficients(order + 1);

    for (size_t i = 0; i <= order; ++i)
    {
        if (i >= order / 2) 
        {
            highShelfCoefficients.coefficients.set(i, highpassCoefficients->coefficients[i] * gain);
        }
        else
        {
            highShelfCoefficients.coefficients.set(i, highpassCoefficients->coefficients[i]);
        }
    }

    return highShelfCoefficients;
}


void Filter::addFIRFilter(FilterType type, float cutoffFrequency, float q, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType, float beta, float gain, Slope slope)
{
    juce::dsp::FIR::Coefficients<float>::Ptr coefficients;
    double sampleRate = 44100.0;
    size_t baseOrder = 64; 
    order = baseOrder * static_cast<size_t>(slope); 

    switch (type)
    {
    case LowPass:
        coefficients = juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(cutoffFrequency, sampleRate, order, windowType);
        break;
    case HighPass:
        coefficients = designHighpassfilter(cutoffFrequency,sampleRate, order, windowType);
        break;
    case BandPass:
        coefficients = designBandpassFilter(cutoffFrequency,q,sampleRate,order,windowType);
        break;
    case Notch:
        coefficients = designBandstopFIR(cutoffFrequency, q, sampleRate, order, windowType);
        break;
    case Peak:
        coefficients = designPeakFIR(cutoffFrequency, q, sampleRate, order, windowType,gain);
        break;
    case HighShelf:
         break;
        coefficients = designHighShelfFIR(cutoffFrequency, sampleRate, order, windowType, gain);
    case LowShelf:
         break;
        coefficients = designLowShelfFIR(cutoffFrequency, sampleRate, order, windowType, gain);
    default:
        jassertfalse;
        return;
    }
    auto newFIRFilter = std::make_unique<juce::dsp::FIR::Filter<float>>(coefficients);
    firFilters.push_back(std::move(newFIRFilter));
}
void Filter::updateFIRFilter(size_t index) {
    if (index < firFilters.size()) {

        juce::dsp::FIR::Coefficients<float>::Ptr coefficients;
        double sampleRate = 44100.0;
        size_t baseOrder = 64;
        size_t order = baseOrder * static_cast<size_t>(slopes[index]);


        switch (filterTypes[index])
        {
        case LowPass:
            coefficients = juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(cutoffFrequencies[index], sampleRate, order, juce::dsp::WindowingFunction<float>::WindowingMethod::hann);
            break;

        case HighPass:
            coefficients = designHighpassfilter(cutoffFrequencies[index], sampleRate, order, juce::dsp::WindowingFunction<float>::WindowingMethod::hann);
            break;

        case BandPass:
            coefficients = designBandpassFilter(cutoffFrequencies[index], qValues[index], sampleRate, order, juce::dsp::WindowingFunction<float>::WindowingMethod::hann);
            break;

        case Notch:
            coefficients = designBandstopFIR(cutoffFrequencies[index], qValues[index], sampleRate, order, juce::dsp::WindowingFunction<float>::WindowingMethod::hann);
            break;

        case Peak:
            coefficients = designPeakFIR(cutoffFrequencies[index], qValues[index], sampleRate, order, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, gains[index]);
            break;

        case LowShelf:
            coefficients = designLowShelfFIR(cutoffFrequencies[index], sampleRate, order, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, gains[index]);
            break;

        case HighShelf:
            coefficients = designHighShelfFIR(cutoffFrequencies[index], sampleRate, order, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, gains[index]);
            break;

        default:
            jassertfalse;
            return;
        }
        firFilters[index]->coefficients = coefficients;
    }
}