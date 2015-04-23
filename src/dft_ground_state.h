// Copyright (c) 2013-2014 Anton Kozhevnikov, Thomas Schulthess
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that 
// the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
//    following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions 
//    and the following disclaimer in the documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/** \file dft_ground_state.h
 *
 *  \brief Contains definition and partial implementation of sirius::DFT_ground_state class.
 */

#ifndef __DFT_GROUND_STATE_H__
#define __DFT_GROUND_STATE_H__

#include "global.h"
#include "potential.h"
#include "density.h"
#include "k_set.h"
#include "force.h"

/** \page DFT Spin-polarized DFT
    \section section1 Preliminary notes

    \note Here and below sybol \f$ \sigma \f$ is reserved for the Pauli matrices. Spin components are labeled with \f$ \alpha \f$ or \f$ \beta\f$.

    Wave-function of spin-1/2 particle is a two-component spinor:
    \f[
        {\bf \varphi}({\bf r})=\left( \begin{array}{c} \varphi_1({\bf r}) \\ \varphi_2({\bf r}) \end{array} \right)
    \f]
    Operator of spin:
    \f[
        {\bf \hat S}=\frac{\hbar}{2}{\bf \sigma},
    \f]
*/
namespace sirius
{

class DFT_ground_state
{
    private:

        Global& parameters_;

        Potential* potential_;

        Density* density_;

        K_set* kset_;

        int use_symmetry_;

        double ewald_energy_;

        double ewald_energy()
        {
            Timer t("sirius::DFT_ground_state::ewald_energy");

            double alpha = 1.5;
            
            double ewald_g = 0;

            auto rl = parameters_.reciprocal_lattice();

            #pragma omp parallel
            {
                double ewald_g_pt = 0;

                #pragma omp for
                for (int igloc = 0; igloc < (int)rl->spl_num_gvec().local_size(); igloc++)
                {
                    int ig = rl->spl_num_gvec(igloc);

                    double_complex rho(0, 0);
                    for (int ia = 0; ia < parameters_.unit_cell()->num_atoms(); ia++)
                    {
                        rho += rl->gvec_phase_factor<local>(igloc, ia) * double(parameters_.unit_cell()->atom(ia)->zn());
                    }
                    double g2 = pow(rl->gvec_len(ig), 2);
                    if (ig)
                    {
                        ewald_g_pt += pow(abs(rho), 2) * exp(-g2 / 4 / alpha) / g2;
                    }
                    else
                    {
                        ewald_g_pt -= pow(parameters_.unit_cell()->num_electrons(), 2) / alpha / 4; // constant term in QE comments
                    }
                }

                #pragma omp critical
                ewald_g += ewald_g_pt;
            }
            parameters_.comm().allreduce(&ewald_g, 1);
            ewald_g *= (twopi / parameters_.unit_cell()->omega());

            /* remove self-interaction */
            for (int ia = 0; ia < parameters_.unit_cell()->num_atoms(); ia++)
            {
                ewald_g -= sqrt(alpha / pi) * std::pow(parameters_.unit_cell()->atom(ia)->zn(), 2);
            }

            double ewald_r = 0;
            #pragma omp parallel
            {
                double ewald_r_pt = 0;

                #pragma omp for
                for (int ia = 0; ia < parameters_.unit_cell()->num_atoms(); ia++)
                {
                    for (int i = 1; i < parameters_.unit_cell()->num_nearest_neighbours(ia); i++)
                    {
                        int ja = parameters_.unit_cell()->nearest_neighbour(i, ia).atom_id;
                        double d = parameters_.unit_cell()->nearest_neighbour(i, ia).distance;
                        ewald_r_pt += 0.5 * parameters_.unit_cell()->atom(ia)->zn() * 
                                            parameters_.unit_cell()->atom(ja)->zn() * gsl_sf_erfc(sqrt(alpha) * d) / d;
                    }
                }

                #pragma omp critical
                ewald_r += ewald_r_pt;
            }

            return (ewald_g + ewald_r);
        }

    public:

        DFT_ground_state(Global& parameters__, Potential* potential__, Density* density__, K_set* kset__, int use_symmetry__)
            : parameters_(parameters__), 
              potential_(potential__), 
              density_(density__), 
              kset_(kset__),
              use_symmetry_(use_symmetry__)
        {
            if (parameters_.esm_type() == ultrasoft_pseudopotential) ewald_energy_ = ewald_energy();
        }

        void move_atoms(int istep);

        void forces(mdarray<double, 2>& atom_force);

        void scf_loop(double potential_tol, double energy_tol, int num_dft_iter);

        void relax_atom_positions();

        void update();

        void print_info();

        /// Return nucleus energy in the electrostatic field.
        /** Compute energy of nucleus in the electrostatic potential generated by the total (electrons + nuclei) 
            charge density. Diverging self-interaction term z*z/|r=0| is excluded. */
        double energy_enuc();
        
        /// Return eigen-value sum of core states.
        double core_eval_sum();
        
        double energy_vha()
        {
            //return inner(parameters_, density_->rho(), potential_->hartree_potential());
            return potential_->energy_vha();
        }
        
        double energy_vxc()
        {
            return inner(parameters_, density_->rho(), potential_->xc_potential());
        }
        
