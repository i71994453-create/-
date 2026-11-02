# GPAI C++ 계산 모듈

`gpai_calculations.hpp`는 C++17 표준 라이브러리만 사용하는 헤더 전용 계산 모듈입니다. 입력과 출력의 기본 단위는 SI입니다. 계산 함수는 잘못된 입력에 `std::invalid_argument`, 수치 오버플로에 `std::overflow_error`를 던집니다.

## 제공 범위

| 분야 | 함수 | 공식과 전제 |
|---|---|---|
| 미적분 | `derivative_central` | 중앙 유한차분 `(f(x+h)-f(x-h))/(2h)` |
| 미적분 | `integrate_simpson` | 합성 Simpson 적분. 구간 분할 수는 양의 짝수 |
| 미적분 | `root_bisection` | 연속 함수의 근을 이분법으로 계산. 구간 양 끝의 함수값은 서로 다른 부호여야 함 |
| 재료역학 | `axial_response` | 균일 단면·선형 탄성 축부재: `sigma=F/A`, `strain=sigma/E`, `deltaL=strain*L` |
| 재료역학 | `bending_stress` | 기초 보 이론: `sigma=M*y/I`. 단면의 중립축 거리와 단면 2차 모멘트를 입력 |
| 재료역학 | `solid_shaft_torsion` | 선형 탄성 실원형 축: `tau_max=16T/(pi*d^3)`, `theta=T*L/(J*G)`, `J=pi*d^4/32` |
| 열역학 | `ideal_gas_pressure_pa` | 이상기체: `P=nRT/V` |
| 열역학 | `first_law_delta_u_j` | `deltaU=Q-W`. Q는 계로 들어오는 열, W는 계가 한 일 |
| 열역학 | `ideal_gas_delta_u_j` | 이상기체: `deltaU=n*Cv*deltaT` |
| 열역학 | `isobaric_work_j` | 등압 경계일: `W=P*deltaV` |
| 열역학 | `carnot_efficiency` | 가역 카르노 기관: `eta=1-Tcold/Thot` |
| 기하 | `circle_*`, `sphere_*`, `cylinder_*`, `cuboid_*`, `triangle_area_m2` | 원·구·원기둥·직육면체와 Heron 공식으로 삼각형 계산 |

## 사용 예시

```cpp
#include "gpai_calculations.hpp"

#include <iostream>

int main() {
    const auto axial = gpai::mechanics_of_materials::axial_response(
        10'000.0,  // force: N
        1e-4,      // area: m^2
        2.0,       // length: m
        200e9      // Young's modulus: Pa
    );

    std::cout << "stress (Pa): " << axial.stress_pa << '\\n';
    std::cout << "elongation (m): " << axial.elongation_m << '\\n';

    const double area =
        gpai::geometry::circle_area_m2(0.5);  // radius: m
    std::cout << "circle area (m^2): " << area << '\\n';
}
```

컴파일 예시:

```sh
c++ -std=c++17 -Wall -Wextra -pedantic example.cpp -o example
```

## 단위와 해석 한계

- 길이 m, 면적 m², 부피 m³, 힘 N, 응력·탄성계수 Pa, 에너지 J, 압력 Pa, 토크 N·m, 몰수 mol을 사용합니다.
- 온도 입력은 절대온도 K입니다. 각도 결과는 rad입니다.
- 미적분 기능은 **수치 근사**입니다. 기호 미분·적분이나 모든 방정식의 해를 제공하지 않습니다.
- 재료역학 함수는 기본 선형 탄성 공식입니다. 좌굴, 소성, 피로, 응력 집중, 복합 하중, 실제 부재의 안전 설계를 대신하지 않습니다.
- 열역학 함수의 상태식은 이상기체 가정입니다. 실제 유체의 상태방정식이나 상변화는 포함하지 않습니다.
- 이 헤더는 계산 라이브러리이며 웹 UI 연결 및 WebAssembly JavaScript 바인딩은 별도 작업입니다. Emscripten에서 예외를 JavaScript로 전달하려면 해당 빌드 설정도 지정해야 합니다.

## 참고 자료

- UC Berkeley, [Math 128A Lecture Notes, Chapter 4: Numerical Differentiation and Integration](https://persson.berkeley.edu/math128a/lecture_notes/math128a_lecture_notes_chapter_4.html): 중앙차분과 합성 Simpson 적분 공식 및 근사 오차를 설명합니다.
- University of Colorado Boulder, [Introductory Engineering Design, Chapter 14: The Basics of Engineering Mechanics](https://www.colorado.edu/program/ide/sites/default/files/attached-files/introductory_engineering_design_textbook_0_0.pdf): 축응력·Hooke 법칙·축변형, 굽힘 응력, 원형축 비틀림 및 각변형 공식을 설명합니다.
- MIT OpenCourseWare, [2.017J Topics in Machine Elements](https://ocw.mit.edu/courses/2-017j-design-of-electromechanical-robotic-systems-fall-2009/16cb0f850752422026a85d838f30e340_MIT2_017JF09_machines.pdf): 원형축의 극관성모멘트, 비틀림각, 비틀림 및 굽힘 응력 공식을 요약합니다.
- UC Davis Physics 9B, [First Law and Related Results](https://kiskis.physics.ucdavis.edu/student/phy9b_11/fstl.pdf): 이상기체 상태방정식, 제1법칙의 부호 규약, `deltaU=n*Cv*deltaT`, 등압 일 공식을 정리합니다.
- University of Nebraska–Lincoln, [Thermodynamics formula sheet](https://physics.unl.edu/sites/unl.edu.cas.physics/files/media/file/DayIIExam_Aug24.pdf): 카르노 효율 관계를 포함한 열역학 요약 자료입니다.

이 참고 자료는 구현된 기초식의 출처입니다. 실제 설계 검토에서는 재료 데이터, 하중 조건, 적용 규격과 안전계수를 별도로 확인해야 합니다.
