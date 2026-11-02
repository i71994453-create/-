# GPAI 공개 웹앱과 AI 계산 API

## 구성 요소

| 구성요소 | 역할 |
|---|---|
| `site/` | GitHub Pages에 올라가는 정적 화면 |
| `cpp/gpai_core.cpp` | 미적분·재료역학·열역학·기하의 계산 코어 |
| `cpp/gpai_dispatch.cpp` | 허용 목록에 있는 C++ 계산만 호출하는 디스패처 |
| `backend/app.py` | 자연어 문제를 AI에 전달하고 함수 호출 흐름을 조정하는 FastAPI 서버 |
| `backend/engine.py` | 입력을 검증하고 C ABI로 C++ 공유 라이브러리를 호출 |
| `backend/calculations.json` | 화면과 AI가 공유하는 계산 항목·입력·출력 정의 |
| `training/` | Python 분야 분류기 학습 예시와 코드 |
| `.github/workflows/pages.yml` | Pages 파일, Python 모델, WebAssembly와 네이티브 C++ API 빌드 |

## AI 요청 흐름

1. 브라우저가 문제 문장을 `POST /api/solve`로 Python API에 보냅니다.
2. Python API가 Gemini에 계산 도구 명세를 전달합니다. 모델은 계산 항목과 숫자 인자를 제안합니다. 모델이 직접 계산 코드를 실행하지는 않습니다.
3. 서버는 도구 이름이 허용 목록에 있는지 확인하고 인자 개수·자료형·유한 숫자 여부를 검사합니다.
4. `gpai_dispatch.cpp`가 C++ 계산 함수를 호출합니다. 실제 계산값은 C++ 결과를 그대로 반환합니다.
5. Python API가 결과를 AI에 전달해 설명을 생성하고, 응답에도 C++ 결과를 별도 필드로 넣습니다.

값이나 단위가 부족하면 AI가 확인을 요청하도록 지시합니다. Gemini 요청은 `store=False`인 stateless 방식으로 보냅니다. 문제 문장은 GPAI 서버와 Gemini API로 전송됩니다.

## 로컬 실행 — Windows PowerShell

필요한 도구: Docker Desktop, Python 3.10 이상, Emscripten SDK. Gemini API 키는 Google AI Studio에서 발급해 서버에서만 사용합니다.

### 1. 서버 비밀값 설정

저장소 루트에서:

```powershell
Copy-Item backend/.env.example backend/.env
notepad backend/.env
```

`GEMINI_API_KEY`에 키를 입력합니다. `backend/.env`는 Git과 Docker 빌드에서 제외됩니다. 키를 `site/assets/config.js`, HTML, JavaScript, GitHub 커밋에 넣지 마세요.

### 2. C++ 계산 API 실행

```powershell
docker build -t gpai-ai-api .
docker run --rm --env-file backend/.env -p 8000:8080 -e GPAI_ALLOWED_ORIGINS=http://localhost:8080 gpai-ai-api
```

새 PowerShell 창에서 `http://localhost:8000/api/health`를 열면 API 상태를 확인할 수 있습니다.

### 3. 웹 프런트엔드 빌드 및 실행

Emscripten SDK가 활성화된 PowerShell에서 저장소 루트로 실행합니다.

```powershell
New-Item -ItemType Directory -Force dist/assets | Out-Null
python training/train.py --output dist/assets/domain-model.json
Copy-Item site/index.html dist/index.html
Copy-Item site/assets/styles.css,site/assets/config.js,site/assets/app.js dist/assets/
```

`site/assets/config.js`에서 로컬 API 주소를 설정합니다.

```js
window.GPAI_API_BASE_URL = "http://localhost:8000";
```

그 후 C++ 코어를 WebAssembly로 빌드하고 정적 파일 서버를 실행합니다.

```powershell
em++ cpp/gpai_core.cpp -std=c++17 -O2 -fexceptions -DGPAI_CORE_BUILD -Icpp -sDISABLE_EXCEPTION_CATCHING=0 -sMODULARIZE=1 -sEXPORT_NAME=createGpaiModule -sENVIRONMENT=web -sEXPORTED_FUNCTIONS=@cpp/wasm_exports.json -sEXPORTED_RUNTIME_METHODS=ccall,getValue -o dist/assets/gpai_core.js
python -m http.server 8080 --directory dist
```

브라우저에서 `http://localhost:8080`을 엽니다. 페이지의 C++ 계산기와 AI 계산 요청을 사용할 수 있습니다.

## GitHub Pages와 공개 AI 서버

GitHub Pages는 정적 파일 호스팅이라 Python API를 실행하지 않습니다. 공개 웹사이트에서 AI를 쓰려면 Pages 화면과 HTTPS가 적용된 별도 API 서버가 모두 필요합니다.

