#pragma once

#include <cmath>
#include <cstddef>
#include <functional>
#include <stdexcept>

namespace gpai {
namespace detail {

inline void require_finite(double value, const char* name) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument(name);
    }
}

inline void require_positive(double value, const char* name) {
    require_finite(value, name);
    if (value <= 0.0) {
        throw std::invalid_argument(name);
    }
}

inline double finite_result(double value, const char* message) {
    if (!std::isfinite(value)) {
        throw std::overflow_error(message);
    }
    return value;
}

}  // namespace detail

constexpr double pi = 3.141592653589793238462643383279502884;
constexpr double universal_gas_constant_j_per_mol_k = 8.31446261815324;

namespace calculus {

using Function = std::function<double(double)>;

// Central finite difference. h is a step in the same unit as x.
inline double derivative_central(const Function& f, double x, double h = 1e-5) {
    if (!f) {
        throw std::invalid_argument("function must be callable");
    }
    detail::require_finite(x, "x must be finite");
    detail::require_positive(h, "h must be positive and finite");

    const double left = f(x - h);
    const double right = f(x + h);
    detail::require_finite(left, "function returned a non-finite value");
    detail::require_finite(right, "function returned a non-finite value");
    return detail::finite_result((right - left) / (2.0 * h),
                                 "derivative result is not finite");
}

// Composite Simpson integration. n must be a positive even number.
// Reversing a and b reverses the sign of the result.
inline double integrate_simpson(const Function& f, double a, double b,
                                std::size_t n = 1000) {
    if (!f) {
        throw std::invalid_argument("function must be callable");
    }
    detail::require_finite(a, "a must be finite");
    detail::require_finite(b, "b must be finite");
    if (n == 0 || n % 2 != 0) {
        throw std::invalid_argument("n must be a positive even number");
    }
    if (a == b) {
        return 0.0;
    }

    const double step = (b - a) / static_cast<double>(n);
    double sum = 0.0;
    for (std::size_t i = 0; i <= n; ++i) {
        const double x = a + step * static_cast<double>(i);
        const double y = f(x);
        detail::require_finite(y, "function returned a non-finite value");
        const double weight = (i == 0 || i == n) ? 1.0 : (i % 2 == 0 ? 2.0 : 4.0);
        sum += weight * y;
    }
    return detail::finite_result((step / 3.0) * sum,
                                 "integral result is not finite");
}

// Bisection requires a continuous function on [a,b] and a sign change.
// tolerance is measured in x units.
inline double root_bisection(const Function& f, double a, double b,
                             double tolerance = 1e-10,
                             std::size_t max_iterations = 1000) {
    if (!f) {
        throw std::invalid_argument("function must be callable");
    }
    detail::require_finite(a, "a must be finite");
    detail::require_finite(b, "b must be finite");
    detail::require_positive(tolerance, "tolerance must be positive and finite");
    if (a >= b) {
        throw std::invalid_argument("a must be less than b");
    }
    if (max_iterations == 0) {
        throw std::invalid_argument("max_iterations must be positive");
    }

    double fa = f(a);
    double fb = f(b);
    detail::require_finite(fa, "function returned a non-finite value");
    detail::require_finite(fb, "function returned a non-finite value");
    if (fa == 0.0) return a;
    if (fb == 0.0) return b;
    if (std::signbit(fa) == std::signbit(fb)) {
        throw std::invalid_argument("endpoints must bracket a root");
    }

    for (std::size_t i = 0; i < max_iterations; ++i) {
        const double midpoint = a + (b - a) * 0.5;
        const double fm = f(midpoint);
        detail::require_finite(fm, "function returned a non-finite value");
        if (fm == 0.0 || std::abs(b - a) <= tolerance) {
            return midpoint;
        }

        if (std::signbit(fa) != std::signbit(fm)) {
            b = midpoint;
        } else {
            a = midpoint;
            fa = fm;
        }
    }
    throw std::runtime_error("bisection did not converge within max_iterations");
}

}  // namespace calculus

