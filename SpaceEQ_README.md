# SpaceEQ

> JUCE 기반 오디오 이퀄라이저(EQ) 플러그인 프로젝트 (VST3 / AU / Standalone)

SpaceEQ는 [JUCE](https://juce.com/) 프레임워크로 만드는 오디오 이퀄라이저 플러그인입니다. 현재는 **초기 개발 단계**로, 플러그인 기본 골격(프로세서·에디터)이 세팅되어 있고 EQ 핵심 기능(필터, 스펙트럼 분석기, 밴드 컨트롤 UI)을 구현해 나가는 중입니다.

---

## 현재 상태

| 항목 | 상태 |
| --- | --- |
| JUCE 프로젝트 설정 (`.jucer`) | ✅ 완료 |
| 플러그인 프로세서 / 에디터 골격 | ✅ 완료 (JUCE 템플릿 기본 상태) |
| 오디오 처리 (`processBlock`) | ⬜ 미구현 (현재는 입력을 그대로 통과) |
| 필터 (`Filter`) | ⬜ 파일만 생성됨 |
| 스펙트럼 분석기 (`Analyzer`) | ⬜ 파일만 생성됨 |
| 밴드 컨트롤 UI (`Bandcontroller`) | ⬜ 파일만 생성됨 |
| 옵션 박스 (`Optionbox`) | ⬜ 뼈대만 있음 |
| 파라미터 저장/복원 | ⬜ 미구현 |

> 에디터는 아직 "Hello World!" 화면(400×300)만 표시합니다.

---

## 계획된 구조

```
SpaceEQ
├── SpaceEQ.jucer            # Projucer 프로젝트 파일
├── JuceLibraryCode/         # Projucer가 자동 생성하는 JUCE 모듈 래퍼
└── Source
    ├── PluginProcessor.*    # 오디오 처리 및 상태 관리
    ├── PluginEditor.*       # 플러그인 UI 창
    ├── Filter.*             # EQ 필터 (예정)
    ├── Analyzer.*           # 스펙트럼 분석기 (예정)
    ├── Bandcontroller.*     # 밴드별 컨트롤 (예정)
    └── Optionbox.*          # 옵션 UI (예정)
```

파일명을 기준으로 한 설계 의도이며, 구현이 진행되면서 달라질 수 있습니다.

---

## 빌드 환경

| 항목 | 내용 |
| --- | --- |
| 프레임워크 | JUCE 8.0.0 |
| 프로젝트 타입 | Audio Plug-in |
| 플러그인 포맷 | VST3, AU, Standalone |
| 채널 구성 | 모노 / 스테레오 (입력과 출력 동일) |
| 빌드 대상 | Visual Studio 2019 / 2022, Xcode (macOS) |

---

## 빌드 방법

### 1. JUCE 준비

[JUCE 8.0.0](https://juce.com/download/)을 다운로드합니다.

### 2. 모듈 경로 확인

`SpaceEQ.jucer`의 모듈 경로가 개발자 PC 기준 상대 경로(`../../juce-8.0.0-windows/JUCE/modules`)로 지정되어 있습니다. 본인의 JUCE 설치 위치가 다르면 Projucer에서 모듈 경로를 수정해야 합니다.

### 3. 프로젝트 생성 및 빌드

1. Projucer로 `SpaceEQ.jucer`를 엽니다.
2. 모듈 경로를 본인 환경에 맞게 설정합니다.
3. 사용하는 IDE(Visual Studio 또는 Xcode)를 선택해 **Save and Open in IDE**를 누릅니다.
4. IDE에서 빌드합니다. 빌드 결과물은 `Builds/` 폴더 아래에 생성됩니다.

> `Builds/` 폴더는 `.gitignore`에 포함되어 있어 저장소에 올라가지 않습니다. 클론 후 Projucer로 직접 생성해야 합니다.

---

## 로드맵

- [ ] 파라미터 시스템 구성 (`AudioProcessorValueTreeState`)
- [ ] 밴드별 필터 구현 (Peak, Low/High Shelf, Low/High Cut)
- [ ] `processBlock`에 필터 체인 적용
- [ ] 실시간 스펙트럼 분석기 (FFT)
- [ ] 밴드 컨트롤 UI (주파수 / 게인 / Q)
- [ ] 프리셋 및 상태 저장/복원
- [ ] 플러그인 제조사 정보 설정 (현재 기본 placeholder `yourcompany`)

---

## License

아직 라이선스가 지정되지 않았습니다.

> JUCE는 별도의 라이선스(AGPLv3 또는 상용)를 따르므로, 배포 시 JUCE 라이선스 조건을 확인해야 합니다.
