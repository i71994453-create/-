# -
물리와 수학 문제를 풀이합니다.
<!DOCTYPE html>
<html lang="ko">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>고등학교 수학·물리 AI 솔버 & 2D 그래프 시각화</title>

    <!-- 1. MathJax: LaTeX 수식 렌더링 엔진 -->
    <script>
        window.MathJax = {
            tex: {
                inlineMath: [['$', '$'], ['\\(', '\\)']],
                displayMath: [['$$', '$$'], ['\\[', '\\]']]
            },
            svg: { fontCache: 'global' }
        };
    </script>
    <script src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js" async></script>

    <!-- 2. Marked.js: AI의 마크다운 텍스트를 HTML로 변환 -->
    <script src="https://cdn.jsdelivr.net/npm/marked/marked.min.js"></script>

    <!-- 3. Nerdamer.js: 브라우저 내장 기호 수학 연산 엔진 (방정식, 미분, 적분) -->
    <script src="https://cdn.jsdelivr.net/npm/nerdamer@1.1.13/all.min.js"></script>

    <!-- 4. Plotly.js: 수학 함수 및 물리 운동 2D 인터랙티브 그래프 엔진 -->
    <script src="https://cdn.plot.ly/plotly-2.35.2.min.js"></script>

    <!-- 웹페이지 스타일 (CSS) -->
    <style>
        :root {
            --primary: #2563eb;
            --primary-hover: #1d4ed8;
            --bg: #f8fafc;
            --card-bg: #ffffff;
            --text: #1e293b;
            --border: #e2e8f0;
        }
        * { box-sizing: border-box; font-family: 'Pretendard', -apple-system, BlinkMacSystemFont, sans-serif; }
        body { margin: 0; padding: 20px; background-color: var(--bg); color: var(--text); line-height: 1.6; }
        .container { max-width: 1000px; margin: 0 auto; }
        
        header {
            background: linear-gradient(135deg, #1e3a8a, #2563eb);
            color: white; padding: 28px; border-radius: 16px; margin-bottom: 24px;
            box-shadow: 0 4px 15px rgba(37, 99, 235, 0.15);
        }
        header h1 { margin: 0 0 8px 0; font-size: 1.7rem; }
        header p { margin: 0; opacity: 0.9; font-size: 0.95rem; }

        .card {
            background: var(--card-bg); border: 1px solid var(--border);
            border-radius: 14px; padding: 24px; margin-bottom: 20px;
            box-shadow: 0 2px 6px rgba(0,0,0,0.03);
        }

        .settings-row { display: flex; gap: 16px; flex-wrap: wrap; align-items: center; }
        .settings-row > div { flex: 1; min-width: 180px; }
        label { display: block; font-weight: 600; margin-bottom: 6px; font-size: 0.9rem; }
        input[type="password"], input[type="text"], input[type="number"], select, textarea {
            width: 100%; padding: 10px 14px; border: 1px solid #cbd5e1;
            border-radius: 8px; font-size: 0.95rem; outline: none; transition: border 0.2s;
        }
        input:focus, select:focus, textarea:focus { border-color: var(--primary); }

        /* 탭 버튼 스타일 */
        .tabs { display: flex; gap: 10px; margin-bottom: 20px; }
        .tab-btn {
            flex: 1; padding: 14px; font-size: 1rem; font-weight: 700;
            background: #e2e8f0; border: none; border-radius: 10px; cursor: pointer;
            color: #475569; transition: all 0.2s;
        }
        .tab-btn.active { background: var(--primary); color: white; box-shadow: 0 4px 10px rgba(37,99,235,0.25); }
        .tab-content { display: none; }
        .tab-content.active { display: block; }

        /* 수식 입력 보조 키패드 */
        .keypad { display: flex; gap: 6px; flex-wrap: wrap; margin: 10px 0 14px 0; }
        .key-btn {
            padding: 6px 12px; background: #f1f5f9; border: 1px solid #cbd5e1;
            border-radius: 6px; cursor: pointer; font-size: 0.85rem; font-weight: 600;
        }
        .key-btn:hover { background: #e2e8f0; }

        /* 실행 버튼 */
        .solve-btn {
            width: 100%; padding: 14px; background: var(--primary); color: white;
            border: none; border-radius: 10px; font-size: 1.05rem; font-weight: 700;
            cursor: pointer; margin-top: 12px; transition: background 0.2s;
        }
        .solve-btn:hover { background: var(--primary-hover); }

        /* 결과 및 그래프 영역 스타일 */
        .result-box {
            background: #f8fafc; border-left: 5px solid var(--primary);
            padding: 20px; border-radius: 8px; margin-top: 16px; overflow-x: auto;
        }
        .graph-container {
            background: #ffffff; border: 1px solid var(--border);
            border-radius: 12px; padding: 16px; margin-top: 18px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.04);
        }
        .sympy-badge {
            display: inline-block; background: #dbeafe; color: #1e40af;
            padding: 4px 10px; border-radius: 6px; font-size: 0.8rem; font-weight: 700; margin-bottom: 8px;
        }
        .graph-badge {
            display: inline-block; background: #dcfce7; color: #166534;
            padding: 4px 10px; border-radius: 6px; font-size: 0.8rem; font-weight: 700; margin-bottom: 8px;
        }
        .error-msg { color: #dc2626; background: #fef2f2; padding: 12px; border-radius: 8px; font-weight: 600; }
    </style>
</head>
<body>

<div class="container">
    <header>
        <h1>📐 고등학교 수학·물리 AI 솔버 & 2D 그래프 시각화</h1>
        <p>자연어 조건 분석, 기호 수학 정밀 계산, 그리고 함수 및 물리 운동의 실시간 2D 그래프 렌더링</p>
    </header>

    <!-- 상단 환경 설정 카드 -->
    <div class="card">
        <div class="settings-row">
            <div style="flex: 2;">
                <label for="apiKey">🔑 Gemini API Key 입력 (Google AI Studio)</label>
                <input type="password" id="apiKey" placeholder="AIzaSy... 로 시작하는 API 키를 입력하세요">
            </div>
            <div style="flex: 1.5;">
                <label for="difficulty">🎯 설명 상세도 선택</label>
                <select id="difficulty">
                    <option value="기초부터 아주 쉽게 단계별로">기초부터 아주 쉽게</option>
                    <option value="고등학교 표준 교과서 및 수능 풀이 수준으로" selected>표준 교과서 수준</option>
                    <option value="심화 개념과 공식 유도 과정을 포함하여">심화 개념 포함</option>
                </select>
            </div>
        </div>
    </div>

    <!-- 모드 전환 탭 버튼 -->
    <div class="tabs">
        <button class="tab-btn active" onclick="switchTab('tab-natural', this)">
            📝 1. 자연어로 문제 조건 입력 (+ 물리/수학 그래프)
        </button>
        <button class="tab-btn" onclick="switchTab('tab-formula', this)">
            🧮 2. 수식만 넣기 (+ 정밀 계산 & 2D 함수 그래프)
        </button>
    </div>

    <!-- [탭 1] 자연어로 문제 조건 입력 -->
    <div id="tab-natural" class="card tab-content active">
        <h3>📝 문장으로 된 수학·물리 문제 조건 입력하기</h3>
        <div style="margin-bottom: 12px;">
            <label for="subjectSelect">과목 선택</label>
            <select id="subjectSelect">
                <option value="물리학 (역학/전자기/파동)">물리학 (역학 / 전자기 / 파동)</option>
                <option value="수학 (대수/미적분/기하/확률)">수학 (대수 / 미적분 / 기하 / 확률)</option>
            </select>
        </div>
        <label for="naturalInput">문제 조건 및 질문 입력</label>
        <textarea id="naturalInput" rows="4" placeholder="예시 (물리): 지면에서 초속 20m/s로 수직 위로 던져 올린 공의 시간에 따른 높이를 식으로 나타내고, 최고점 도달 시간과 높이를 구해줘. (중력가속도 g = 10m/s^2)&#10;예시 (수학): 이차함수 y = x^2 - 4x + 3의 꼭짓점 좌표와 x절편을 구해줘."></textarea>
        
        <button class="solve-btn" onclick="solveNaturalLanguage()">🚀 자연어 문제 풀이 및 2D 그래프 그리기</button>
        <div id="naturalResult"></div>
        <!-- 자연어 문제에서 추출된 수식의 2D 그래프가 그려질 영역 -->
        <div id="naturalGraphWrapper" class="graph-container" style="display: none;">
            <span class="graph-badge">📈 AI가 추출한 물리 운동 / 수학 함수 2D 그래프</span>
            <div id="naturalGraphPlot" style="width: 100%; height: 420px;"></div>
        </div>
    </div>

    <!-- [탭 2] 수식만 넣기 -->
    <div id="tab-formula" class="card tab-content">
        <h3>🧮 수식만 직접 입력하여 풀기 (기호 수학 엔진 + 2D 그래프 + AI 해설)</h3>
        <div class="settings-row" style="margin-bottom: 10px;">
            <div style="flex: 1.2;">
                <label for="operationType">연산 종류 선택</label>
                <select id="operationType">
                    <option value="solve">방정식 해 구하기 (실근 위치 표시)</option>
                    <option value="diff">미분하기 (원함수와 도함수 비교)</option>
                    <option value="integrate">부정적분하기 (원함수와 적분함수 비교)</option>
                    <option value="factor">인수분해 및 함수 개형 그리기</option>
                </select>
            </div>
            <div style="flex: 2;">
                <label for="formulaInput">수식 입력 (변수 x 기준, 곱셈 생략 가능)</label>
                <input type="text" id="formulaInput" placeholder="예: x^2 - 5x + 6 = 0  또는  x^3 - 3x">
            </div>
        </div>

        <!-- 그래프 x축 범위 설정 -->
        <div class="settings-row" style="margin-bottom: 10px;">
            <div>
                <label for="xMinInput">📉 그래프 x축 최소값 (x_min)</label>
                <input type="number" id="xMinInput" value="-10" step="1">
            </div>
            <div>
                <label for="xMaxInput">📈 그래프 x축 최대값 (x_max)</label>
                <input type="number" id="xMaxInput" value="10" step="1">
            </div>
        </div>

        <!-- 마우스 클릭으로 수식을 쉽게 넣는 보조 키패드 -->
        <div class="keypad">
            <span style="font-size:0.85rem; font-weight:600; align-self:center; margin-right:4px;">기호 빠른 입력:</span>
            <button class="key-btn" onclick="insertSymbol('x^2')">x²</button>
            <button class="key-btn" onclick="insertSymbol('x^3')">x³</button>
            <button class="key-btn" onclick="insertSymbol('sqrt(x)')">√x</button>
            <button class="key-btn" onclick="insertSymbol('sin(x)')">sin(x)</button>
            <button class="key-btn" onclick="insertSymbol('cos(x)')">cos(x)</button>
            <button class="key-btn" onclick="insertSymbol('exp(x)')">eˣ</button>
            <button class="key-btn" onclick="insertSymbol(' = 0')">= 0</button>
        </div>

        <button class="solve-btn" onclick="solveFormulaOnly()">⚡ 수식 정밀 계산 & 2D 그래프 시각화</button>
        
        <!-- 수식 전용 2D 그래프가 그려질 영역 -->
        <div id="formulaGraphWrapper" class="graph-container" style="display: none;">
            <span class="graph-badge">📊 실시간 2D 함수 및 연산 결과 비교 그래프 (마우스로 확대/축소 가능)</span>
            <div id="formulaGraphPlot" style="width: 100%; height: 440px;"></div>
        </div>

        <div id="formulaResult"></div>
    </div>
</div>

<!-- ==========================================
     [자바스크립트 핵심 로직: 수식 계산, 2D 그래프, AI 연동]
     ========================================== -->
<script>
    // ==========================================
    // [수정 가능한 주요 변수 및 매개변수 설정]
    // ==========================================
    // 1. 사용할 Gemini 모델 이름
    const MODEL_NAME = "gemini-2.5-flash";

    // 2. 창의성 조절 변수 (0.0 ~ 1.0)
    const TEMPERATURE = 0.1;

    // 3. 수식 계산 기본 미지수 기호
    const DEFAULT_VARIABLE = "x";

    // 4. 2D 그래프를 그릴 때 샘플링할 좌표 점의 개수 (높을수록 곡선이 매끄러워짐)
    const GRAPH_SAMPLE_POINTS = 300;

    // 5. 유리함수(예: 1/x) 등 점근선 발산 시 그래프 왜곡을 막기 위한 y축 최대 절대값 제한
    const Y_CLIP_LIMIT = 10000;

    // 6. [모드 1] 자연어 문제 풀이 및 그래프 수식 추출 프롬프트
    const NATURAL_SYSTEM_PROMPT = `
당신은 고등학교 수학과 물리 문제를 분석하고 풀어주는 전문 AI 튜터입니다.
사용자가 문장으로 문제 조건을 입력하면 반드시 아래 4단계 형식으로 답변하세요.

1. **[주어진 조건 및 구할 값 정리]**: 문장에서 숫자, 단위, 물리량/수학적 조건을 기호(예: $m = 2\\text{ kg}$)로 명확히 추출하세요.
2. **[적용할 핵심 개념 및 공식]**: 문제 풀이에 필요한 수학 공식이나 물리 법칙을 제시하세요.
3. **[단계별 대입 및 풀이 과정]**: 공식에 조건을 대입하여 답을 도출하는 과정을 논리적으로 설명하세요.
4. **[최종 정답]**: 정확한 수치와 단위를 포함하여 최종 답을 제시하세요.

중요 규칙 1: 모든 수식과 기호는 반드시 LaTeX 문법(인라인은 $수식$, 블록은 $$수식$$)으로 작성하세요.
중요 규칙 2 (2D 그래프 자동 생성용 데이터):
문제가 수학 함수이거나 시간에 따른 물리량 변화(위치, 속도, 에너지 등)를 다루어 2D 그래프로 나타낼 수 있다면, 답변의 맨 마지막 줄에 아래 형식의 한 줄 코드를 정확히 추가하세요. (독립변수는 시간 t 대신 반드시 소문자 x로 통일하고, 곱하기는 * 기호를 명시하세요.)
형식: @@GRAPH: 수식 | 그래프제목 | 가로축이름 | 세로축이름 | x최소값 | x최대값@@
예시 (물리 포물선/연직 운동): @@GRAPH: 20*x - 5*x^2 | 시간에 따른 공의 높이 변화 | 시간 t (초) | 높이 h (m) | 0 | 4@@
예시 (이차함수): @@GRAPH: x^2 - 4*x + 3 | 이차함수 y = x^2 - 4x + 3 그래프 | x | y | -2 | 6@@
`;

    // 7. [모드 2] 수식 전용 해설 시스템 프롬프트
    const FORMULA_SYSTEM_PROMPT = `
당신은 수학 수식의 단계별 풀이 과정을 설명해 주는 AI 튜터입니다.
사용자가 입력한 수식과 기호 수학 엔진이 계산한 '정확한 계산 결과'가 함께 제공됩니다.
기호 수학 엔진의 계산 결과를 최종 정답으로 삼아, 처음 수식에서 어떻게 그 정답이 도출되는지 고등학생이 이해하기 쉽게 단계별 중간 풀이 과정을 LaTeX 수식($...$, $$...$$)으로 작성해 주세요.
`;

    // ==========================================
    // [화면 제어 및 수식 전처리 보조 함수]
    // ==========================================
    function switchTab(tabId, btnElement) {
        document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
        document.querySelectorAll('.tab-btn').forEach(el => el.classList.remove('active'));
        document.getElementById(tabId).classList.add('active');
        btnElement.classList.add('active');
    }

    function insertSymbol(symbol) {
        const input = document.getElementById('formulaInput');
        input.value += symbol;
        input.focus();
    }

    function renderMath() {
        if (window.MathJax && window.MathJax.typesetPromise) {
            window.MathJax.typesetPromise();
        }
    }

    /**
     * 사용자가 '2x^2 - 5x'처럼 곱하기(*)를 생략하고 입력해도
     * 컴퓨터가 이해할 수 있도록 '2*x^2 - 5*x'로 자동 보정하는 함수
     */
    function normalizeMathExpression(rawStr) {
        let s = rawStr.trim();
        // 숫자와 문자(또는 여는 괄호) 사이에 * 추가 (예: 2x -> 2*x, 3(x+1) -> 3*(x+1))
        s = s.replace(/(\d)([a-zA-Z\(])/g, '$1*$2');
        // 닫는 괄호와 숫자/문자/여는 괄호 사이에 * 추가 (예: (x+1)(x-1) -> (x+1)*(x-1))
        s = s.replace(/(\))([\da-zA-Z\(])/g, '$1*$2');
        return s;
    }

    // ==========================================
    // [핵심 함수: 수식을 2D 그래프용 (x, y) 좌표 배열로 변환]
    // ==========================================
    function sampleFunctionData(exprStr, xMin, xMax, numPoints = GRAPH_SAMPLE_POINTS) {
        const xValues = [];
        const yValues = [];

        // 등호(=)가 있는 방정식이면 '좌변 - (우변)' 형태로 만들어 y = f(x) 형태로 변환
        let cleanExpr = normalizeMathExpression(exprStr);
        if (cleanExpr.includes("=")) {
            const parts = cleanExpr.split("=");
            cleanExpr = `(${parts[0]}) - (${parts[1]})`;
        }

        // Nerdamer를 이용해 수식을 고속 계산 가능한 자바스크립트 함수로 컴파일
        const compiledFn = nerdamer(cleanExpr).buildFunction([DEFAULT_VARIABLE]);
        const step = (xMax - xMin) / (numPoints - 1);

        for (let i = 0; i < numPoints; i++) {
            const xVal = xMin + step * i;
            xValues.push(xVal);
            try {
                const yVal = compiledFn(xVal);
                // 실수 범위 내에 있고 무한대로 발산하지 않는 유효한 값만 기록
                if (typeof yVal === 'number' && Number.isFinite(yVal) && Math.abs(yVal) <= Y_CLIP_LIMIT) {
                    yValues.push(yVal);
                } else {
                    yValues.push(null); // 점근선이나 정의되지 않은 구간은 선을 끊음
                }
            } catch (e) {
                yValues.push(null);
            }
        }
        return { xValues, yValues, cleanExpr };
    }

    // ==========================================
    // [Plotly.js를 이용한 2D 그래프 그리기 함수]
    // ==========================================
    function draw2DGraph(targetDivId, traces, titleText, xAxisTitle = "x", yAxisTitle = "y") {
        const layout = {
            title: { text: titleText, font: { size: 16, weight: 'bold' } },
            xaxis: {
                title: xAxisTitle,
                zeroline: true,
                zerolinecolor: '#334155',
                zerolinewidth: 2,
                gridcolor: '#e2e8f0'
            },
            yaxis: {
                title: yAxisTitle,
                zeroline: true,
                zerolinecolor: '#334155',
                zerolinewidth: 2,
                gridcolor: '#e2e8f0'
            },
            margin: { t: 50, r: 30, b: 50, l: 55 },
            hovermode: 'closest',
            legend: { orientation: 'h', y: -0.2 }
        };

        const config = { responsive: true, displaylogo: false };
        Plotly.newPlot(targetDivId, traces, layout, config);
    }

    // ==========================================
    // [Gemini API 직접 호출 함수]
    // ==========================================
    async function callGeminiAPI(systemPrompt, userPrompt) {
        const apiKey = document.getElementById('apiKey').value.trim();
        if (!apiKey) {
            throw new Error("상단 설정 창에 Gemini API Key를 먼저 입력해 주세요!");
        }

        const url = `https://generativelanguage.googleapis.com/v1beta/models/${MODEL_NAME}:generateContent?key=${apiKey}`;
        const payload = {
            system_instruction: { parts: [{ text: systemPrompt }] },
            contents: [{ parts: [{ text: userPrompt }] }],
            generationConfig: { temperature: TEMPERATURE }
        };

        const response = await fetch(url, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(payload)
        });

        const data = await response.json();
        if (!response.ok) {
            throw new Error(data.error?.message || "API 호출 중 오류가 발생했습니다.");
        }
        return data.candidates[0].content.parts[0].text;
    }

    // ==========================================
    // [기능 1: 자연어 문제 조건 풀이 & 물리/수학 그래프 자동 시각화]
    // ==========================================
    async function solveNaturalLanguage() {
        const subject = document.getElementById('subjectSelect').value;
        const difficulty = document.getElementById('difficulty').value;
        const question = document.getElementById('naturalInput').value.trim();
        const resultDiv = document.getElementById('naturalResult');
        const graphWrapper = document.getElementById('naturalGraphWrapper');

        graphWrapper.style.display = "none";

        if (!question) {
            resultDiv.innerHTML = `<div class="error-msg" style="margin-top:14px;">⚠️ 문제 조건을 입력해 주세요!</div>`;
            return;
        }

        resultDiv.innerHTML = `<div class="result-box">🧠 AI가 문제의 조건과 공식을 분석하고 그래프 데이터를 구성하고 있습니다...</div>`;

        try {
            const prompt = `[과목: ${subject} / 설명 수준: ${difficulty}]\n학생이 입력한 문제 조건:\n${question}`;
            let aiResponse = await callGeminiAPI(NATURAL_SYSTEM_PROMPT, prompt);

            // AI 답변에서 @@GRAPH: ...@@ 태그가 있는지 정규표현식으로 추출
            const graphMatch = aiResponse.match(/@@GRAPH:\s*([^|]+)\|\s*([^|]+)\|\s*([^|]+)\|\s*([^|]+)\|\s*([^|]+)\|\s*([^@]+)@@/);
            
            // 화면에는 태그를 제외한 깔끔한 풀이 텍스트만 표시
            const cleanMarkdown = aiResponse.replace(/@@GRAPH:.*?@@/g, "").trim();
            resultDiv.innerHTML = `<div class="result-box">${marked.parse(cleanMarkdown)}</div>`;
            renderMath();

            // 추출된 그래프 수식이 있다면 2D 그래프를 자동으로 그리기
            if (graphMatch) {
                const exprStr = graphMatch[1].trim();
                const gTitle = graphMatch[2].trim();
                const xLabel = graphMatch[3].trim();
                const yLabel = graphMatch[4].trim();
                const xMin = parseFloat(graphMatch[5]) || 0;
                const xMax = parseFloat(graphMatch[6]) || 10;

                try {
                    const sampled = sampleFunctionData(exprStr, xMin, xMax);
                    const trace = {
                        x: sampled.xValues,
                        y: sampled.yValues,
                        mode: 'lines',
                        name: `수식: ${exprStr}`,
                        line: { color: '#2563eb', width: 3 }
                    };
                    graphWrapper.style.display = "block";
                    draw2DGraph('naturalGraphPlot', [trace], gTitle, xLabel, yLabel);
                } catch (gErr) {
                    console.warn("자연어 그래프 생성 생략:", gErr);
                }
            }
        } catch (err) {
            resultDiv.innerHTML = `<div class="error-msg" style="margin-top:14px;">❌ ${err.message}</div>`;
        }
    }

    // ==========================================
    // [기능 2: 수식만 넣기 (기호 수학 계산 + 다중 2D 그래프 + AI 해설)]
    // ==========================================
    async function solveFormulaOnly() {
        const op = document.getElementById('operationType').value;
        const rawInput = document.getElementById('formulaInput').value.trim();
        const difficulty = document.getElementById('difficulty').value;
        const xMin = parseFloat(document.getElementById('xMinInput').value) || -10;
        const xMax = parseFloat(document.getElementById('xMaxInput').value) || 10;
        
        const resultDiv = document.getElementById('formulaResult');
        const graphWrapper = document.getElementById('formulaGraphWrapper');

        if (!rawInput) {
            resultDiv.innerHTML = `<div class="error-msg" style="margin-top:14px;">⚠️ 계산할 수식을 입력해 주세요!</div>`;
            graphWrapper.style.display = "none";
            return;
        }

        if (xMin >= xMax) {
            resultDiv.innerHTML = `<div class="error-msg" style="margin-top:14px;">⚠️ x축 최소값(x_min)은 최대값(x_max)보다 작아야 합니다!</div>`;
            return;
        }

        const normFormula = normalizeMathExpression(rawInput);
        let calcResultText = "";
        let calcResultLatex = "";
        let opName = "";
        const traces = [];

        // 1단계: 기호 수학 엔진(Nerdamer) 계산 및 2D 그래프 데이터 생성
        try {
            // 원함수 f(x) 곡선 샘플링 및 추가 (파란색 실선)
            const baseSample = sampleFunctionData(normFormula, xMin, xMax);
            traces.push({
                x: baseSample.xValues,
                y: baseSample.yValues,
                mode: 'lines',
                name: `입력 함수 f(x) = ${baseSample.cleanExpr}`,
                line: { color: '#2563eb', width: 3 }
            });

            if (op === "solve") {
                opName = "방정식 해 구하기";
                const eq = normFormula.includes("=") ? normFormula : `${normFormula} = 0`;
                const sol = nerdamer.solveEquations(eq, DEFAULT_VARIABLE);
                calcResultText = sol.toString();
                calcResultLatex = `x = ${sol.toTeX()}`;

                // 방정식의 실근(해)을 찾아 그래프의 x축(y = 0) 위에 빨간색 마커(점)로 표시
                const rootStrings = calcResultText.replace(/[\[\]]/g, '').split(',');
                const rootX = [];
                const rootY = [];
                rootStrings.forEach(rStr => {
                    if (rStr.trim() !== "") {
                        try {
                            const val = Number(nerdamer(rStr).evaluate().text('decimals'));
                            if (Number.isFinite(val)) {
                                rootX.push(val);
                                rootY.push(0);
                            }
                        } catch (e) {}
                    }
                });

                if (rootX.length > 0) {
                    traces.push({
                        x: rootX,
                        y: rootY,
                        mode: 'markers',
                        name: '방정식의 실근 (y = 0 교점)',
                        marker: { color: '#dc2626', size: 11, symbol: 'circle' }
                    });
                }

            } else if (op === "diff") {
                opName = "미분 (도함수 d/dx)";
                const exprOnly = normFormula.split("=")[0];
                const res = nerdamer(`diff(${exprOnly}, ${DEFAULT_VARIABLE})`);
                calcResultText = res.toString();
                calcResultLatex = `f'(x) = ${res.toTeX()}`;

                // 미분된 도함수 f'(x) 곡선을 주황색 점선으로 함께 그리기
                const diffSample = sampleFunctionData(calcResultText, xMin, xMax);
                traces.push({
                    x: diffSample.xValues,
                    y: diffSample.yValues,
                    mode: 'lines',
                    name: `도함수 f'(x) = ${calcResultText}`,
                    line: { color: '#f97316', width: 2.5, dash: 'dash' }
                });

            } else if (op === "integrate") {
                opName = "부정적분 (∫ dx)";
                const exprOnly = normFormula.split("=")[0];
                const res = nerdamer(`integrate(${exprOnly}, ${DEFAULT_VARIABLE})`);
                const integralExpr = res.toString();
                calcResultText = integralExpr + " + C";
                calcResultLatex = `\\int f(x)dx = ${res.toTeX()} + C`;

                // 적분된 함수 F(x) (적분상수 C=0 기준) 곡선을 초록색 점선으로 함께 그리기
                const intSample = sampleFunctionData(integralExpr, xMin, xMax);
                traces.push({
                    x: intSample.xValues,
                    y: intSample.yValues,
                    mode: 'lines',
                    name: `부정적분 F(x) (C=0) = ${integralExpr}`,
                    line: { color: '#16a34a', width: 2.5, dash: 'dot' }
                });

            } else if (op === "factor") {
                opName = "인수분해 및 식 정리";
                const exprOnly = normFormula.split("=")[0];
                const res = nerdamer(`factor(${exprOnly})`);
                calcResultText = res.toString();
                calcResultLatex = res.toTeX();
            }

            // 2D 그래프 화면에 출력
            graphWrapper.style.display = "block";
            draw2DGraph('formulaGraphPlot', traces, `[${opName}] 2D 좌표평면 시각화`, "x", "y = f(x)");

        } catch (mathErr) {
            graphWrapper.style.display = "none";
            resultDiv.innerHTML = `<div class="error-msg" style="margin-top:14px;">⚠️ 수식 형식을 다시 확인해 주세요. (예: x^2 - 5x + 6 = 0)<br>상세 오류: ${mathErr.message}</div>`;
            return;
        }

        // 기호 수학 엔진의 계산 결과를 화면에 표시
        resultDiv.innerHTML = `
            <div class="result-box">
                <span class="sympy-badge">1️⃣ 기호 수학 엔진(Symbolic Engine) 정밀 계산 결과</span>
                <p style="margin: 6px 0;"><strong>수행 연산 (${opName}):</strong> $$${calcResultLatex}$$</p>
                <hr style="border: 0; border-top: 1px solid #cbd5e1; margin: 16px 0;">
                <div id="aiStepExplanation">🧠 <strong>2️⃣ AI 단계별 풀이 과정 생성 중...</strong> (API 키 입력 시 자동 생성됩니다)</div>
            </div>
        `;
        renderMath();

        // 2단계: API 키가 입력되어 있다면 AI에게 중간 풀이 과정 요청
        const apiKey = document.getElementById('apiKey').value.trim();
        const stepDiv = document.getElementById('aiStepExplanation');
        if (!apiKey) {
            stepDiv.innerHTML = `💡 <em>상단 설정 창에 Gemini API Key를 입력하시면 위 정답과 그래프가 도출되는 상세한 단계별 풀이 과정도 함께 볼 수 있습니다!</em>`;
            return;
        }

        try {
            const prompt = `입력된 수식: ${rawInput}\n수행할 연산: ${opName}\n기호 수학 엔진이 계산한 정확한 정답: ${calcResultText}\n설명 수준: ${difficulty}\n위 수식에서 어떻게 해당 정답이 나오는지 단계별 풀이 과정을 설명해 주세요.`;
            const aiResponse = await callGeminiAPI(FORMULA_SYSTEM_PROMPT, prompt);
            stepDiv.innerHTML = `<span class="sympy-badge">2️⃣ AI 단계별 중간 풀이 해설</span>` + marked.parse(aiResponse);
            renderMath();
        } catch (err) {
            stepDiv.innerHTML = `<div class="error-msg">❌ AI 해설 호출 오류: ${err.message}</div>`;
        }
    }
</script>

</body>
</html>
