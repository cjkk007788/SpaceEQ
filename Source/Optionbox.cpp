#include "Optionbox.h"


//defalut EQ Mode
Optionbox::Optionbox()
    : currentEQMode(BasicEQ), // basicEQ  ==> IIR FILTER
    currentProcessingMode(Mid), // MID PROCESSING
    busModeEnabled(false), // BUS MODE
    currentSpectrumAccuracy(Level1), // Spectrum
    currentPivotSlope(0.0f) // pivot
{
}
void Optionbox::setEQMode(EQMode newMode) {
    currentEQMode = newMode;
}

Optionbox::EQMode Optionbox::getEQMode() const {
    return currentEQMode;
}

void Optionbox::setProcessingMode(ProcessingMode newMode) {
    currentProcessingMode = newMode;
}

void Optionbox::setBusMode(bool enabled) {
    busModeEnabled = enabled;
}

bool Optionbox::getBusMode() const {
    return busModeEnabled;
}

void Optionbox::setSpectrumAccuracy(SpectrumAccuracy level) {
    currentSpectrumAccuracy = level;
}

void Optionbox::setPivotSlope(float slope) {
    currentPivotSlope = slope;
}

float Optionbox::getPivotSlope() const {
    return currentPivotSlope;
}