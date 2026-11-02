"use strict";

const operations = [
  { group:"미적분", id:"gpai_calc_derivative_quadratic", name:"수치 미분 · 이차함수", formula:"중앙 유한차분: f′(x) ≈ [f(x+h) − f(x−h)] / 2h", inputs:[["x","평가 위치 x","",2],["a","이차항 계수 a","",1],["b","일차항 계수 b","",3],["c","상수항 c","",0],["step","간격 h","x와 같은 단위",0.001]], outputs:[["수치 미분값","함수 단위 / x 단위"]] },
  { group:"미적분", id:"gpai_calc_derivative_exact_quadratic", name:"해석 미분 · 이차함수", formula:"f(x)=ax²+bx+c의 도함수: f′(x)=2ax+b", inputs:[["x","평가 위치 x","",2],["a","이차항 계수 a","",1],["b","일차항 계수 b","",3]], outputs:[["도함수 값","함수 단위 / x 단위"]] },
  { group:"미적분", id:"gpai_calc_integral_simpson_quadratic", name:"수치 적분 · Simpson", formula:"합성 Simpson 적분. 구간 분할 수는 양의 짝수여야 합니다.", inputs:[["start","시작값","",0],["end","끝값","",2],["intervals","구간 분할 수","양의 짝수",100],["a","이차항 계수 a","",1],["b","일차항 계수 b","",0],["c","상수항 c","",0]], outputs:[["정적분 근삿값","함수 단위 × x 단위"]] },
  { group:"미적분", id:"gpai_calc_integral_exact_quadratic", name:"해석 적분 · 이차함수", formula:"f(x)=ax²+bx+c의 정적분을 원시함수로 계산합니다.", inputs:[["start","시작값","",0],["end","끝값","",2],["a","이차항 계수 a","",1],["b","일차항 계수 b","",0],["c","상수항 c","",0]], outputs:[["정적분 값","함수 단위 × x 단위"]] },

  { group:"재료역학", id:"gpai_mech_axial_response", name:"축응력 · 변형률 · 길이 변화", formula:"균일 단면·선형 탄성 가정: σ=F/A, ε=σ/E, ΔL=εL", inputs:[["force_n","축방향 힘","N",10000],["area_m2","단면적","m²",0.0001],["length_m","부재 길이","m",2],["young_modulus_pa","영률 E","Pa",200000000000]], outputs:[["응력 σ","Pa"],["변형률 ε","무차원"],["길이 변화 ΔL","m"]] },
  { group:"재료역학", id:"gpai_mech_cantilever_tip_deflection", name:"외팔보 끝단 처짐", formula:"끝단 집중하중·Euler–Bernoulli 보 가정: δ=FL³/(3EI)", inputs:[["load_n","끝단 하중 F","N",100],["length_m","보 길이 L","m",1],["young_modulus_pa","영률 E","Pa",200000000000],["second_moment_m4","단면 2차 모멘트 I","m⁴",0.000001]], outputs:[["끝단 처짐 δ","m"]] },
  { group:"재료역학", id:"gpai_mech_transverse_shear_stress", name:"보의 전단응력", formula:"기초 보 이론: τ=VQ/(It)", inputs:[["shear_force_n","전단력 V","N",1000],["first_moment_m3","단면 1차 모멘트 Q","m³",0.00001],["second_moment_m4","단면 2차 모멘트 I","m⁴",0.000001],["thickness_m","해당 위치의 두께 t","m",0.01]], outputs:[["전단응력 τ","Pa"]] },
  { group:"재료역학", id:"gpai_mech_bending_stress", name:"굽힘 응력", formula:"탄성 보 이론: σ=My/I", inputs:[["moment_nm","굽힘 모멘트 M","N·m",100],["distance_m","중립축에서 거리 y","m",0.05],["second_moment_m4","단면 2차 모멘트 I","m⁴",0.000001]], outputs:[["굽힘 응력 σ","Pa"]] },
  { group:"재료역학", id:"gpai_mech_solid_shaft_torsion", name:"실원형 축 비틀림", formula:"선형 탄성 실축: τmax=16T/(πd³), θ=TL/(JG)", inputs:[["torque_nm","토크 T","N·m",10],["diameter_m","축 지름 d","m",0.02],["length_m","축 길이 L","m",1],["shear_modulus_pa","전단탄성계수 G","Pa",80000000000]], outputs:[["최대 전단응력 τmax","Pa"],["비틀림각 θ","rad"]] },
  { group:"재료역학", id:"gpai_mech_circular_second_moment", name:"원형 단면 2차 모멘트", formula:"실원형 단면의 중심축 기준: I=πd⁴/64", inputs:[["diameter_m","지름 d","m",0.02]], outputs:[["단면 2차 모멘트 I","m⁴"]] },
  { group:"재료역학", id:"gpai_mech_circular_polar_moment", name:"원형 단면 극관성 모멘트", formula:"실원형 단면의 극관성 모멘트: J=πd⁴/32", inputs:[["diameter_m","지름 d","m",0.02]], outputs:[["극관성 모멘트 J","m⁴"]] },
  { group:"재료역학", id:"gpai_mech_safety_factor", name:"응력 기준 안전율", formula:"기초 비율: 안전율 = 허용 응력 / 실제 응력", inputs:[["allowable_stress_pa","허용 응력","Pa",250000000],["actual_stress_pa","실제 응력","Pa",125000000]], outputs:[["안전율","무차원"]] },

  { group:"열역학", id:"gpai_thermo_ideal_gas_pressure", name:"이상기체 압력", formula:"이상기체 상태식: P=nRT/V, R=8.314462618 J/(mol·K)", inputs:[["amount_mol","몰수 n","mol",1],["temperature_k","절대온도 T","K",300],["volume_m3","부피 V","m³",0.025]], outputs:[["압력 P","Pa"]] },
  { group:"열역학", id:"gpai_thermo_ideal_gas_volume", name:"이상기체 부피", formula:"이상기체 상태식에서 V=nRT/P를 계산합니다.", inputs:[["amount_mol","몰수 n","mol",1],["temperature_k","절대온도 T","K",300],["pressure_pa","압력 P","Pa",101325]], outputs:[["부피 V","m³"]] },
  { group:"열역학", id:"gpai_thermo_ideal_gas_temperature", name:"이상기체 절대온도", formula:"이상기체 상태식에서 T=PV/(nR)를 계산합니다.", inputs:[["pressure_pa","압력 P","Pa",101325],["volume_m3","부피 V","m³",0.025],["amount_mol","몰수 n","mol",1]], outputs:[["절대온도 T","K"]] },
  { group:"열역학", id:"gpai_thermo_heat", name:"비열로 열량 계산", formula:"온도 변화가 있는 현열: Q=mcΔT", inputs:[["mass_kg","질량 m","kg",1],["specific_heat_j_per_kg_k","비열 c","J/(kg·K)",4184],["delta_temperature_k","온도 변화 ΔT","K",10]], outputs:[["열량 Q","J"]] },
  { group:"열역학", id:"gpai_thermo_first_law_delta_u", name:"열역학 제1법칙", formula:"부호 규약: Q는 계로 들어온 열, W는 계가 한 일. ΔU=Q−W", inputs:[["heat_into_system_j","계로 들어온 열 Q","J",500],["work_by_system_j","계가 한 일 W","J",200]], outputs:[["내부에너지 변화 ΔU","J"]] },
  { group:"열역학", id:"gpai_thermo_isobaric_work", name:"등압 경계일", formula:"압력이 일정할 때 계가 한 일: W=P(V₂−V₁)", inputs:[["pressure_pa","압력 P","Pa",100000],["initial_volume_m3","초기 부피 V₁","m³",0.01],["final_volume_m3","최종 부피 V₂","m³",0.02]], outputs:[["경계일 W","J"]] },
  { group:"열역학", id:"gpai_thermo_carnot_efficiency_percent", name:"카르노 효율", formula:"가역 카르노 기관: η=100×(1−Tc/Th), 두 온도 모두 K", inputs:[["cold_reservoir_k","저온 열원 Tc","K",300],["hot_reservoir_k","고온 열원 Th","K",600]], outputs:[["최대 열효율 η","%"]] },
  { group:"열역학", id:"gpai_thermo_enthalpy", name:"엔탈피", formula:"정의식: H=U+PV", inputs:[["internal_energy_j","내부에너지 U","J",1000],["pressure_pa","압력 P","Pa",100000],["volume_m3","부피 V","m³",0.01]], outputs:[["엔탈피 H","J"]] },
  { group:"열역학", id:"gpai_thermo_ideal_gas_delta_u", name:"이상기체 내부에너지 변화", formula:"정적 몰비열 사용: ΔU=nCvΔT", inputs:[["amount_mol","몰수 n","mol",1],["molar_cv_j_per_mol_k","정적 몰비열 Cv","J/(mol·K)",20.8],["delta_temperature_k","온도 변화 ΔT","K",10]], outputs:[["내부에너지 변화 ΔU","J"]] },
  { group:"열역학", id:"gpai_thermo_ideal_gas_delta_h", name:"이상기체 엔탈피 변화", formula:"정압 몰비열 사용: ΔH=nCpΔT", inputs:[["amount_mol","몰수 n","mol",1],["molar_cp_j_per_mol_k","정압 몰비열 Cp","J/(mol·K)",29.1],["delta_temperature_k","온도 변화 ΔT","K",10]], outputs:[["엔탈피 변화 ΔH","J"]] },

  { group:"기하", id:"gpai_geometry_circle_area", name:"원 넓이", formula:"A=πr²", inputs:[["radius_m","반지름 r","m",0.5]], outputs:[["넓이 A","m²"]] },
  { group:"기하", id:"gpai_geometry_circle_circumference", name:"원 둘레", formula:"C=2πr", inputs:[["radius_m","반지름 r","m",0.5]], outputs:[["둘레 C","m"]] },
  { group:"기하", id:"gpai_geometry_cylinder_volume", name:"원기둥 부피", formula:"V=πr²h", inputs:[["radius_m","밑면 반지름 r","m",0.5],["height_m","높이 h","m",2]], outputs:[["부피 V","m³"]] },
  { group:"기하", id:"gpai_geometry_sphere_volume", name:"구 부피", formula:"V=4πr³/3", inputs:[["radius_m","반지름 r","m",0.5]], outputs:[["부피 V","m³"]] },
  { group:"기하", id:"gpai_geometry_sphere_surface_area", name:"구 표면적", formula:"A=4πr²", inputs:[["radius_m","반지름 r","m",0.5]], outputs:[["표면적 A","m²"]] },
  { group:"기하", id:"gpai_geometry_cuboid_volume", name:"직육면체 부피", formula:"V=길이×너비×높이", inputs:[["length_m","길이","m",2],["width_m","너비","m",1],["height_m","높이","m",0.5]], outputs:[["부피 V","m³"]] },
  { group:"기하", id:"gpai_geometry_triangle_area", name:"삼각형 넓이", formula:"A=밑변×높이/2", inputs:[["base_m","밑변","m",3],["height_m","수직 높이","m",4]], outputs:[["넓이 A","m²"]] },
  { group:"기하", id:"gpai_geometry_hypotenuse", name:"직각삼각형 빗변", formula:"피타고라스 정리: c=√(a²+b²)", inputs:[["side_a_m","직각변 a","m",3],["side_b_m","직각변 b","m",4]], outputs:[["빗변 c","m"]] },
  { group:"기하", id:"gpai_geometry_distance_2d", name:"2차원 두 점 거리", formula:"d=√[(x₂−x₁)²+(y₂−y₁)²]", inputs:[["x1_m","첫 점 x₁","m",0],["y1_m","첫 점 y₁","m",0],["x2_m","둘째 점 x₂","m",3],["y2_m","둘째 점 y₂","m",4]], outputs:[["거리 d","m"]] },
  { group:"기하", id:"gpai_geometry_distance_3d", name:"3차원 두 점 거리", formula:"d=√[(x₂−x₁)²+(y₂−y₁)²+(z₂−z₁)²]", inputs:[["x1_m","첫 점 x₁","m",0],["y1_m","첫 점 y₁","m",0],["z1_m","첫 점 z₁","m",0],["x2_m","둘째 점 x₂","m",1],["y2_m","둘째 점 y₂","m",2],["z2_m","둘째 점 z₂","m",2]], outputs:[["거리 d","m"]] },
  { group:"기하", id:"gpai_geometry_dot_product_3d", name:"3차원 벡터 내적", formula:"u·v=uₓvₓ+uᵧvᵧ+u_zv_z", inputs:[["ux","벡터 u의 x 성분","",1],["uy","벡터 u의 y 성분","",0],["uz","벡터 u의 z 성분","",0],["vx","벡터 v의 x 성분","",0],["vy","벡터 v의 y 성분","",1],["vz","벡터 v의 z 성분","",0]], outputs:[["내적 u·v","성분 단위의 곱"]] },
  { group:"기하", id:"gpai_geometry_vector_magnitude_3d", name:"3차원 벡터 크기", formula:"|v|=√(x²+y²+z²)", inputs:[["x","x 성분","",1],["y","y 성분","",2],["z","z 성분","",2]], outputs:[["벡터 크기 |v|","성분과 같은 단위"]] },
  { group:"기하", id:"gpai_geometry_vector_angle_3d_degrees", name:"3차원 벡터 사이각", formula:"cos θ=(u·v)/(|u||v|), 결과는 도(degree)", inputs:[["ux","벡터 u의 x 성분","",1],["uy","벡터 u의 y 성분","",0],["uz","벡터 u의 z 성분","",0],["vx","벡터 v의 x 성분","",0],["vy","벡터 v의 y 성분","",1],["vz","벡터 v의 z 성분","",0]], outputs:[["사이각 θ","°"]] }
];

