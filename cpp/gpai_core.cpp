#include "gpai_core.h"

#include "gpai_calculations.hpp"

#include <cmath>
#include <stdexcept>

namespace {

template <typename Function>
int gpai_call(Function&& function) noexcept {
    try {
        function();
        return GPAI_STATUS_OK;
    } catch (const std::invalid_argument&) {
        return GPAI_STATUS_INVALID_ARGUMENT;
    } catch (const std::overflow_error&) {
        return GPAI_STATUS_OVERFLOW;
    } catch (...) {
        return GPAI_STATUS_INTERNAL_ERROR;
    }
}

template <typename Function>
int gpai_write_result(double* out_value, Function&& function) noexcept {
    if (out_value == nullptr) {
        return GPAI_STATUS_INVALID_ARGUMENT;
    }
    return gpai_call([&]() {
        const double value = function();
        if (!std::isfinite(value)) {
            throw std::overflow_error("result is not finite");
        }
        *out_value = value;
    });
}

void require_finite_output(double value) {
    if (!std::isfinite(value)) {
        throw std::overflow_error("result is not finite");
    }
}

}  // namespace

extern "C" {

int gpai_calc_derivative_quadratic(
    double x, double a, double b, double c, double step, double* out_value) {
    const auto f = [=](double input) {
        return a * input * input + b * input + c;
    };
    return gpai_write_result(out_value, [&]() {
        return gpai::calculus::derivative_central(f, x, step);
    });
}

int gpai_calc_derivative_exact_quadratic(
    double x, double a, double b, double* out_value) {
    return gpai_write_result(out_value, [&]() {
        return gpai::calculus::derivative_quadratic_exact(x, a, b);
    });
}

int gpai_calc_integral_simpson_quadratic(
    double start, double end, int intervals,
    double a, double b, double c, double* out_value) {
    if (intervals <= 0) {
        return GPAI_STATUS_INVALID_ARGUMENT;
    }
    const auto f = [=](double input) {
        return a * input * input + b * input + c;
    };
    return gpai_write_result(out_value, [&]() {
        return gpai::calculus::integrate_simpson(
            f, start, end, static_cast<std::size_t>(intervals));
    });
}

int gpai_calc_integral_exact_quadratic(
    double start, double end, double a, double b, double c, double* out_value) {
    return gpai_write_result(out_value, [&]() {
        return gpai::calculus::integral_quadratic_exact(start, end, a, b, c);
    });
}

int gpai_mech_axial_response(
    double force_n, double area_m2, double length_m, double young_modulus_pa,
    double* out_stress_pa, double* out_strain, double* out_elongation_m) {
    if (out_stress_pa == nullptr || out_strain == nullptr ||
        out_elongation_m == nullptr) {
        return GPAI_STATUS_INVALID_ARGUMENT;
    }
    return gpai_call([&]() {
        const auto result = gpai::mechanics_of_materials::axial_response(
            force_n, area_m2, length_m, young_modulus_pa);
        require_finite_output(result.stress_pa);
        require_finite_output(result.strain);
        require_finite_output(result.elongation_m);
        *out_stress_pa = result.stress_pa;
        *out_strain = result.strain;
        *out_elongation_m = result.elongation_m;
    });
}

int gpai_mech_cantilever_tip_deflection(
    double load_n, double length_m, double young_modulus_pa,
    double second_moment_m4, double* out_deflection_m) {
    return gpai_write_result(out_deflection_m, [&]() {
        return gpai::mechanics_of_materials::cantilever_tip_deflection_m(
            load_n, length_m, young_modulus_pa, second_moment_m4);
    });
}

int gpai_mech_transverse_shear_stress(
    double shear_force_n, double first_moment_m3, double second_moment_m4,
    double thickness_m, double* out_stress_pa) {
    return gpai_write_result(out_stress_pa, [&]() {
        return gpai::mechanics_of_materials::transverse_shear_stress_pa(
            shear_force_n, first_moment_m3, second_moment_m4, thickness_m);
    });
}

int gpai_mech_bending_stress(
    double moment_nm, double distance_m, double second_moment_m4,
    double* out_stress_pa) {
    return gpai_write_result(out_stress_pa, [&]() {
        return gpai::mechanics_of_materials::bending_stress(
            moment_nm, distance_m, second_moment_m4);
    });
}

int gpai_mech_solid_shaft_torsion(
    double torque_nm, double diameter_m, double length_m,
    double shear_modulus_pa, double* out_shear_stress_pa,
    double* out_twist_angle_rad) {
    if (out_shear_stress_pa == nullptr || out_twist_angle_rad == nullptr) {
        return GPAI_STATUS_INVALID_ARGUMENT;
    }
    return gpai_call([&]() {
        const auto result = gpai::mechanics_of_materials::solid_shaft_torsion(
            torque_nm, diameter_m, length_m, shear_modulus_pa);
        require_finite_output(result.maximum_shear_stress_pa);
        require_finite_output(result.twist_angle_rad);
        *out_shear_stress_pa = result.maximum_shear_stress_pa;
        *out_twist_angle_rad = result.twist_angle_rad;
    });
}

int gpai_mech_circular_second_moment(
    double diameter_m, double* out_second_moment_m4) {
    return gpai_write_result(out_second_moment_m4, [&]() {
        return gpai::mechanics_of_materials::circular_second_moment_m4(diameter_m);
    });
}

int gpai_mech_circular_polar_moment(
    double diameter_m, double* out_polar_moment_m4) {
    return gpai_write_result(out_polar_moment_m4, [&]() {
        return gpai::mechanics_of_materials::circular_polar_moment_m4(diameter_m);
    });
}

int gpai_mech_safety_factor(
    double allowable_stress_pa, double actual_stress_pa, double* out_factor) {
    return gpai_write_result(out_factor, [&]() {
        return gpai::mechanics_of_materials::safety_factor(
            allowable_stress_pa, actual_stress_pa);
    });
}

int gpai_thermo_ideal_gas_pressure(
    double amount_mol, double temperature_k, double volume_m3,
    double* out_pressure_pa) {
    return gpai_write_result(out_pressure_pa, [&]() {
        return gpai::thermodynamics::ideal_gas_pressure_pa(
            amount_mol, temperature_k, volume_m3);
    });
}

int gpai_thermo_ideal_gas_volume(
    double amount_mol, double temperature_k, double pressure_pa,
    double* out_volume_m3) {
    return gpai_write_result(out_volume_m3, [&]() {
        return gpai::thermodynamics::ideal_gas_volume_m3(
            amount_mol, temperature_k, pressure_pa);
    });
}

int gpai_thermo_ideal_gas_temperature(
    double pressure_pa, double volume_m3, double amount_mol,
    double* out_temperature_k) {
    return gpai_write_result(out_temperature_k, [&]() {
        return gpai::thermodynamics::ideal_gas_temperature_k(
            pressure_pa, volume_m3, amount_mol);
    });
}

int gpai_thermo_heat(
    double mass_kg, double specific_heat_j_per_kg_k,
    double delta_temperature_k, double* out_heat_j) {
    return gpai_write_result(out_heat_j, [&]() {
        return gpai::thermodynamics::heat_from_specific_heat_j(
            mass_kg, specific_heat_j_per_kg_k, delta_temperature_k);
    });
}

int gpai_thermo_first_law_delta_u(
    double heat_into_system_j, double work_by_system_j, double* out_delta_u_j) {
    return gpai_write_result(out_delta_u_j, [&]() {
        return gpai::thermodynamics::first_law_delta_u_j(
            heat_into_system_j, work_by_system_j);
    });
}

int gpai_thermo_isobaric_work(
    double pressure_pa, double initial_volume_m3, double final_volume_m3,
    double* out_work_j) {
    return gpai_write_result(out_work_j, [&]() {
        return gpai::thermodynamics::isobaric_work_j(
            pressure_pa, final_volume_m3 - initial_volume_m3);
    });
}

int gpai_thermo_carnot_efficiency_percent(
    double cold_reservoir_k, double hot_reservoir_k,
    double* out_efficiency_percent) {
    return gpai_write_result(out_efficiency_percent, [&]() {
        return 100.0 * gpai::thermodynamics::carnot_efficiency(
            cold_reservoir_k, hot_reservoir_k);
    });
}

int gpai_thermo_enthalpy(
    double internal_energy_j, double pressure_pa, double volume_m3,
    double* out_enthalpy_j) {
    return gpai_write_result(out_enthalpy_j, [&]() {
        return gpai::thermodynamics::enthalpy_j(
            internal_energy_j, pressure_pa, volume_m3);
    });
}

int gpai_thermo_ideal_gas_delta_u(
    double amount_mol, double molar_cv_j_per_mol_k,
    double delta_temperature_k, double* out_delta_u_j) {
    return gpai_write_result(out_delta_u_j, [&]() {
        return gpai::thermodynamics::ideal_gas_delta_u_j(
            amount_mol, molar_cv_j_per_mol_k, delta_temperature_k);
    });
}

int gpai_thermo_ideal_gas_delta_h(
    double amount_mol, double molar_cp_j_per_mol_k,
    double delta_temperature_k, double* out_delta_h_j) {
    return gpai_write_result(out_delta_h_j, [&]() {
        return gpai::thermodynamics::ideal_gas_delta_h_j(
            amount_mol, molar_cp_j_per_mol_k, delta_temperature_k);
    });
}

int gpai_geometry_circle_area(double radius_m, double* out_area_m2) {
    return gpai_write_result(out_area_m2, [&]() {
        return gpai::geometry::circle_area_m2(radius_m);
    });
}

int gpai_geometry_circle_circumference(
    double radius_m, double* out_circumference_m) {
    return gpai_write_result(out_circumference_m, [&]() {
        return gpai::geometry::circle_circumference_m(radius_m);
    });
}

int gpai_geometry_cylinder_volume(
    double radius_m, double height_m, double* out_volume_m3) {
    return gpai_write_result(out_volume_m3, [&]() {
        return gpai::geometry::cylinder_volume_m3(radius_m, height_m);
    });
}

int gpai_geometry_sphere_volume(double radius_m, double* out_volume_m3) {
    return gpai_write_result(out_volume_m3, [&]() {
        return gpai::geometry::sphere_volume_m3(radius_m);
    });
}

int gpai_geometry_sphere_surface_area(double radius_m, double* out_area_m2) {
    return gpai_write_result(out_area_m2, [&]() {
        return gpai::geometry::sphere_surface_area_m2(radius_m);
    });
}

int gpai_geometry_cuboid_volume(
    double length_m, double width_m, double height_m, double* out_volume_m3) {
    return gpai_write_result(out_volume_m3, [&]() {
        return gpai::geometry::cuboid_volume_m3(length_m, width_m, height_m);
    });
}

int gpai_geometry_triangle_area(
    double base_m, double height_m, double* out_area_m2) {
    return gpai_write_result(out_area_m2, [&]() {
        return gpai::geometry::triangle_area_from_base_height_m2(base_m, height_m);
    });
}

int gpai_geometry_hypotenuse(
    double side_a_m, double side_b_m, double* out_length_m) {
    return gpai_write_result(out_length_m, [&]() {
        return gpai::geometry::hypotenuse(side_a_m, side_b_m);
    });
}

int gpai_geometry_distance_2d(
    double x1_m, double y1_m, double x2_m, double y2_m,
    double* out_distance_m) {
    return gpai_write_result(out_distance_m, [&]() {
        return gpai::geometry::distance_2d_m(x1_m, y1_m, x2_m, y2_m);
    });
}

int gpai_geometry_distance_3d(
    double x1_m, double y1_m, double z1_m,
    double x2_m, double y2_m, double z2_m, double* out_distance_m) {
    return gpai_write_result(out_distance_m, [&]() {
        return gpai::geometry::distance_3d_m(
            x1_m, y1_m, z1_m, x2_m, y2_m, z2_m);
    });
}

int gpai_geometry_dot_product_3d(
    double ux, double uy, double uz, double vx, double vy, double vz,
    double* out_dot_product) {
    return gpai_write_result(out_dot_product, [&]() {
        return gpai::geometry::dot_product_3d(ux, uy, uz, vx, vy, vz);
    });
}

int gpai_geometry_vector_magnitude_3d(
    double x, double y, double z, double* out_magnitude) {
    return gpai_write_result(out_magnitude, [&]() {
        return gpai::geometry::vector_magnitude_3d(x, y, z);
    });
}

int gpai_geometry_vector_angle_3d_degrees(
    double ux, double uy, double uz, double vx, double vy, double vz,
    double* out_angle_degrees) {
    return gpai_write_result(out_angle_degrees, [&]() {
        return gpai::geometry::vector_angle_3d_degrees(
            ux, uy, uz, vx, vy, vz);
    });
}

}  // extern "C"
