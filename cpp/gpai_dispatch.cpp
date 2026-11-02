#include "gpai_core.h"

#include <cmath>
#include <cstddef>
#include <cstring>
#include <limits>

extern "C" {

GPAI_API int gpai_calculate(
    const char* operation,
    const double* inputs,
    std::size_t input_count,
    double* outputs,
    std::size_t output_count) {
    if (operation == nullptr || inputs == nullptr || outputs == nullptr) {
        return GPAI_STATUS_INVALID_ARGUMENT;
    }
    for (std::size_t i = 0; i < input_count; ++i) {
        if (!std::isfinite(inputs[i])) return GPAI_STATUS_INVALID_ARGUMENT;
    }

#define GPAI_MATCH(ID, IN_COUNT, OUT_COUNT) \
    (std::strcmp(operation, ID) == 0 && input_count == IN_COUNT && output_count == OUT_COUNT)
#define GPAI_ONE(ID, FN, N, ...) \
    if (GPAI_MATCH(ID, N, 1)) return FN(__VA_ARGS__, &outputs[0])

    if (std::strcmp(operation, "gpai_calc_integral_simpson_quadratic") == 0) {
        if (input_count != 6 || output_count != 1 || inputs[2] <= 0.0 ||
            std::trunc(inputs[2]) != inputs[2] ||
            inputs[2] > static_cast<double>(std::numeric_limits<int>::max())) {
            return GPAI_STATUS_INVALID_ARGUMENT;
        }
        return gpai_calc_integral_simpson_quadratic(
            inputs[0], inputs[1], static_cast<int>(inputs[2]),
            inputs[3], inputs[4], inputs[5], &outputs[0]);
    }

    GPAI_ONE("gpai_calc_derivative_quadratic", gpai_calc_derivative_quadratic, 5,
             inputs[0], inputs[1], inputs[2], inputs[3], inputs[4]);
    GPAI_ONE("gpai_calc_derivative_exact_quadratic", gpai_calc_derivative_exact_quadratic, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_calc_integral_exact_quadratic", gpai_calc_integral_exact_quadratic, 5,
             inputs[0], inputs[1], inputs[2], inputs[3], inputs[4]);

    if (GPAI_MATCH("gpai_mech_axial_response", 4, 3)) {
        return gpai_mech_axial_response(inputs[0], inputs[1], inputs[2], inputs[3],
                                        &outputs[0], &outputs[1], &outputs[2]);
    }
    GPAI_ONE("gpai_mech_cantilever_tip_deflection", gpai_mech_cantilever_tip_deflection, 4,
             inputs[0], inputs[1], inputs[2], inputs[3]);
    GPAI_ONE("gpai_mech_transverse_shear_stress", gpai_mech_transverse_shear_stress, 4,
             inputs[0], inputs[1], inputs[2], inputs[3]);
    GPAI_ONE("gpai_mech_bending_stress", gpai_mech_bending_stress, 3,
             inputs[0], inputs[1], inputs[2]);
    if (GPAI_MATCH("gpai_mech_solid_shaft_torsion", 4, 2)) {
        return gpai_mech_solid_shaft_torsion(inputs[0], inputs[1], inputs[2], inputs[3],
                                             &outputs[0], &outputs[1]);
    }
    GPAI_ONE("gpai_mech_circular_second_moment", gpai_mech_circular_second_moment, 1,
             inputs[0]);
    GPAI_ONE("gpai_mech_circular_polar_moment", gpai_mech_circular_polar_moment, 1,
             inputs[0]);
    GPAI_ONE("gpai_mech_safety_factor", gpai_mech_safety_factor, 2,
             inputs[0], inputs[1]);

    GPAI_ONE("gpai_thermo_ideal_gas_pressure", gpai_thermo_ideal_gas_pressure, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_thermo_ideal_gas_volume", gpai_thermo_ideal_gas_volume, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_thermo_ideal_gas_temperature", gpai_thermo_ideal_gas_temperature, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_thermo_heat", gpai_thermo_heat, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_thermo_first_law_delta_u", gpai_thermo_first_law_delta_u, 2,
             inputs[0], inputs[1]);
    GPAI_ONE("gpai_thermo_isobaric_work", gpai_thermo_isobaric_work, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_thermo_carnot_efficiency_percent", gpai_thermo_carnot_efficiency_percent, 2,
             inputs[0], inputs[1]);
    GPAI_ONE("gpai_thermo_enthalpy", gpai_thermo_enthalpy, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_thermo_ideal_gas_delta_u", gpai_thermo_ideal_gas_delta_u, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_thermo_ideal_gas_delta_h", gpai_thermo_ideal_gas_delta_h, 3,
             inputs[0], inputs[1], inputs[2]);

    GPAI_ONE("gpai_geometry_circle_area", gpai_geometry_circle_area, 1, inputs[0]);
    GPAI_ONE("gpai_geometry_circle_circumference", gpai_geometry_circle_circumference, 1,
             inputs[0]);
    GPAI_ONE("gpai_geometry_cylinder_volume", gpai_geometry_cylinder_volume, 2,
             inputs[0], inputs[1]);
    GPAI_ONE("gpai_geometry_sphere_volume", gpai_geometry_sphere_volume, 1, inputs[0]);
    GPAI_ONE("gpai_geometry_sphere_surface_area", gpai_geometry_sphere_surface_area, 1,
             inputs[0]);
    GPAI_ONE("gpai_geometry_cuboid_volume", gpai_geometry_cuboid_volume, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_geometry_triangle_area", gpai_geometry_triangle_area, 2,
             inputs[0], inputs[1]);
    GPAI_ONE("gpai_geometry_hypotenuse", gpai_geometry_hypotenuse, 2,
             inputs[0], inputs[1]);
    GPAI_ONE("gpai_geometry_distance_2d", gpai_geometry_distance_2d, 4,
             inputs[0], inputs[1], inputs[2], inputs[3]);
    GPAI_ONE("gpai_geometry_distance_3d", gpai_geometry_distance_3d, 6,
             inputs[0], inputs[1], inputs[2], inputs[3], inputs[4], inputs[5]);
    GPAI_ONE("gpai_geometry_dot_product_3d", gpai_geometry_dot_product_3d, 6,
             inputs[0], inputs[1], inputs[2], inputs[3], inputs[4], inputs[5]);
    GPAI_ONE("gpai_geometry_vector_magnitude_3d", gpai_geometry_vector_magnitude_3d, 3,
             inputs[0], inputs[1], inputs[2]);
    GPAI_ONE("gpai_geometry_vector_angle_3d_degrees", gpai_geometry_vector_angle_3d_degrees, 6,
             inputs[0], inputs[1], inputs[2], inputs[3], inputs[4], inputs[5]);

#undef GPAI_ONE
#undef GPAI_MATCH
    return GPAI_STATUS_INVALID_ARGUMENT;
}

}  // extern "C"