const statusMessage = {
  1: "입력값을 확인하세요. 양수 조건이나 Simpson 짝수 분할 조건을 확인해 주세요.",
  2: "계산 결과가 숫자 범위를 벗어났습니다.",
  3: "계산 코어에서 처리하지 못한 오류가 발생했습니다."
};

let engine = null;
let model = null;
let vocabulary = null;

const byId = (id) => document.getElementById(id);
const domainSelect = byId("domain");
const operationSelect = byId("operation");
const inputFields = byId("input-fields");
const calculateButton = byId("calculate-button");

function setStatus(element, text, state) {
  element.textContent = text;
  element.className = "status" + (state ? " " + state : "");
}

function renderOperations() {
  const selectedDomain = domainSelect.value;
  const available = operations.filter((item) => item.group === selectedDomain);
  operationSelect.replaceChildren();
  for (const item of available) {
    const option = document.createElement("option");
    option.value = item.id;
    option.textContent = item.name;
    operationSelect.append(option);
  }
  renderInputs();
}

function renderInputs() {
  const item = operations.find((entry) => entry.id === operationSelect.value);
  inputFields.replaceChildren();
  byId("formula-note").textContent = item ? item.formula : "";
  if (!item) return;

  item.inputs.forEach(([name, label, unit, initial]) => {
    const wrapper = document.createElement("div");
    wrapper.className = "field";
    const inputId = "input-" + name;
    const labelElement = document.createElement("label");
    labelElement.htmlFor = inputId;
    labelElement.textContent = label;
    const input = document.createElement("input");
    input.id = inputId;
    input.name = name;
    input.type = "number";
    input.step = "any";
    input.required = true;
    input.value = String(initial);
    const hint = document.createElement("small");
    hint.textContent = unit ? "단위: " + unit : "무차원 값";
    wrapper.append(labelElement, input, hint);
    inputFields.append(wrapper);
  });
}