        double energy_exc()
        {
            double exc = inner(parameters_, density_->rho(), potential_->xc_energy_density());
            if (parameters_.esm_type() == ultrasoft_pseudopotential) 
                exc += inner(parameters_, density_->rho_pseudo_core(), potential_->xc_energy_density());
            return exc;
        }

        double energy_bxc()
        {
            double ebxc = 0.0;
            for (int j = 0; j < parameters_.num_mag_dims(); j++) 
                ebxc += inner(parameters_, density_->magnetization(j), potential_->effective_magnetic_field(j));
            return ebxc;
        }

        double energy_veff()
        {
            //return inner(parameters_, density_->rho(), potential_->effective_potential());
            return energy_vha() + energy_vxc();
        }

        /// Full eigen-value sum (core + valence)
        double eval_sum()
        {
            return (core_eval_sum() + kset_->valence_eval_sum());
        }
        
        /// Kinetic energy
        /** more doc here
        */
        double energy_kin()
        {
            return (eval_sum() - energy_veff() - energy_bxc());
        }

        /// Total energy of the electronic subsystem.
        /** From the definition of the density functional we have:
         *  
         *  \f[
         *      E[\rho] = T[\rho] + E^{H}[\rho] + E^{XC}[\rho] + E^{ext}[\rho]
         *  \f]
         *  where \f$ T[\rho] \f$ is the kinetic energy, \f$ E^{H}[\rho] \f$ - electrostatic energy of
         *  electron-electron density interaction, \f$ E^{XC}[\rho] \f$ - exchange-correlation energy
         *  and \f$ E^{ext}[\rho] \f$ - energy in the external field of nuclei.
         *  
         *  Electrostatic and external field energies are grouped in the following way:
         *  \f[
         *      \frac{1}{2} \int \int \frac{\rho({\bf r})\rho({\bf r'}) d{\bf r} d{\bf r'}}{|{\bf r} - {\bf r'}|} + 
         *          \int \rho({\bf r}) V^{nuc}({\bf r}) d{\bf r} = \frac{1}{2} \int V^{H}({\bf r})\rho({\bf r})d{\bf r} + 
         *          \frac{1}{2} \int \rho({\bf r}) V^{nuc}({\bf r}) d{\bf r}
         *  \f]
         *  Here \f$ V^{H}({\bf r}) \f$ is the total (electron + nuclei) electrostatic potential returned by the 
         *  poisson solver. Next we transform the remaining term:
         *  \f[
         *      \frac{1}{2} \int \rho({\bf r}) V^{nuc}({\bf r}) d{\bf r} = 
         *      \frac{1}{2} \int \int \frac{\rho({\bf r})\rho^{nuc}({\bf r'}) d{\bf r} d{\bf r'}}{|{\bf r} - {\bf r'}|} = 
         *      \frac{1}{2} \int V^{H,el}({\bf r}) \rho^{nuc}({\bf r}) d{\bf r}
         *  \f]
         */
        double total_energy()
        {
            switch (parameters_.esm_type())
            {
                case full_potential_lapwlo:
                case full_potential_pwlo:
                {
                    return (energy_kin() + energy_exc() + 0.5 * energy_vha() + energy_enuc());
                }
                case ultrasoft_pseudopotential:
                case norm_conserving_pseudopotential:
                {
                    return (kset_->valence_eval_sum() - (energy_vxc() + energy_vha()) + 0.5 * energy_vha() + 
                            energy_exc() + ewald_energy_);
                }
                default:
                {
                    STOP();
                }
            }
            return 0; // make compiler happy
        }

        void generate_effective_potential()
        {
            switch(parameters_.esm_type())
            {
                case full_potential_lapwlo:
                case full_potential_pwlo:
                {
                    potential_->generate_effective_potential(density_->rho(), density_->magnetization());
                    break;
                }
                case ultrasoft_pseudopotential:
                case norm_conserving_pseudopotential:
                {
                    potential_->generate_effective_potential(density_->rho(), density_->rho_pseudo_core(), density_->magnetization());
                    break;
                }
            }

            if (parameters_.esm_type() == ultrasoft_pseudopotential ||
                parameters_.esm_type() == norm_conserving_pseudopotential)
            {
                potential_->generate_d_mtrx();
            }

        }

        void symmetrize_density()
        {
            auto fft = parameters_.fft();
            
            auto& spl_num_gvec = parameters_.reciprocal_lattice()->spl_num_gvec();
            auto& comm = parameters_.comm();

            parameters_.unit_cell()->symmetry()->symmetrize_function(&density_->rho()->f_pw(0), fft, spl_num_gvec, comm);
            parameters_.unit_cell()->symmetry()->symmetrize_function(density_->rho()->f_mt(), comm);

            if (parameters_.num_mag_dims() == 1)
            {
                parameters_.unit_cell()->symmetry()->symmetrize_vector_z_component(&density_->magnetization(0)->f_pw(0), fft, comm);
                parameters_.unit_cell()->symmetry()->symmetrize_vector_z_component(density_->magnetization(0)->f_mt(), comm);
            }

        }
};

};

#endif // __DFT_GROUND_STATE_H__

