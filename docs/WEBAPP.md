# GPAI 정적 웹앱

## 실행 구조

- `site/index.html`은 공개 화면입니다. 빌드 후 Pages 문서 루트로 복사됩니다.
- `cpp/gpai_core.cpp`와 `cpp/gpai_calculations.hpp`가 계산을 수행합니다. Emscripten이 이를 WebAssembly로 컴파일하며 브라우저는 `ctypes` 대신 C ABI를 JavaScript에서 호출합니다.
- `training/examples.json`은 분야 분류용으로 직접 작성한 48개 한국어 예시입니다.
- `training/train.py`는 표준 Python만 사용해 다항 나이브 베이즈 분류기를 학습하고 `assets/domain-model.json`을 만듭니다.
- 정적 페이지에서 Python 서버를 실행하지 않습니다. Python 학습은 GitHub Actions 빌드 중에 실행되고, 만들어진 JSON 모델은 정적 파일로 공개됩니다.
- 문제 입력은 브라우저에서 분류하고 계산하며 GPAI 서버로 보내지 않습니다. 기존 Gemini API 키나 서버 비밀값은 새 계산 페이지에서 사용하지 않습니다.

## 로컬 빌드

Python 3.10 이상과 Emscripten SDK가 필요합니다.

```sh
python training/train.py --output dist/assets/domain-model.json
cp site/index.html dist/index.html
mkdir -p dist/assets
cp site/assets/* dist/assets/
emcc cpp/gpai_core.cpp -std=c++17 -O2 -fexceptions -DGPAI_CORE_BUILD -Icpp \
  -sDISABLE_EXCEPTION_CATCHING=0 \
  -sMODULARIZE=1 -sEXPORT_NAME=createGpaiModule -sENVIRONMENT=web \
  -sEXPORTED_FUNCTIONS=@cpp/wasm_exports.json \
  -sEXPORTED_RUNTIME_METHODS=ccall,getValue \
  -o dist/assets/gpai_core.js
```

그 다음 `dist`를 정적 HTTP 서버로 제공하세요. `file://`로 HTML 파일을 직접 열면 모델 JSON과 WebAssembly 파일 로딩이 차단될 수 있습니다.

## GitHub Pages 배포

워크플로 `.github/workflows/pages.yml`은 PR과 기능 브랜치에서 빌드 아티팩트를 만들고, `main`에 push되면 GitHub Pages에 배포합니다. 저장소의 **Settings → Pages → Build and deployment → Source**가 **GitHub Actions**여야 합니다. 배포 후 주소는 [https://i71994453-create.github.io/-/](https://i71994453-create.github.io/-/)입니다.

정적 사이트라서 공개 주소를 아는 사람은 페이지와 배포 파일에 접근할 수 있습니다. Python 서버 코드는 실행되지 않고, API 키를 공유하거나 학습 데이터에 넣지 않습니다.

## 분류 모델의 의미

나이브 베이즈 모델은 미적분, 재료역학, 열역학, 기하 중 입력 문장에 가까운 분야를 제안합니다. 제공된 작은 예시 모음으로 학습하므로 전문 용어가 다르거나 여러 분야를 함께 다루는 질문에서 오분류할 수 있습니다. UI의 상대 모델 점수는 확률 보정이 된 신뢰도 값이 아닙니다. 정답을 풀이하는 생성형 AI 모델이 아니며, 최종 계산식과 입력 조건은 사용자가 확인해야 합니다. 데이터를 보강하려면 `training/examples.json`에 대표 문장을 추가하고 같은 학습 명령을 실행하세요.

## 계산 범위

웹 화면은 C API의 계산 함수를 제공합니다. 단위는 SI입니다. Simpson 수치 적분에는 양의 짝수 구간 수가 필요합니다. C++에서 입력 범위를 확인하고 상태 코드로 입력 오류, 오버플로, 내부 오류를 전달합니다.

재료역학 기능은 균일 단면·선형 탄성·기초 보 이론의 식을 계산합니다. 실제 설계, 좌굴, 피로, 소성, 응력 집중, 복합 하중과 법규 검증을 포함하지 않습니다. 열역학의 상태식은 이상기체 가정이며 실제 유체의 상변화나 상태방정식을 다루지 않습니다. 계산 결과는 교육용 기초 계산입니다.

## 대학 강의 자료 및 공식 출처

- University of California, Berkeley, [Math 128A Lecture Notes, Chapter 4](https://persson.berkeley.edu/math128a/lecture_notes/math128a_lecture_notes_chapter_4.html): 수치 미분, Simpson 적분과 오차.
- University of Colorado Boulder, [Introductory Engineering Design, Chapter 14](https://www.colorado.edu/program/ide/sites/default/files/attached-files/introductory_engineering_design_textbook_0_0.pdf): 축응력, Hooke 법칙, 축변형, 굽힘과 원형축 비틀림.
- Massachusetts Institute of Technology, [2.017J Topics in Machine Elements](https://ocw.mit.edu/courses/2-017j-design-of-electromechanical-robotic-systems-fall-2009/16cb0f850752422026a85d838f30e340_MIT2_017JF09_machines.pdf): 극관성 모멘트, 축 비틀림, 응력 공식.
- University of California, Davis, [Physics 9B: First Law and Related Results](https://kiskis.physics.ucdavis.edu/student/phy9b_11/fstl.pdf): 이상기체식, 열역학 제1법칙과 등압 일.
- University of Nebraska–Lincoln, [Thermodynamics Formula Sheet](https://physics.unl.edu/sites/unl.edu.cas.physics/files/media/file/DayIIExam_Aug24.pdf): 카르노 효율 등 열역학 관계식.

이 자료들은 구현한 기초 공식의 출처입니다. 실제 기계·구조 설계에는 해당 분야의 적용 규격과 검증된 재료값이 필요합니다.

## 공식 기술 문서

- [GitHub Pages custom workflows](https://docs.github.com/en/pages/getting-started-with-github-pages/using-custom-workflows-with-github-pages)
- [Emscripten JavaScript/C++ interop](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html)
- [Emscripten C++ exceptions](https://emscripten.org/docs/porting/exceptions.html)
