#ifndef GPAI_CORE_H
#define GPAI_CORE_H

#if defined(_WIN32)
#  if defined(GPAI_CORE_BUILD)
#    define GPAI_API __declspec(dllexport)
#  else
#    define GPAI_API __declspec(dllimport)
#  endif
#else
#  define GPAI_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum gpai_status {
    GPAI_STATUS_OK = 0,
    GPAI_STATUS_INVALID_ARGUMENT = 1,
    GPAI_STATUS_OVERFLOW = 2,
    GPAI_STATUS_INTERNAL_ERROR = 3
};

/* Calculus: the function is f(x) = a*x*x + b*x + c. */
GPAI_API int gpai_calc_derivative_quadratic(
    double x, double a, double b, double c, double step, double* out_value);
GPAI_API int gpai_calc_derivative_exact_quadratic(
    double x, double a, double b, double* out_value);
GPAI_API int gpai_calc_integral_simpson_quadratic(
    double start, double end, int intervals,
    double a, double b, double c, double* out_value);
GPAI_API int gpai_calc_integral_exact_quadratic(
    double start, double end, double a, double b, double c, double* out_value);

/* Mechanics of materials: SI units. */
GPAI_API int gpai_mech_axial_response(
    double force_n, double area_m2, double length_m, double young_modulus_pa,
    double* out_stress_pa, double* out_strain, double* out_elongation_m);
GPAI_API int gpai_mech_cantilever_tip_deflection(
    double load_n, double length_m, double young_modulus_pa,
    double second_moment_m4, double* out_deflection_m);
GPAI_API int gpai_mech_transverse_shear_stress(
    double shear_force_n, double first_moment_m3, double second_moment_m4,
    double thickness_m, double* out_stress_pa);
GPAI_API int gpai_mech_bending_stress(
    double moment_nm, double distance_m, double second_moment_m4,
    double* out_stress_pa);
GPAI_API int gpai_mech_solid_shaft_torsion(
    double torque_nm, double diameter_m, double length_m,
    double shear_modulus_pa, double* out_shear_stress_pa,
    double* out_twist_angle_rad);
GPAI_API int gpai_mech_circular_second_moment(
    double diameter_m, double* out_second_moment_m4);
GPAI_API int gpai_mech_circular_polar_moment(
    double diameter_m, double* out_polar_moment_m4);
GPAI_API int gpai_mech_safety_factor(
    double allowable_stress_pa, double actual_stress_pa, double* out_factor);

/* Thermodynamics: temperatures are absolute K; energies are J. */
GPAI_API int gpai_thermo_ideal_gas_pressure(
    double amount_mol, double temperature_k, double volume_m3, double* out_pressure_pa);
GPAI_API int gpai_thermo_ideal_gas_volume(
    double amount_mol, double temperature_k, double pressure_pa, double* out_volume_m3);
GPAI_API int gpai_thermo_ideal_gas_temperature(
    double pressure_pa, double volume_m3, double amount_mol, double* out_temperature_k);
GPAI_API int gpai_thermo_heat(
    double mass_kg, double specific_heat_j_per_kg_k,
    double delta_temperature_k, double* out_heat_j);
GPAI_API int gpai_thermo_first_law_delta_u(
    double heat_into_system_j, double work_by_system_j, double* out_delta_u_j);
GPAI_API int gpai_thermo_isobaric_work(
    double pressure_pa, double initial_volume_m3, double final_volume_m3,
    double* out_work_j);
GPAI_API int gpai_thermo_carnot_efficiency_percent(
    double cold_reservoir_k, double hot_reservoir_k, double* out_efficiency_percent);
GPAI_API int gpai_thermo_enthalpy(
    double internal_energy_j, double pressure_pa, double volume_m3,
    double* out_enthalpy_j);
GPAI_API int gpai_thermo_ideal_gas_delta_u(
    double amount_mol, double molar_cv_j_per_mol_k,
    double delta_temperature_k, double* out_delta_u_j);
GPAI_API int gpai_thermo_ideal_gas_delta_h(
    double amount_mol, double molar_cp_j_per_mol_k,
    double delta_temperature_k, double* out_delta_h_j);

/* Geometry: lengths are m; areas are m^2; volumes are m^3. */
GPAI_API int gpai_geometry_circle_area(double radius_m, double* out_area_m2);
GPAI_API int gpai_geometry_circle_circumference(double radius_m, double* out_circumference_m);
GPAI_API int gpai_geometry_cylinder_volume(
    double radius_m, double height_m, double* out_volume_m3);
GPAI_API int gpai_geometry_sphere_volume(double radius_m, double* out_volume_m3);
GPAI_API int gpai_geometry_sphere_surface_area(double radius_m, double* out_area_m2);
GPAI_API int gpai_geometry_cuboid_volume(
    double length_m, double width_m, double height_m, double* out_volume_m3);
GPAI_API int gpai_geometry_triangle_area(
    double base_m, double height_m, double* out_area_m2);
GPAI_API int gpai_geometry_hypotenuse(double side_a_m, double side_b_m, double* out_length_m);
GPAI_API int gpai_geometry_distance_2d(
    double x1_m, double y1_m, double x2_m, double y2_m, double* out_distance_m);
GPAI_API int gpai_geometry_distance_3d(
    double x1_m, double y1_m, double z1_m,
    double x2_m, double y2_m, double z2_m, double* out_distance_m);
GPAI_API int gpai_geometry_dot_product_3d(
    double ux, double uy, double uz, double vx, double vy, double vz,
    double* out_dot_product);
GPAI_API int gpai_geometry_vector_magnitude_3d(
    double x, double y, double z, double* out_magnitude);
GPAI_API int gpai_geometry_vector_angle_3d_degrees(
    double ux, double uy, double uz, double vx, double vy, double vz,
    double* out_angle_degrees);

#ifdef __cplusplus
}
#endif

#endif
