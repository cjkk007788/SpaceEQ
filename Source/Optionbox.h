class Optionbox {

public:
    enum EQMode {
        LinearPhase, // LinearPhase mode // Fir filter
        BasicEQ  // IIR Filter
    };

    enum ProcessingMode {
        Basic,       // Stereo mode
        Mid,         // Mid processing mode
        Side,        // Side processing mode
        SideLeft,    // Side Left channel processing mode
        SideRight    // Side Right channel processing mode
    };

    enum SpectrumAccuracy {
        Level1, // spectrum analyser // level1
        Level2, // spectrum analyser // level2
        Level3 // spectrum analyser // level3
    };

    Optionbox();

    void setEQMode(EQMode newMode);

    EQMode getEQMode() const;
    
    void setProcessingMode(ProcessingMode newMode);
    
    void setBusMode(bool enabled);
    
    ProcessingMode getProcessingMode() const;
    
    bool getBusMode() const;
    
    void setSpectrumAccuracy(SpectrumAccuracy level);
    SpectrumAccuracy getSpectrumAccuracy() const;
    void setPivotSlope(float slope);
    float getPivotSlope() const;


private:
    EQMode currentEQMode; //currentEqMode Parameter --> using for select linear phase mode or BasicEQmode
    ProcessingMode currentProcessingMode; // using for selecting side/mid/sideleft,sideright..... 
    bool busModeEnabled;//Using for bus mode
    SpectrumAccuracy currentSpectrumAccuracy;// using for selecting spectrum accuracy
    float currentPivotSlope; // using for pivot slope mode

};