namespace mechanics_of_materials {

struct AxialResponse {
    double stress_pa;
    double strain;
    double elongation_m;
};

// Uniform axial load on a prismatic, linearly elastic member.
inline AxialResponse axial_response(double force_n, double area_m2,
                                    double length_m, double young_modulus_pa) {
    detail::require_finite(force_n, "force must be finite");
    detail::require_positive(area_m2, "area must be positive");
    detail::require_positive(length_m, "length must be positive");
    detail::require_positive(young_modulus_pa, "Young's modulus must be positive");

    const double stress = force_n / area_m2;
    const double strain = stress / young_modulus_pa;
    const double elongation = strain * length_m;
    return {
        detail::finite_result(stress, "stress result is not finite"),
        detail::finite_result(strain, "strain result is not finite"),
        detail::finite_result(elongation, "elongation result is not finite")
    };
}

// Elementary beam theory: sigma = M*y/I. y may be signed to indicate side.
inline double bending_stress(double moment_nm, double distance_from_neutral_axis_m,
                             double second_moment_of_area_m4) {
    detail::require_finite(moment_nm, "moment must be finite");
    detail::require_finite(distance_from_neutral_axis_m, "distance must be finite");
    detail::require_positive(second_moment_of_area_m4,
                             "second moment of area must be positive");
    return detail::finite_result(
        moment_nm * distance_from_neutral_axis_m / second_moment_of_area_m4,
        "bending stress result is not finite");
}

struct SolidShaftTorsion {
    double maximum_shear_stress_pa;
    double twist_angle_rad;
};

// Saint-Venant elastic torsion for a solid circular shaft.
inline SolidShaftTorsion solid_shaft_torsion(double torque_nm, double diameter_m,
                                             double length_m,
                                             double shear_modulus_pa) {
    detail::require_finite(torque_nm, "torque must be finite");
    detail::require_positive(diameter_m, "diameter must be positive");
    detail::require_positive(length_m, "length must be positive");
    detail::require_positive(shear_modulus_pa, "shear modulus must be positive");

    const double polar_moment = pi * std::pow(diameter_m, 4) / 32.0;
    const double max_shear = 16.0 * torque_nm /
                             (pi * std::pow(diameter_m, 3));
    const double twist = torque_nm * length_m /
                         (polar_moment * shear_modulus_pa);
    return {
        detail::finite_result(max_shear, "shear stress result is not finite"),
        detail::finite_result(twist, "twist angle result is not finite")
    };
}

}  // namespace mechanics_of_materials

namespace thermodynamics {

// Ideal-gas equation: P = n*R*T/V. Temperature is absolute temperature in K.
inline double ideal_gas_pressure_pa(double amount_mol, double temperature_k,
                                    double volume_m3) {
    detail::require_positive(amount_mol, "amount must be positive");
    detail::require_positive(temperature_k, "temperature must be positive in K");
    detail::require_positive(volume_m3, "volume must be positive");
    return detail::finite_result(
        amount_mol * universal_gas_constant_j_per_mol_k * temperature_k / volume_m3,
        "pressure result is not finite");
}

// Sign convention: heat entering the system is positive;
// work done by the system is positive. First law: delta U = Q - W.
inline double first_law_delta_u_j(double heat_into_system_j,
                                  double work_by_system_j) {
    detail::require_finite(heat_into_system_j, "heat must be finite");
    detail::require_finite(work_by_system_j, "work must be finite");
    return detail::finite_result(heat_into_system_j - work_by_system_j,
                                 "internal-energy result is not finite");
}

// Ideal-gas internal-energy change: delta U = n*Cv*delta T.
inline double ideal_gas_delta_u_j(double amount_mol,
                                  double molar_cv_j_per_mol_k,
                                  double delta_temperature_k) {
    detail::require_positive(amount_mol, "amount must be positive");
    detail::require_positive(molar_cv_j_per_mol_k,
                             "molar heat capacity must be positive");
    detail::require_finite(delta_temperature_k, "temperature change must be finite");
    return detail::finite_result(
        amount_mol * molar_cv_j_per_mol_k * delta_temperature_k,
        "internal-energy result is not finite");
}

// Constant-pressure boundary work: W = P*delta V, positive for expansion.
inline double isobaric_work_j(double pressure_pa, double delta_volume_m3) {
    detail::require_positive(pressure_pa, "pressure must be positive");
    detail::require_finite(delta_volume_m3, "volume change must be finite");
    return detail::finite_result(pressure_pa * delta_volume_m3,
                                 "work result is not finite");
}

// Carnot efficiency for a reversible engine between two reservoirs.
inline double carnot_efficiency(double cold_reservoir_k,
                                double hot_reservoir_k) {
    detail::require_positive(cold_reservoir_k,
                             "cold-reservoir temperature must be positive in K");
    detail::require_positive(hot_reservoir_k,
                             "hot-reservoir temperature must be positive in K");
    if (cold_reservoir_k >= hot_reservoir_k) {
        throw std::invalid_argument("hot-reservoir temperature must exceed cold");
    }
    return 1.0 - cold_reservoir_k / hot_reservoir_k;
}

}  // namespace thermodynamics

