#include "Optionbox.h"
#include <JuceHeader.h>
#include "juce_dsp/juce_dsp.h"
#include <vector>



class Filter
{

public:

    enum FilterType { //fitler type enum for fir and iir
        LowPass,
        HighPass,
        BandPass,
        Notch,
        Peak,
        LowShelf,
        HighShelf,
    };

    enum Slope { //slope for filter (using for fir and iir)
        
        Slope6dB = 1,
        Slope12dB = 2,
        Slope18dB = 3,
        Slope24dB = 4,

    };

    Filter(); 
    ~Filter();

    void addIIRFilter(FilterType type, float cutoffFrequency, float q,float gain,Slope slope); // adding iir filter band parameter
    void removeIIRFilter(size_t index); // removing iirfilter band with indexing
    void setFilterType(FilterType type, size_t index); // set filter type
    void updateIIRFilter(size_t index);//update iirfilter(not adding, for chagning parameter in band)

    FilterType getFilterType(size_t index) const;

    void addFIRFilter(FilterType type, float cutoffFrequency, float q, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType, float beta,float gain,Slope slope);
    //adding firfilter band
    void removeFIRFilter(size_t index);
    //removing firfilter band with index
    void updateFIRFilter(size_t index); 
    // chaing prameter of band

    
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

    juce::dsp::FIR::Coefficients<float>::Ptr Filter::designHighpassfilter(float cutoffFrequenc,double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType);
    juce::dsp::FIR::Coefficients<float>::Ptr designBandpassFilter(float centerFrequency, float bandwidth, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType);
    juce::dsp::FIR::Coefficients<float>::Ptr designBandstopFIR(float centerFrequency, float bandwidth, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType);
    juce::dsp::FIR::Coefficients<float>::Ptr Filter::designPeakFIR(float centerFrequency, float Q, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType, float gain);
    juce::dsp::FIR::Coefficients<float>::Ptr Filter::designLowShelfFIR(float cutoffFrequency, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType, float gain);
    juce::dsp::FIR::Coefficients<float>::Ptr Filter::designHighShelfFIR(float cutoffFrequency, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType, float gain );
    juce::dsp::FIR::Coefficients<float>::Ptr convertLowpassToHighpass(const juce::dsp::FIR::Coefficients<float>::Ptr& lowpassCoefficients);
    juce::dsp::FIR::Coefficients<float>::Ptr designLowpassFIR(float cutoffFrequency, double sampleRate, size_t order, juce::dsp::WindowingFunction<float>::WindowingMethod windowType);
    // getting coeefficients of filter --> coeefitent makes filter type

private:

    Optionbox::EQMode currentEQMode; // linear or basic
    Optionbox::ProcessingMode currentProcessingMode; //mid side

    //vectors for parameter of bands


    std::vector<FilterType> filterTypes;
    std::vector<float> cutoffFrequencies;
    std::vector<float> qValues;
    std::vector<float> gains;
    std::vector<Slope> slopes;

    //vectors for parameter of bands

    using IIRFilterBand = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using FIRFilterBand = juce::dsp::FIR::Filter<float>;
    using Gain = juce::dsp::Gain<float>;

    //filter vectors
    std::vector<std::unique_ptr<IIRFilterBand>> iirFilters;
    std::vector<std::unique_ptr<FIRFilterBand>> firFilters; 
    //fiter vecotrs

};