function modelFeatures(text) {
  const normalized = text.normalize("NFKC").toLocaleLowerCase("ko");
  const words = normalized.match(/[\uAC00-\uD7A3]+|[a-z]+|[0-9]+(?:\.[0-9]+)?/g) || [];
  const result = [];
  for (const word of words) {
    result.push("w:" + word);
    if (/^[\uAC00-\uD7A3]+$/.test(word)) {
      const chars = Array.from(word);
      for (const width of [2, 3]) {
        for (let i = 0; i + width <= chars.length; i += 1) {
          result.push("c" + width + ":" + chars.slice(i, i + width).join(""));
        }
      }
    }
  }
  return result;
}

function classifyText(text) {
  const tokens = modelFeatures(text).filter((token) => vocabulary.has(token));
  const scores = {};
  for (const label of model.labels) {
    const likelihood = model.logLikelihood[label];
    scores[label] = model.logPrior[label] + tokens.reduce((sum, token) => sum + likelihood[token], 0);
  }
  const maxScore = Math.max(...Object.values(scores));
  const expScores = Object.fromEntries(
    Object.entries(scores).map(([label, score]) => [label, Math.exp(score - maxScore)])
  );
  const total = Object.values(expScores).reduce((sum, value) => sum + value, 0);
  const ranking = Object.entries(expScores)
    .map(([label, score]) => ({ label, score: score / total }))
    .sort((a, b) => b.score - a.score);
  return ranking;
}

