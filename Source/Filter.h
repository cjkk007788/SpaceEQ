#include "Optionbox.h"
#include <JuceHeader.h>
#include "juce_dsp/juce_dsp.h"
#include <vector>



class Filter
{

private:

    Optionbox::EQMode currentEQMode;  // Optionbox::EQMode 타입 사용
    int currentFilterType;
    float cutoffFrequency;
    float resonance;

    juce::dsp::IIR::Filter<float> iirFilter; // IIR 필터 객체
    juce::dsp::FIR::Filter<float> firFilter; // FIR 필터 객체
    juce::dsp::FIR::Coefficients<float>::Ptr firCoefficients; // FIR 필터 계수

    void updateFilter();

public:

    enum FilterType {
        LowPass,
        HighPass,
        BandPass,
        Notch,
        Peak,
        LowShelf,
        HighShelf,
    };

    Filter(); 
    ~Filter(); 

    void setFilterType(FilterType type);
    FilterType getFilterType(size_t index) const;
    
    void setEQMode(Optionbox::EQMode mode);
    Optionbox::EQMode getEQMode() const;

    void setCutoffFrequency(float frequency);
    float getCutoffFrequency() const;

    void setQ(float resonance);
    float getQ() const;

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);

private:

    std::vector<FilterType> filterTypes;
    Optionbox::EQMode currentEQMode;
    std::vector<float> cutoffFrequencies;
    std::vector<float> qValues;

    using IIRFilterBand = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using FIRFilterBand = juce::dsp::FIR::Filter<float>;
    using Gain = juce::dsp::Gain<float>;

    juce::dsp::ProcessorChain<IIRFilterBand, IIRFilterBand, IIRFilterBand, IIRFilterBand, IIRFilterBand, IIRFilterBand, Gain> iirFilterChain;
    juce::dsp::ProcessorChain<FIRFilterBand, FIRFilterBand, FIRFilterBand, FIRFilterBand, FIRFilterBand, FIRFilterBand, Gain> firFilterChain;

    void updateIIRFilter();
    void updateFIRFilter();
    
    //filter 변수는 기본적으로 밴드가 6개 달린 Filter이다


};