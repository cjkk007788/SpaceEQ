#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>

enum FilterType
{
    NoFilter = 0,
    HighPass,
    HighPass1st,
    LowShelf,
    BandPass,
    AllPass,
    AllPass1st,
    Notch,
    Peak,
    HighShelf,
    LowPass1st,
    LowPass,
    LastFilterID
};

struct Band {
    Band(const juce::String& nameToUse, juce::Colour colourToUse, FilterType typeToUse,
        float frequencyToUse, float qualityToUse, float gainToUse = 1.0f, bool shouldBeActive = true);

    juce::String name;
    juce::Colour colour;
    FilterType   type = BandPass;
    float        frequency = 1000.0f;
    float        quality = 1.0f;
    float        gain = 1.0f;
    bool         active = true;
    std::vector<double> magnitudes;
};

class Bandcontroller {
public:
    void addBand(const juce::String& name, juce::Colour colour, FilterType type,
        float frequency, float quality, float gain = 1.0f, bool active = true);

    Band* getBand(int index);

    void setBand(int index, const juce::String& name, juce::Colour colour, FilterType type,
        float frequency, float quality, float gain = 1.0f, bool active = true);

private:
    std::vector<Band> bands; 
};