function selectDomainFor(label) {
  domainSelect.value = label;
  renderOperations();
}

function showClassification() {
  const text = byId("question").value.trim();
  const result = byId("classification-result");
  if (!text) {
    result.textContent = "분야를 찾을 문장이나 문제를 입력하세요.";
    return;
  }
  if (!model) {
    result.textContent = "분류 모델이 준비되지 않았습니다. 아래 계산 메뉴는 직접 선택해 사용할 수 있습니다.";
    return;
  }
  const [best, runnerUp] = classifyText(text);
  result.replaceChildren();
  const line = document.createElement("div");
  const label = document.createElement("strong");
  label.textContent = "분류 분야: " + best.label;
  const score = document.createElement("span");
  score.className = "score";
  score.textContent = "상대 모델 점수 " + (best.score * 100).toFixed(1) + "%";
  line.append(label, score);
  if (runnerUp) {
    const other = document.createElement("span");
    other.className = "score";
    other.textContent = "다음 후보 " + runnerUp.label;
    line.append(other);
  }
  const useButton = document.createElement("button");
  useButton.className = "use-domain";
  useButton.type = "button";
  useButton.textContent = "이 분야 계산기로 이동";
  useButton.addEventListener("click", () => selectDomainFor(best.label));
  line.append(useButton);
  result.append(line);
}