namespace geometry {

inline double circle_area_m2(double radius_m) {
    detail::require_positive(radius_m, "radius must be positive");
    return detail::finite_result(pi * radius_m * radius_m,
                                 "circle area result is not finite");
}

inline double circle_circumference_m(double radius_m) {
    detail::require_positive(radius_m, "radius must be positive");
    return detail::finite_result(2.0 * pi * radius_m,
                                 "circle circumference result is not finite");
}

inline double sphere_surface_area_m2(double radius_m) {
    detail::require_positive(radius_m, "radius must be positive");
    return detail::finite_result(4.0 * pi * radius_m * radius_m,
                                 "sphere area result is not finite");
}

inline double sphere_volume_m3(double radius_m) {
    detail::require_positive(radius_m, "radius must be positive");
    return detail::finite_result((4.0 / 3.0) * pi * std::pow(radius_m, 3),
                                 "sphere volume result is not finite");
}

inline double cylinder_volume_m3(double radius_m, double height_m) {
    detail::require_positive(radius_m, "radius must be positive");
    detail::require_positive(height_m, "height must be positive");
    return detail::finite_result(pi * radius_m * radius_m * height_m,
                                 "cylinder volume result is not finite");
}

inline double cylinder_total_surface_area_m2(double radius_m, double height_m) {
    detail::require_positive(radius_m, "radius must be positive");
    detail::require_positive(height_m, "height must be positive");
    return detail::finite_result(
        2.0 * pi * radius_m * (radius_m + height_m),
        "cylinder surface-area result is not finite");
}

inline double cuboid_volume_m3(double length_m, double width_m, double height_m) {
    detail::require_positive(length_m, "length must be positive");
    detail::require_positive(width_m, "width must be positive");
    detail::require_positive(height_m, "height must be positive");
    return detail::finite_result(length_m * width_m * height_m,
                                 "cuboid volume result is not finite");
}

inline double cuboid_surface_area_m2(double length_m, double width_m,
                                     double height_m) {
    detail::require_positive(length_m, "length must be positive");
    detail::require_positive(width_m, "width must be positive");
    detail::require_positive(height_m, "height must be positive");
    return detail::finite_result(
        2.0 * (length_m * width_m + length_m * height_m + width_m * height_m),
        "cuboid surface-area result is not finite");
}

// Heron's formula. Side lengths must form a non-degenerate triangle.
inline double triangle_area_m2(double side_a_m, double side_b_m,
                               double side_c_m) {
    detail::require_positive(side_a_m, "side a must be positive");
    detail::require_positive(side_b_m, "side b must be positive");
    detail::require_positive(side_c_m, "side c must be positive");
    if (side_a_m + side_b_m <= side_c_m ||
        side_a_m + side_c_m <= side_b_m ||
        side_b_m + side_c_m <= side_a_m) {
        throw std::invalid_argument("side lengths must form a triangle");
    }

    const double semiperimeter = (side_a_m + side_b_m + side_c_m) / 2.0;
    const double radicand = semiperimeter * (semiperimeter - side_a_m) *
                            (semiperimeter - side_b_m) *
                            (semiperimeter - side_c_m);
    return detail::finite_result(std::sqrt(radicand),
                                 "triangle area result is not finite");
}

}  // namespace geometry
}  // namespace gpai
