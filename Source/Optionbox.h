#ifndef OPTIONBOX_H
#define OPTIONBOX_H

class Optionbox {
public:
    enum EQMode {
        LinearPhase, // LinearPhase mode // Fir filter
        BasicEQ      // IIR Filter
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
        Level3  // spectrum analyser // level3
    };

    Optionbox();

    void setEQMode(EQMode newMode);
    EQMode getEQMode() const;
    
    void setProcessingMode(ProcessingMode newMode);
    ProcessingMode getProcessingMode() const;
    
    void setBusMode(bool enabled);
    bool getBusMode() const;
    
    void setSpectrumAccuracy(SpectrumAccuracy level);
    SpectrumAccuracy getSpectrumAccuracy() const;
    
    void setPivotSlope(float slope);
    float getPivotSlope() const;

private:
    EQMode currentEQMode;                  // 현재 EQ 모드
    ProcessingMode currentProcessingMode;  // 현재 처리 모드
    bool busModeEnabled;                   // 버스 모드 활성화 여부
    SpectrumAccuracy currentSpectrumAccuracy; // 현재 스펙트럼 분석기 정확도
    float currentPivotSlope;               // 현재 피벗 슬로프
};

#endif // OPTIONBOX_H

