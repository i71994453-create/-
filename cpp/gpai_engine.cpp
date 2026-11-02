#include "gpai_core.h"

#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>

namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr double kGasConstant = 8.314462618;

double error_value() {
    return std::numeric_limits<double>::quiet_NaN();
}

bool finite_values(std::initializer_list<double> values) {
    for (double value : values) {
        if (!std::isfinite(value)) return false;
    }
    return true;
}

template <typename Function>
double from_core(Function&& function) {
    double value = 0.0;
    const int status = function(&value);
    return status == GPAI_STATUS_OK && std::isfinite(value) ? value : error_value();
}

double checked_ratio(double numerator, double denominator) {
    if (!finite_values({numerator, denominator}) || denominator == 0.0) {
        return error_value();
    }
    const double value = numerator / denominator;
    return std::isfinite(value) ? value : error_value();
}

double square_function(double x) {
    return x * x;
}

double numerical_derivative(double (*function)(double), double x) {
    if (function == nullptr || !std::isfinite(x)) return error_value();
    const double step = 1e-6;
    const double left = function(x - step);
    const double right = function(x + step);
    if (!finite_values({left, right})) return error_value();
    const double value = (right - left) / (2.0 * step);
    return std::isfinite(value) ? value : error_value();
}

double numerical_integral(double (*function)(double), double start, double end, int intervals) {
    if (function == nullptr || !finite_values({start, end}) ||
        intervals <= 0 || intervals > 1000000) {
        return error_value();
    }
    const double step = (end - start) / static_cast<double>(intervals);
    const double first = function(start);
    const double last = function(end);
    if (!finite_values({step, first, last})) return error_value();

    double sum = 0.5 * (first + last);
    for (int i = 1; i < intervals; ++i) {
        const double x = start + static_cast<double>(i) * step;
        const double value = function(x);
        if (!std::isfinite(value)) return error_value();
        sum += value;
        if (!std::isfinite(sum)) return error_value();
    }
    const double result = sum * step;
    return std::isfinite(result) ? result : error_value();
}

}  // namespace

/*
 * This file adapts the earlier standalone gpai_engine.cpp API to the
 * validated gpai_core C ABI. The core remains the single source of the
 * project formulas; this file adds compatibility wrappers and insertion sort.
 */