async function loadModel() {
  const status = byId("model-status");
  try {
    const response = await fetch("assets/domain-model.json", { cache: "no-cache" });
    if (!response.ok) throw new Error("모델 파일 HTTP " + response.status);
    model = await response.json();
    if (model.formatVersion !== 1 || !Array.isArray(model.labels)) {
      throw new Error("지원하지 않는 모델 형식");
    }
    vocabulary = new Set(model.vocabulary);
    const validation = model.validation;
    setStatus(status, "Python 학습 모델 준비 완료 · 검증 " +
      validation.correct + "/" + validation.examples, "ready");
  } catch (error) {
    setStatus(status, "분류 모델을 불러오지 못함", "error");
    console.error(error);
  }
}

async function loadEngine() {
  const status = byId("engine-status");
  try {
    if (typeof createGpaiModule !== "function") {
      throw new Error("WebAssembly 파일이 페이지에 포함되지 않았습니다.");
    }
    engine = await createGpaiModule();
    setStatus(status, "C++ WebAssembly 준비 완료", "ready");
    calculateButton.disabled = false;
    byId("sort-button").disabled = false;
    setStatus(byId("sort-status"), "C++ 정렬 엔진 준비 완료", "ready");
  } catch (error) {
    setStatus(status, "C++ 엔진을 불러오지 못함", "error");
    byId("calculation-result").textContent =
      "브라우저가 WebAssembly 파일을 불러오지 못했습니다. 배포 빌드가 완료됐는지 확인하세요.";
    setStatus(byId("sort-status"), "C++ 엔진을 불러오지 못함", "error");
    console.error(error);
  }
}