클라우드 배포 전에 Google Cloud Secret Manager에 `gemini-api-key` 이름으로 API 키를 저장하고 Cloud Run 서비스 계정에 그 비밀값을 읽을 권한을 부여합니다. 저장소 루트에서 실행하는 예시입니다.

```sh
gcloud run deploy gpai-ai-api \
  --source . \
  --region asia-northeast3 \
  --allow-unauthenticated \
  --max 2 \
  --set-secrets GEMINI_API_KEY=gemini-api-key:latest \
  --set-env-vars GPAI_ALLOWED_ORIGINS=https://i71994453-create.github.io
```

배포가 끝나면 출력된 HTTPS 서비스 주소를 `site/assets/config.js`의 `GPAI_API_BASE_URL` 값에 넣고 변경 사항을 저장소에 반영합니다. Pages 설정의 **Settings → Pages → Build and deployment → Source**는 **GitHub Actions**여야 합니다.

이 예시는 공개 API입니다. 코드의 10회/분 IP 제한은 각 서버 인스턴스 메모리에서만 작동해 전체 인스턴스의 정확한 제한이나 인증을 제공하지 않습니다. 공개 배포 전에 Cloud Run 최대 인스턴스·요청 제한, Gemini 사용량 한도와 결제 알림을 설정하세요. CORS는 브라우저 정책이지 접근 인증이 아닙니다.

## 키와 개인정보 취급

- `GEMINI_API_KEY`는 서버 환경 변수 또는 Secret Manager에서만 읽습니다.
- `store=False`는 Gemini Interactions 저장을 끄는 API 설정입니다. 직접 운영하는 서버의 로그나 호스팅 사업자의 운영 로그까지 지우지는 않습니다.
- AI 기능을 쓰면 입력한 문제와 값이 GPAI 백엔드와 Gemini API로 전송됩니다. 민감한 개인정보는 입력하지 마세요.
- API 오류 응답에 공급자 원문이나 API 키를 넣지 않습니다.

## 분야 분류 모델

`training/examples.json`의 한국어 예시 48개로 나이브 베이즈 분류기를 학습합니다. 상대 모델 점수는 보정된 확률이 아니며, 여러 분야가 섞인 문제에서 오분류할 수 있습니다. 분야 분류는 풀이를 생성하는 AI와 별개 기능입니다.

## 계산 범위와 학술 출처

기본 단위는 SI입니다. Simpson 수치 적분은 양의 짝수 분할 수를 사용합니다. 재료역학 코어는 균일 단면·선형 탄성·기초 보 이론을 쓰며, 좌굴·피로·소성·응력 집중·복합 하중 및 법규 검증은 하지 않습니다. 열역학 함수는 이상기체 가정과 기초 관계식을 다룹니다. 결과는 교육용이며 실제 설계에 그대로 사용하지 마세요.

- University of California, Berkeley, [Math 128A Lecture Notes, Chapter 4](https://persson.berkeley.edu/math128a/lecture_notes/math128a_lecture_notes_chapter_4.html): 수치 미분과 Simpson 적분.
- University of Colorado Boulder, [Introductory Engineering Design, Chapter 14](https://www.colorado.edu/program/ide/sites/default/files/attached-files/introductory_engineering_design_textbook_0_0.pdf): 축응력·변형, 보 굽힘, 원형축 비틀림.
- Massachusetts Institute of Technology, [2.017J Topics in Machine Elements](https://ocw.mit.edu/courses/2-017j-design-of-electromechanical-robotic-systems-fall-2009/16cb0f850752422026a85d838f30e340_MIT2_017JF09_machines.pdf): 단면 관성모멘트와 비틀림.
- University of California, Davis, [Physics 9B: First Law and Related Results](https://kiskis.physics.ucdavis.edu/student/phy9b_11/fstl.pdf): 이상기체 상태식, 열역학 제1법칙, 등압 일.
- University of Nebraska–Lincoln, [Thermodynamics Formula Sheet](https://physics.unl.edu/sites/unl.edu.cas.physics/files/media/file/DayIIExam_Aug24.pdf): 카르노 효율 등.

## 공식 기술 문서

- Google AI for Developers, [Gemini function calling](https://ai.google.dev/gemini-api/docs/function-calling)
- Google AI for Developers, [Interactions API and data storage](https://ai.google.dev/gemini-api/docs/interactions-overview)
- Google AI for Developers, [API key security](https://ai.google.dev/gemini-api/docs/api-key)
- GitHub Docs, [GitHub Pages custom workflows](https://docs.github.com/en/pages/getting-started-with-github-pages/using-custom-workflows-with-github-pages)
- Emscripten, [C++/JavaScript interop](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html)