extern "C" {

double pyNumericalDerivative(double x) {
    return numerical_derivative(square_function, x);
}

double pyNumericalIntegral(double start, double end, int intervals) {
    return numerical_integral(square_function, start, end, intervals);
}

double target_function(double x, double a, double b, double c) {
    if (!finite_values({x, a, b, c})) return error_value();
    const double value = a * x * x + b * x + c;
    return std::isfinite(value) ? value : error_value();
}

double calc_derivative(double x, double a, double b, double c) {
    return from_core([=](double* out) {
        (void)c;
        return gpai_calc_derivative_quadratic(x, a, b, c, 1e-5, out);
    });
}

double calc_derivative_exact(double x, double a, double b, double c) {
    (void)c;
    return from_core([=](double* out) {
        return gpai_calc_derivative_exact_quadratic(x, a, b, out);
    });
}

double calc_integral_simpson(
    double start, double end, int intervals,
    double a, double b, double c) {
    if (intervals <= 0 || intervals % 2 != 0) return error_value();
    return from_core([=](double* out) {
        return gpai_calc_integral_simpson_quadratic(
            start, end, intervals, a, b, c, out);
    });
}

double calc_integral_exact(double start, double end, double a, double b, double c) {
    return from_core([=](double* out) {
        return gpai_calc_integral_exact_quadratic(start, end, a, b, c, out);
    });
}

double mech_normal_stress(double force_N, double area_m2) {
    if (area_m2 <= 0.0) return error_value();
    return checked_ratio(force_N, area_m2);
}

double mech_axial_strain(double stress_Pa, double youngs_modulus_Pa) {
    if (youngs_modulus_Pa <= 0.0) return error_value();
    return checked_ratio(stress_Pa, youngs_modulus_Pa);
}

double mech_strain_from_length(double delta_length_m, double initial_length_m) {
    if (initial_length_m <= 0.0) return error_value();
    return checked_ratio(delta_length_m, initial_length_m);
}

double mech_axial_deformation(
    double force_N, double length_m, double area_m2, double youngs_modulus_Pa) {
    double stress = 0.0;
    double strain = 0.0;
    double elongation = 0.0;
    const int status = gpai_mech_axial_response(
        force_N, area_m2, length_m, youngs_modulus_Pa,
        &stress, &strain, &elongation);
    return status == GPAI_STATUS_OK && std::isfinite(elongation)
        ? elongation : error_value();
}

double mech_hooke_stress(double youngs_modulus_Pa, double strain) {
    if (!finite_values({youngs_modulus_Pa, strain}) || youngs_modulus_Pa <= 0.0) {
        return error_value();
    }
    const double value = youngs_modulus_Pa * strain;
    return std::isfinite(value) ? value : error_value();
}

double mech_cantilever_deflection(
    double load_N, double length_m, double modulus_E_Pa, double inertia_I_m4) {
    return from_core([=](double* out) {
        return gpai_mech_cantilever_tip_deflection(
            load_N, length_m, modulus_E_Pa, inertia_I_m4, out);
    });
}

double mech_bending_stress(double moment_Nm, double distance_m, double inertia_I_m4) {
    return from_core([=](double* out) {
        return gpai_mech_bending_stress(moment_Nm, distance_m, inertia_I_m4, out);
    });
}

double mech_shear_stress(
    double shear_force_N, double first_moment_Q_m3,
    double inertia_I_m4, double thickness_m) {
    return from_core([=](double* out) {
        return gpai_mech_transverse_shear_stress(
            shear_force_N, first_moment_Q_m3, inertia_I_m4, thickness_m, out);
    });
}

double mech_torsional_stress(
    double torque_Nm, double radius_m, double polar_moment_m4) {
    if (polar_moment_m4 <= 0.0) return error_value();
    return checked_ratio(torque_Nm * radius_m, polar_moment_m4);
}

double mech_circular_second_moment(double diameter_m) {
    return from_core([=](double* out) {
        return gpai_mech_circular_second_moment(diameter_m, out);
    });
}

double mech_circular_polar_moment(double diameter_m) {
    return from_core([=](double* out) {
        return gpai_mech_circular_polar_moment(diameter_m, out);
    });
}

double mech_safety_factor(double allowable_stress_Pa, double actual_stress_Pa) {
    if (!std::isfinite(actual_stress_Pa)) return error_value();
    return from_core([=](double* out) {
        return gpai_mech_safety_factor(
            allowable_stress_Pa, std::fabs(actual_stress_Pa), out);
    });
}

double thermo_ideal_gas_pressure(double moles_n, double temp_K, double volume_m3) {
    return from_core([=](double* out) {
        return gpai_thermo_ideal_gas_pressure(moles_n, temp_K, volume_m3, out);
    });
}

double thermo_ideal_gas_volume(double moles_n, double temp_K, double pressure_Pa) {
    return from_core([=](double* out) {
        return gpai_thermo_ideal_gas_volume(moles_n, temp_K, pressure_Pa, out);
    });
}

double thermo_ideal_gas_temperature(double pressure_Pa, double volume_m3, double moles_n) {
    return from_core([=](double* out) {
        return gpai_thermo_ideal_gas_temperature(
            pressure_Pa, volume_m3, moles_n, out);
    });
}

double thermo_heat(double mass_kg, double specific_heat_JkgK, double delta_temperature_K) {
    return from_core([=](double* out) {
        return gpai_thermo_heat(
            mass_kg, specific_heat_JkgK, delta_temperature_K, out);
    });
}

double thermo_internal_energy_change(double heat_Q_J, double work_W_J) {
    if (!finite_values({heat_Q_J, work_W_J})) return error_value();
    const double value = heat_Q_J - work_W_J;
    return std::isfinite(value) ? value : error_value();
}

double thermo_constant_pressure_work(
    double pressure_Pa, double volume_initial_m3, double volume_final_m3) {
    return from_core([=](double* out) {
        return gpai_thermo_isobaric_work(
            pressure_Pa, volume_initial_m3, volume_final_m3, out);
    });
}

double thermo_carnot_efficiency(double temp_low_K, double temp_high_K) {
    return from_core([=](double* out) {
        const int status = gpai_thermo_carnot_efficiency_percent(
            temp_low_K, temp_high_K, out);
        if (status != GPAI_STATUS_OK) return status;
        *out /= 100.0;
        return std::isfinite(*out) ? GPAI_STATUS_OK : GPAI_STATUS_OVERFLOW;
    }) * 100.0;
}

double thermo_enthalpy(double internal_energy_J, double pressure_Pa, double volume_m3) {
    return from_core([=](double* out) {
        return gpai_thermo_enthalpy(
            internal_energy_J, pressure_Pa, volume_m3, out);
    });
}

double thermo_internal_energy_ideal_gas(
    double moles_n, double cv_JmolK, double delta_temperature_K) {
    return from_core([=](double* out) {
        return gpai_thermo_ideal_gas_delta_u(
            moles_n, cv_JmolK, delta_temperature_K, out);
    });
}

double thermo_enthalpy_change_ideal_gas(
    double moles_n, double cp_JmolK, double delta_temperature_K) {
    return from_core([=](double* out) {
        return gpai_thermo_ideal_gas_delta_h(
            moles_n, cp_JmolK, delta_temperature_K, out);
    });
}

double geom_circle_area(double radius) {
    return from_core([=](double* out) {
        return gpai_geometry_circle_area(radius, out);
    });
}

double geom_circle_circumference(double radius) {
    return from_core([=](double* out) {
        return gpai_geometry_circle_circumference(radius, out);
    });
}

double geom_cylinder_volume(double radius, double height) {
    return from_core([=](double* out) {
        return gpai_geometry_cylinder_volume(radius, height, out);
    });
}

double geom_sphere_volume(double radius) {
    return from_core([=](double* out) {
        return gpai_geometry_sphere_volume(radius, out);
    });
}

double geom_sphere_surface_area(double radius) {
    return from_core([=](double* out) {
        return gpai_geometry_sphere_surface_area(radius, out);
    });
}

double geom_rectangular_volume(double length, double width, double height) {
    return from_core([=](double* out) {
        return gpai_geometry_cuboid_volume(length, width, height, out);
    });
}

double geom_distance_2d(double x1, double y1, double x2, double y2) {
    return from_core([=](double* out) {
        return gpai_geometry_distance_2d(x1, y1, x2, y2, out);
    });
}

double geom_distance_3d(
    double x1, double y1, double z1, double x2, double y2, double z2) {
    return from_core([=](double* out) {
        return gpai_geometry_distance_3d(x1, y1, z1, x2, y2, z2, out);
    });
}

double geom_dot_product_3d(
    double ux, double uy, double uz, double vx, double vy, double vz) {
    return from_core([=](double* out) {
        return gpai_geometry_dot_product_3d(ux, uy, uz, vx, vy, vz, out);
    });
}

double geom_vector_magnitude_3d(double x, double y, double z) {
    return from_core([=](double* out) {
        return gpai_geometry_vector_magnitude_3d(x, y, z, out);
    });
}

double geom_vector_angle_3d(
    double ux, double uy, double uz, double vx, double vy, double vz) {
    return from_core([=](double* out) {
        return gpai_geometry_vector_angle_3d_degrees(
            ux, uy, uz, vx, vy, vz, out);
    });
}

double geom_triangle_area(double base, double height) {
    return from_core([=](double* out) {
        return gpai_geometry_triangle_area(base, height, out);
    });
}

double geom_hypotenuse(double a, double b) {
    return from_core([=](double* out) {
        return gpai_geometry_hypotenuse(a, b, out);
    });
}

GPAI_API int gpai_engine_insertion_sort(
    double* values, std::size_t count, int descending) {
    if ((count != 0 && values == nullptr) ||
        (descending != 0 && descending != 1)) {
        return GPAI_STATUS_INVALID_ARGUMENT;
    }

    for (std::size_t i = 0; i < count; ++i) {
        if (!std::isfinite(values[i])) {
            return GPAI_STATUS_INVALID_ARGUMENT;
        }
    }

    for (std::size_t i = 1; i < count; ++i) {
        const double key = values[i];
        std::size_t position = i;
        while (position > 0 &&
               (descending ? values[position - 1] < key
                          : values[position - 1] > key)) {
            values[position] = values[position - 1];
            --position;
        }
        values[position] = key;
    }

    return GPAI_STATUS_OK;
}

}  // extern "C"

#if defined(GPAI_ENGINE_STANDALONE)
int main() {
    double values[] = {7.0, 2.0, 9.0, -1.0, 2.0, 4.0};
    constexpr std::size_t count = sizeof(values) / sizeof(values[0]);
    const int status = gpai_engine_insertion_sort(values, count, 0);
    if (status != GPAI_STATUS_OK) {
        std::cerr << "Insertion sort failed with status " << status << std::endl;
        return status;
    }

    std::cout << "Ascending: ";
    for (std::size_t i = 0; i < count; ++i) {
        if (i != 0) std::cout << ", ";
        std::cout << values[i];
    }
    std::cout << std::endl;
    return 0;
}
#endif