function formatNumber(value) {
  if (!Number.isFinite(value)) return String(value);
  if (value !== 0 && (Math.abs(value) < 1e-6 || Math.abs(value) >= 1e9)) {
    return value.toExponential(6);
  }
  return new Intl.NumberFormat("ko-KR", { maximumSignificantDigits: 9 }).format(value);
}

function renderError(message) {
  const box = byId("calculation-result");
  box.replaceChildren();
  const error = document.createElement("span");
  error.className = "result-error";
  error.textContent = message;
  box.append(error);
}

function calculate(event) {
  event.preventDefault();
  if (!engine) {
    renderError("C++ 계산 엔진이 아직 준비되지 않았습니다.");
    return;
  }
  const item = operations.find((entry) => entry.id === operationSelect.value);
  if (!item) {
    renderError("계산 항목을 찾을 수 없습니다.");
    return;
  }

  const inputs = Array.from(inputFields.querySelectorAll("input"));
  const values = inputs.map((input) => Number(input.value));
  if (inputs.some((input) => input.value.trim() === "") || values.some((value) => !Number.isFinite(value))) {
    renderError("모든 입력란에 유한한 숫자를 입력하세요.");
    return;
  }
  if (item.id === "gpai_calc_integral_simpson_quadratic" &&
      (!Number.isInteger(values[2]) || values[2] <= 0 || values[2] % 2 !== 0)) {
    renderError("Simpson 구간 분할 수는 양의 짝수여야 합니다.");
    return;
  }

  const bytes = item.outputs.length * 8;
  const outputPointer = engine._malloc(bytes);
  if (!outputPointer) {
    renderError("계산 메모리를 확보하지 못했습니다.");
    return;
  }
  try {
    const status = engine.ccall(
      item.id,
      "number",
      Array(values.length).fill("number").concat("number"),
      values.concat(outputPointer)
    );
    if (status !== 0) {
      renderError(statusMessage[status] || "계산 오류 코드: " + status);
      return;
    }

    const outputs = item.outputs.map(([label, unit], index) => ({
      label, unit, value: engine.getValue(outputPointer + index * 8, "double")
    }));
    const box = byId("calculation-result");
    box.replaceChildren();
    for (const output of outputs) {
      const row = document.createElement("div");
      row.className = "result-row";
      const name = document.createElement("span");
      name.textContent = output.label;
      const value = document.createElement("strong");
      value.textContent = formatNumber(output.value) + (output.unit ? " " + output.unit : "");
      row.append(name, value);
      box.append(row);
    }
  } catch (error) {
    renderError("계산 실행 중 오류가 발생했습니다. " + error.message);
  } finally {
    engine._free(outputPointer);
  }
}


function apiBaseUrl() {
  let value = (window.GPAI_API_BASE_URL || "").trim();
  while (value.endsWith("/")) value = value.slice(0, -1);
  return value;
}

function setupAiCalculator() {
  const button = byId("ai-solve-button");
  const status = byId("ai-status");
  if (!apiBaseUrl()) {
    button.disabled = true;
    setStatus(status, "AI 서버 주소 설정 필요", "error");
    byId("ai-result").textContent =
      "site/assets/config.js에 배포된 GPAI API 주소를 설정하면 AI 계산을 사용할 수 있습니다.";
    return;
  }
  button.disabled = false;
  setStatus(status, "AI 서버 연결 설정됨", "ready");
  button.addEventListener("click", solveWithAi);
}

