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
    //기본적으로 밴드 6개
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
        // stereo mode
        for (auto& filter : iirFilters) {
            filter->process(juce::dsp::ProcessContextReplacing<float>(block));
        }
        break;

    case Optionbox::Mid:
        // Mid side mode
        convertToMidSide(buffer);

        {
            float* leftChannel[] = { buffer.getWritePointer(0) }; // mid channel pointer arrays
            juce::dsp::AudioBlock<float> midBlock(leftChannel,1, static_cast<size_t>(buffer.getNumSamples()));
            juce::dsp::ProcessContextReplacing<float> midContext(midBlock);

            for (auto& filter : iirFilters) {
                filter->process(midContext);
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

            for (auto& filter : iirFilters) {
                filter->process(sideContext);
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
            for (auto& filter : iirFilters) {
                filter->process(sideLeftContext);
            }
        }
        break;

    case Optionbox::SideRight:
        // SideRight mode
        {
            float* rightChannel[] = { buffer.getWritePointer(1) }; // right channel
            juce::dsp::AudioBlock<float> sideBlock(rightChannel, 1, static_cast<size_t>(buffer.getNumSamples()));
            juce::dsp::ProcessContextReplacing<float> sideRightContext(sideBlock);
            for (auto& filter : iirFilters) {
                filter->process(sideRightContext);
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

void Filter::addIIRFilter(FilterType type, float cutoffFrequency, float q, float gain) {
    auto newFilter = std::make_unique<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>>();

    // filter type
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
        break;
    }

    *newFilter->state = *coefficients; // new coefficient
    iirFilters.push_back(std::move(newFilter)); // add vector

}

void Filter::removeIIRFilter(size_t index)
{
    if (index < iirFilters.size())
    {
        iirFilters.erase(iirFilters.begin() + index);
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
