class Optionbox {

public:
    enum EQMode {
        LinearPhase, // LinearPhase mode »ç¿ë½Ã Fir filter
        BasicEQ  // IIR Filter
    };

    enum ProcessingMode {
        Mid, //Mid processing mode
        Side //side processing mode
    };

    enum SpectrumAccuracy {
        Level1, // spectrum analyser ÇØ»óµµ level1
        Level2, // spectrum analyser ÇØ»óµµ level2
        Level3 // spectrum analyser ÇØ»óµµ level3
    };

    Optionbox() {
    };


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
    //ÇöÀç º¯¼ö --> Option box¿¡¼­ UI¼±ÅÃ½Ã ¿©±â º¯¼ö º¯°æ
    EQMode currentEQMode;
    ProcessingMode currentProcessingMode;
    bool busModeEnabled;
    SpectrumAccuracy currentSpectrumAccuracy;
    float currentPivotSlope;

};