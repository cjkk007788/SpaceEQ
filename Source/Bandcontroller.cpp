#include "Bandcontroller.h"

Band::Band(const juce::String& nameToUse, juce::Colour colourToUse, FilterType typeToUse,
    float frequencyToUse, float qualityToUse, float gainToUse, bool shouldBeActive)
    : name(nameToUse),
    colour(colourToUse),
    type(typeToUse),
    frequency(frequencyToUse),
    quality(qualityToUse),
    gain(gainToUse),
    active(shouldBeActive)
{}

void Bandcontroller::addBand(const juce::String& name, juce::Colour colour, FilterType type,
    float frequency, float quality, float gain, bool active) {
    bands.push_back(Band(name, colour, type, frequency, quality, gain, active));
}

Band* Bandcontroller::getBand(int index) {
    if (index >= 0 && index < bands.size()) {
        return &bands[index];
    }
    return nullptr;
}

void Bandcontroller::setBand(int index, const juce::String& name, juce::Colour colour, FilterType type,
    float frequency, float quality, float gain, bool active) {
    if (index >= 0 && index < bands.size()) {
        bands[index] = Band(name, colour, type, frequency, quality, gain, active);
    }
}