function renderAiResponse(payload) {
  const box = byId("ai-result");
  box.replaceChildren();
  const answer = document.createElement("p");
  answer.className = "ai-answer";
  answer.textContent = payload.answer || "AI 응답이 비어 있습니다.";
  box.append(answer);

  for (const calculation of payload.calculations || []) {
    const card = document.createElement("div");
    card.className = "ai-calculation";
    const heading = document.createElement("strong");
    heading.textContent = calculation.name + " · " + calculation.formula;
    card.append(heading);
    for (const output of calculation.outputs || []) {
      const row = document.createElement("div");
      row.className = "result-row";
      const label = document.createElement("span");
      label.textContent = output.label;
      const value = document.createElement("strong");
      value.textContent = formatNumber(output.value) + (output.unit ? " " + output.unit : "");
      row.append(label, value);
      card.append(row);
    }
    box.append(card);
  }
}

async function solveWithAi() {
  const button = byId("ai-solve-button");
  const status = byId("ai-status");
  const question = byId("ai-question").value.trim();
  if (question.length < 4) {
    byId("ai-result").textContent = "문제와 필요한 값을 4자 이상 입력하세요.";
    return;
  }
  button.disabled = true;
  setStatus(status, "AI가 입력을 해석하고 C++로 계산 중", "");
  byId("ai-result").textContent = "계산을 요청했습니다. 잠시 기다려 주세요.";
  try {
    const response = await fetch(apiBaseUrl() + "/api/solve", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ question })
    });
    const payload = await response.json();
    if (!response.ok) throw new Error(payload.detail || "HTTP " + response.status);
    renderAiResponse(payload);
    setStatus(status, "응답 완료 · C++ 계산값 포함", "ready");
  } catch (error) {
    byId("ai-result").textContent = "요청 실패: " + error.message;
    setStatus(status, "AI 서버 오류", "error");
  } finally {
    button.disabled = false;
  }
}


function sortValuesWithCpp() {
  const output = byId("sort-result");
  const tokens = byId("sort-values").value.trim().split(/[\\s,;]+/).filter(Boolean);
  if (tokens.length === 0) {
    output.textContent = "정렬할 숫자를 입력하세요.";
    return;
  }
  if (tokens.length > 1000) {
    output.textContent = "한 번에 최대 1,000개까지 정렬할 수 있습니다.";
    return;
  }
  const values = tokens.map(Number);
  if (values.some((value) => !Number.isFinite(value))) {
    output.textContent = "모든 항목에 유한한 숫자를 입력하세요.";
    return;
  }
  if (!engine) {
    output.textContent = "C++ WebAssembly 엔진이 아직 준비되지 않았습니다.";
    return;
  }

  const pointer = engine._malloc(values.length * 8);
  if (!pointer) {
    output.textContent = "정렬 메모리를 확보하지 못했습니다.";
    return;
  }
  try {
    values.forEach((value, index) => engine.setValue(pointer + index * 8, value, "double"));
    const descending = byId("sort-order").value === "descending" ? 1 : 0;
    const status = engine.ccall(
      "gpai_engine_insertion_sort",
      "number",
      ["number", "number", "number"],
      [pointer, values.length, descending]
    );
    if (status !== 0) {
      output.textContent = status === 1 ? "정렬 입력을 확인하세요." : "C++ 정렬 오류 코드: " + status;
      return;
    }
    const sorted = values.map((_, index) => engine.getValue(pointer + index * 8, "double"));
    output.textContent = sorted.map(formatNumber).join(", ");
    setStatus(byId("sort-status"), "삽입 정렬 완료", "ready");
  } catch (error) {
    output.textContent = "정렬 실행 중 오류가 발생했습니다.";
    setStatus(byId("sort-status"), "정렬 오류", "error");
    console.error(error);
  } finally {
    engine._free(pointer);
  }
}

function init() {
  renderOperations();
  domainSelect.addEventListener("change", renderOperations);
  operationSelect.addEventListener("change", renderInputs);
  byId("classify-button").addEventListener("click", showClassification);
  byId("question").addEventListener("keydown", (event) => {
    if ((event.ctrlKey || event.metaKey) && event.key === "Enter") showClassification();
  });
  byId("calculator-form").addEventListener("submit", calculate);
  byId("sort-button").addEventListener("click", sortValuesWithCpp);
  setupAiCalculator();
  loadEngine();
  loadModel();
}

document.addEventListener("DOMContentLoaded", init);
