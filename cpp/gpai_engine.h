#ifndef GPAI_ENGINE_H
#define GPAI_ENGINE_H

#include "gpai_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Compatibility C ABI for the earlier unified engine. Invalid inputs return NaN. */
GPAI_API double pyNumericalDerivative(double x);
GPAI_API double pyNumericalIntegral(double start, double end, int intervals);
GPAI_API double target_function(double x, double a, double b, double c);
GPAI_API double calc_derivative(double x, double a, double b, double c);
GPAI_API double calc_derivative_exact(double x, double a, double b, double c);
GPAI_API double calc_integral_simpson(
    double start, double end, int intervals, double a, double b, double c);
GPAI_API double calc_integral_exact(
    double start, double end, double a, double b, double c);

GPAI_API double mech_normal_stress(double force_N, double area_m2);
GPAI_API double mech_axial_strain(double stress_Pa, double youngs_modulus_Pa);
GPAI_API double mech_strain_from_length(double delta_length_m, double initial_length_m);
GPAI_API double mech_axial_deformation(
    double force_N, double length_m, double area_m2, double youngs_modulus_Pa);
GPAI_API double mech_hooke_stress(double youngs_modulus_Pa, double strain);
GPAI_API double mech_cantilever_deflection(
    double load_N, double length_m, double modulus_E_Pa, double inertia_I_m4);
GPAI_API double mech_bending_stress(
    double moment_Nm, double distance_m, double inertia_I_m4);
GPAI_API double mech_shear_stress(
    double shear_force_N, double first_moment_Q_m3,
    double inertia_I_m4, double thickness_m);
GPAI_API double mech_torsional_stress(
    double torque_Nm, double radius_m, double polar_moment_m4);
GPAI_API double mech_circular_second_moment(double diameter_m);
GPAI_API double mech_circular_polar_moment(double diameter_m);
GPAI_API double mech_safety_factor(double allowable_stress_Pa, double actual_stress_Pa);

GPAI_API double thermo_ideal_gas_pressure(double moles_n, double temp_K, double volume_m3);
GPAI_API double thermo_ideal_gas_volume(double moles_n, double temp_K, double pressure_Pa);
GPAI_API double thermo_ideal_gas_temperature(
    double pressure_Pa, double volume_m3, double moles_n);
GPAI_API double thermo_heat(
    double mass_kg, double specific_heat_JkgK, double delta_temperature_K);
GPAI_API double thermo_internal_energy_change(double heat_Q_J, double work_W_J);
GPAI_API double thermo_constant_pressure_work(
    double pressure_Pa, double volume_initial_m3, double volume_final_m3);
GPAI_API double thermo_carnot_efficiency(double temp_low_K, double temp_high_K);
GPAI_API double thermo_enthalpy(
    double internal_energy_J, double pressure_Pa, double volume_m3);
GPAI_API double thermo_internal_energy_ideal_gas(
    double moles_n, double cv_JmolK, double delta_temperature_K);
GPAI_API double thermo_enthalpy_change_ideal_gas(
    double moles_n, double cp_JmolK, double delta_temperature_K);

GPAI_API double geom_circle_area(double radius);
GPAI_API double geom_circle_circumference(double radius);
GPAI_API double geom_cylinder_volume(double radius, double height);
GPAI_API double geom_sphere_volume(double radius);
GPAI_API double geom_sphere_surface_area(double radius);
GPAI_API double geom_rectangular_volume(double length, double width, double height);
GPAI_API double geom_distance_2d(double x1, double y1, double x2, double y2);
GPAI_API double geom_distance_3d(
    double x1, double y1, double z1, double x2, double y2, double z2);
GPAI_API double geom_dot_product_3d(
    double ux, double uy, double uz, double vx, double vy, double vz);
GPAI_API double geom_vector_magnitude_3d(double x, double y, double z);
GPAI_API double geom_vector_angle_3d(
    double ux, double uy, double uz, double vx, double vy, double vz);
GPAI_API double geom_triangle_area(double base, double height);
GPAI_API double geom_hypotenuse(double a, double b);

#ifdef __cplusplus
}
#endif

#endif
