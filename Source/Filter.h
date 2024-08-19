#include "Optionbox.h"
#include <JuceHeader.h>
#include "juce_dsp/juce_dsp.h"
#include <vector>



class Filter
{

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

    void addIIRFilter(FilterType type, float cutoffFrequency, float q,float gain);
    void removeIIRFilter(size_t index);
    void setFilterType(FilterType type, size_t index);
    FilterType getFilterType(size_t index) const;
    
    void setEQMode(Optionbox::EQMode mode);
    Optionbox::EQMode getEQMode() const;

    void setCutoffFrequency(float frequency, size_t index);
    float getCutoffFrequency(size_t index) const;

    void setQ(float qValue, size_t index);
    float getQ(size_t index) const;

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);

    void setGain(float gainValue, size_t index);
    float getGain(size_t index) const;

    
    //filter baisic 6bands

private:

    Optionbox::EQMode currentEQMode; // linear or basic
    Optionbox::ProcessingMode currentProcessingMode; 

    std::vector<FilterType> filterTypes;
    std::vector<float> cutoffFrequencies;
    std::vector<float> qValues;
    std::vector<float> gains; 



    using IIRFilterBand = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using FIRFilterBand = juce::dsp::FIR::Filter<float>;
    using Gain = juce::dsp::Gain<float>;

    std::vector<std::unique_ptr<IIRFilterBand>> iirFilters;
    void updateIIRFilter(size_t index);

};