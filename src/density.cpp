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

/** \file density.cpp
 *   
 *  \brief Contains remaining implementation of sirius::Density class.
 */

#include <thread>
#include <mutex>
#include "density.h"

namespace sirius {

//Density::Density(Global& parameters__) 
//    : parameters_(parameters__),
//      gaunt_coefs_(nullptr),
//      high_freq_mixer_(nullptr),
//      low_freq_mixer_(nullptr),
//      mixer_(nullptr)
Density::Density(Simulation_context& ctx__)
    : ctx_(ctx__),
      parameters_(ctx__.parameters()),
      unit_cell_(ctx_.unit_cell()),
      gaunt_coefs_(nullptr),
      high_freq_mixer_(nullptr),
      low_freq_mixer_(nullptr),
      mixer_(nullptr)
{
    fft_ = ctx_.fft();

    rho_ = new Periodic_function<double>(unit_cell_, ctx_.step_function(), fft_, parameters_.lmmax_rho(), ctx_.comm());

    /* core density of the pseudopotential method */
    if (parameters_.esm_type() == ultrasoft_pseudopotential || 
        parameters_.esm_type() == norm_conserving_pseudopotential)
    {
        rho_pseudo_core_ = new Periodic_function<double>(unit_cell_, ctx_.step_function(), fft_, 0, ctx_.comm());
        rho_pseudo_core_->allocate(false, true);
        rho_pseudo_core_->zero();

        generate_pseudo_core_charge_density();
    }

    for (int i = 0; i < parameters_.num_mag_dims(); i++)
    {
        magnetization_[i] = new Periodic_function<double>(unit_cell_, ctx_.step_function(), fft_, parameters_.lmmax_rho(), ctx_.comm());
    }
    
    /* never change this order!!! */
    dmat_spins_.clear();
    dmat_spins_.push_back(std::pair<int, int>(0, 0));
    dmat_spins_.push_back(std::pair<int, int>(1, 1));
    dmat_spins_.push_back(std::pair<int, int>(0, 1));
    
    switch (parameters_.esm_type())
    {
        case full_potential_lapwlo:
        {
            gaunt_coefs_ = new Gaunt_coefficients<double_complex>(parameters_.lmax_apw(), parameters_.lmax_rho(), 
                                                                  parameters_.lmax_apw(), SHT::gaunt_hybrid);
            break;
        }
        case full_potential_pwlo:
        {
            gaunt_coefs_ = new Gaunt_coefficients<double_complex>(parameters_.lmax_pw(), parameters_.lmax_rho(), 
                                                                  parameters_.lmax_pw(), SHT::gaunt_hybrid);
            break;
        }
        case ultrasoft_pseudopotential:
        case norm_conserving_pseudopotential:
        {
            break;
        }
    }

    l_by_lm_ = Utils::l_by_lm(parameters_.lmax_rho());

    if (parameters_.esm_type() == ultrasoft_pseudopotential ||
        parameters_.esm_type() == norm_conserving_pseudopotential)
    {
        high_freq_mixer_ = new Linear_mixer<double_complex>((ctx_.fft()->num_gvec() - ctx_.fft_coarse()->num_gvec()),
                                                            parameters_.mixer_input_section().beta_, ctx_.comm());

        std::vector<double> weights(ctx_.fft_coarse()->num_gvec());
        weights[0] = 0;
        for (int ig = 1; ig < ctx_.fft_coarse()->num_gvec(); ig++)
            weights[ig] = fourpi * unit_cell_.omega() / std::pow(ctx_.fft_coarse()->gvec_len(ig), 2);

        if (parameters_.mixer_input_section().type_ == "linear")
        {
            low_freq_mixer_ = new Linear_mixer<double_complex>(ctx_.fft_coarse()->num_gvec(),
                                                               parameters_.mixer_input_section().beta_,
                                                               ctx_.comm());
        }
        else if (parameters_.mixer_input_section().type_ == "broyden2")
        {
            low_freq_mixer_ = new Broyden_mixer<double_complex>(ctx_.fft_coarse()->num_gvec(),
                                                                parameters_.mixer_input_section().max_history_,
                                                                parameters_.mixer_input_section().beta_,
                                                                weights,
                                                                ctx_.comm());
        } 
        else if (parameters_.mixer_input_section().type_ == "broyden_modified")
        {

            low_freq_mixer_ = new Broyden_modified_mixer<double_complex>(ctx_.fft_coarse()->num_gvec(),
                                                                         parameters_.mixer_input_section().max_history_,
                                                                         parameters_.mixer_input_section().beta_,
                                                                         weights,
                                                                         ctx_.comm());
        }
        else
        {
            TERMINATE("wrong mixer type");
        }
    }

    if (parameters_.esm_type() == full_potential_lapwlo ||
        parameters_.esm_type() == full_potential_pwlo)
    {
        if (parameters_.mixer_input_section().type_ == "linear")
        {
            mixer_ = new Linear_mixer<double>(size(),
                                              parameters_.mixer_input_section().beta_,
                                              ctx_.comm());
        }
        else if (parameters_.mixer_input_section().type_ == "broyden2")
        {
            std::vector<double> weights;
            mixer_ = new Broyden_mixer<double>(size(),
                                               parameters_.mixer_input_section().max_history_,
                                               parameters_.mixer_input_section().beta_,
                                               weights,
                                               ctx_.comm());

        }
        else if (parameters_.mixer_input_section().type_ == "broyden1")
        {
            std::vector<double> weights;
            mixer_ = new Broyden_modified_mixer<double>(size(),
                                                        parameters_.mixer_input_section().max_history_,
                                                        parameters_.mixer_input_section().beta_,
                                                        weights,
                                                        ctx_.comm());
        }
        else
        {
            TERMINATE("wrong mixer type");
        }
    }

}

Density::~Density()
{
    delete rho_;
    for (int j = 0; j < parameters_.num_mag_dims(); j++) delete magnetization_[j];

    if (parameters_.esm_type() == ultrasoft_pseudopotential ||
        parameters_.esm_type() == norm_conserving_pseudopotential) delete rho_pseudo_core_;
    
    if (gaunt_coefs_ != nullptr) delete gaunt_coefs_;
    if (low_freq_mixer_ != nullptr) delete low_freq_mixer_;
    if (high_freq_mixer_ != nullptr) delete high_freq_mixer_;
    if (mixer_ != nullptr) delete mixer_;
}

void Density::set_charge_density_ptr(double* rhomt, double* rhoir)
{
    if (parameters_.esm_type() == full_potential_lapwlo || parameters_.esm_type() == full_potential_pwlo)
        rho_->set_mt_ptr(rhomt);
    rho_->set_it_ptr(rhoir);
}

void Density::set_magnetization_ptr(double* magmt, double* magir)
{
    if (parameters_.num_mag_dims() == 0) return;
    assert(parameters_.num_spins() == 2);

    // set temporary array wrapper
    mdarray<double, 4> magmt_tmp(magmt, parameters_.lmmax_rho(), unit_cell_.max_num_mt_points(), 
                                 unit_cell_.num_atoms(), parameters_.num_mag_dims());
    mdarray<double, 2> magir_tmp(magir, fft_->size(), parameters_.num_mag_dims());
    
    if (parameters_.num_mag_dims() == 1)
    {
        // z component is the first and only one
        magnetization_[0]->set_mt_ptr(&magmt_tmp(0, 0, 0, 0));
        magnetization_[0]->set_it_ptr(&magir_tmp(0, 0));
    }

    if (parameters_.num_mag_dims() == 3)
    {
        // z component is the first
        magnetization_[0]->set_mt_ptr(&magmt_tmp(0, 0, 0, 2));
        magnetization_[0]->set_it_ptr(&magir_tmp(0, 2));
        // x component is the second
        magnetization_[1]->set_mt_ptr(&magmt_tmp(0, 0, 0, 0));
        magnetization_[1]->set_it_ptr(&magir_tmp(0, 0));
        // y component is the third
        magnetization_[2]->set_mt_ptr(&magmt_tmp(0, 0, 0, 1));
        magnetization_[2]->set_it_ptr(&magir_tmp(0, 1));
    }
}
    
void Density::zero()
{
    rho_->zero();
    for (int i = 0; i < parameters_.num_mag_dims(); i++) magnetization_[i]->zero();
}

void Density::generate_valence_density_it(K_set& ks)
{
    Timer t("sirius::Density::generate_valence_density_it", ctx_.comm());

    /* add k-point contribution */
    for (int ikloc = 0; ikloc < (int)ks.spl_num_kpoints().local_size(); ikloc++)
    {
        int ik = ks.spl_num_kpoints(ikloc);
        auto occupied_bands = get_occupied_bands_list(ks.band(), ks[ik]);
        add_k_point_contribution_it(ks[ik], occupied_bands);
    }
    
    /* reduce arrays; assume that each rank did it's own fraction of the density */
    ctx_.comm().allreduce(&rho_->f_it<global>(0), fft_->size()); 
    for (int j = 0; j < parameters_.num_mag_dims(); j++)
        ctx_.comm().allreduce(&magnetization_[j]->f_it<global>(0), fft_->size()); 
}

double Density::core_leakage()
{
    double sum = 0.0;
    for (int ic = 0; ic < unit_cell_.num_atom_symmetry_classes(); ic++)
    {
        sum += core_leakage(ic) * unit_cell_.atom_symmetry_class(ic)->num_atoms();
    }
    return sum;
}

double Density::core_leakage(int ic)
{
    return unit_cell_.atom_symmetry_class(ic)->core_leakage();
}

void Density::generate_core_charge_density()
{
    Timer t("sirius::Density::generate_core_charge_density");

    for (int icloc = 0; icloc < (int)unit_cell_.spl_num_atom_symmetry_classes().local_size(); icloc++)
    {
        int ic = unit_cell_.spl_num_atom_symmetry_classes(icloc);
        unit_cell_.atom_symmetry_class(ic)->generate_core_charge_density();
    }

    for (int ic = 0; ic < unit_cell_.num_atom_symmetry_classes(); ic++)
    {
        int rank = unit_cell_.spl_num_atom_symmetry_classes().local_rank(ic);
        unit_cell_.atom_symmetry_class(ic)->sync_core_charge_density(ctx_.comm(), rank);
    }
}

void Density::generate_pseudo_core_charge_density()
{
    Timer t("sirius::Density::generate_pseudo_core_charge_density");

    auto rl = ctx_.reciprocal_lattice();
    auto rho_core_radial_integrals = generate_rho_radial_integrals(2);

    std::vector<double_complex> v = rl->make_periodic_function(rho_core_radial_integrals, rl->num_gvec());

    fft_->input(fft_->num_gvec(), fft_->index_map(), &v[0]);
    fft_->transform(1);
    fft_->output(&rho_pseudo_core_->f_it<global>(0));
}

void Density::generate_valence(K_set& ks__)
{
    Timer t("sirius::Density::generate_valence", ctx_.comm());
    
    double wt = 0.0;
    double ot = 0.0;
    for (int ik = 0; ik < ks__.num_kpoints(); ik++)
    {
        wt += ks__[ik]->weight();
        for (int j = 0; j < parameters_.num_bands(); j++) ot += ks__[ik]->weight() * ks__[ik]->band_occupancy(j);
    }

    if (std::abs(wt - 1.0) > 1e-12) error_local(__FILE__, __LINE__, "K_point weights don't sum to one");

    if (std::abs(ot - unit_cell_.num_valence_electrons()) > 1e-8)
    {
        std::stringstream s;
        s << "wrong occupancies" << std::endl
          << "  computed : " << ot << std::endl
          << "  required : " << unit_cell_.num_valence_electrons() << std::endl
          << "  difference : " << fabs(ot - unit_cell_.num_valence_electrons());
        warning_local(__FILE__, __LINE__, s);
    }
    
    if (parameters_.esm_type() == ultrasoft_pseudopotential)
    {
        for (int ikloc = 0; ikloc < (int)ks__.spl_num_kpoints().local_size(); ikloc++)
        {
            int ik = ks__.spl_num_kpoints(ikloc);
            splindex<block> spl_bands(num_occupied_bands(ks__[ik]), ks__[ik]->comm().size(), ks__[ik]->comm().rank());
            ks__[ik]->collect_all_gkvec(spl_bands, ks__[ik]->fv_states_slab().at<CPU>(), ks__[ik]->fv_states().at<CPU>()); 
        }
    }

    /* zero density and magnetization */
    zero();

    /* interstitial part is independent of basis type */
    generate_valence_density_it(ks__);

    /* for muffin-tin part */
    switch (parameters_.esm_type())
    {
        case full_potential_lapwlo:
        {
            generate_valence_density_mt(ks__);
            break;
        }
        case full_potential_pwlo:
        {
            STOP();
        }
        default:
        {
            break;
        }
    }

    for (int ir = 0; ir < fft_->size(); ir++)
    {
        if (rho_->f_it<global>(ir) < 0) TERMINATE("density is wrong");
    }
    
    /* get rho(G) */
    rho_->fft_transform(-1);

    if (parameters_.esm_type() == ultrasoft_pseudopotential ||
        parameters_.esm_type() == norm_conserving_pseudopotential)
    {
        augment(ks__);
    }
}

void Density::augment(K_set& ks__)
{
    switch (parameters_.esm_type())
    {
        case ultrasoft_pseudopotential:
        {
            switch (parameters_.processing_unit())
            {
                case CPU:
                {
                    add_q_contribution_to_valence_density(ks__);
                    break;
                }
                #ifdef _GPU_
                case GPU:
                {
                    add_q_contribution_to_valence_density_gpu(ks__);
                    break;
                }
                #endif
                default:
                {
                    error_local(__FILE__, __LINE__, "wrong processing unit");
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void Density::generate(K_set& ks__)
{
    LOG_FUNC_BEGIN();

    Timer t("sirius::Density::generate", ctx_.comm());
    
    generate_valence(ks__);

    if (unit_cell_.full_potential())
    {
        generate_core_charge_density();

        /* add core contribution */
        for (int ialoc = 0; ialoc < (int)unit_cell_.spl_num_atoms().local_size(); ialoc++)
        {
            int ia = unit_cell_.spl_num_atoms(ialoc);
            for (int ir = 0; ir < unit_cell_.atom(ia)->num_mt_points(); ir++)
                rho_->f_mt<local>(0, ir, ialoc) += unit_cell_.atom(ia)->symmetry_class()->core_charge_density(ir) / y00;
        }

        /* synchronize muffin-tin part (interstitial is already syncronized with allreduce) */
        rho_->sync(true, false);
        for (int j = 0; j < parameters_.num_mag_dims(); j++) magnetization_[j]->sync(true, false);
    }
    
    double nel = 0;
    if (parameters_.esm_type() == full_potential_lapwlo ||
        parameters_.esm_type() == full_potential_pwlo)
    {
        std::vector<double> nel_mt;
        double nel_it;
        nel = rho_->integrate(nel_mt, nel_it);
    }
    if (parameters_.esm_type() == ultrasoft_pseudopotential ||
        parameters_.esm_type() == norm_conserving_pseudopotential)
    {
        nel = real(rho_->f_pw(0)) * unit_cell_.omega();
    }

    if (std::abs(nel - unit_cell_.num_electrons()) > 1e-5)
    {
        std::stringstream s;
        s << "wrong charge density after k-point summation" << std::endl
          << "obtained value : " << nel << std::endl 
          << "target value : " << unit_cell_.num_electrons() << std::endl
          << "difference : " << fabs(nel - unit_cell_.num_electrons()) << std::endl;
        if (unit_cell_.full_potential())
        {
            s << "total core leakage : " << core_leakage();
            for (int ic = 0; ic < unit_cell_.num_atom_symmetry_classes(); ic++) 
                s << std::endl << "  atom class : " << ic << ", core leakage : " << core_leakage(ic);
        }
        warning_global(__FILE__, __LINE__, s);
    }

    #ifdef _PRINT_OBJECT_HASH_
    DUMP("hash(rhomt): %16llX", rho_->f_mt().hash());
    DUMP("hash(rhoit): %16llX", rho_->f_it().hash());
    #endif

    //if (debug_level > 1) check_density_continuity_at_mt();
    LOG_FUNC_END();
}

//void Density::check_density_continuity_at_mt()
//{
//    // generate plane-wave coefficients of the potential in the interstitial region
//    ctx_.fft().input(&rho_->f_it<global>(0));
//    ctx_.fft().transform(-1);
//    ctx_.fft().output(parameters_.num_gvec(), ctx_.fft_index(), &rho_->f_pw(0));
//    
//    SHT sht(parameters_.lmax_rho());
//
//    double diff = 0.0;
//    for (int ia = 0; ia < parameters_.num_atoms(); ia++)
//    {
//        for (int itp = 0; itp < sht.num_points(); itp++)
//        {
//            double vc[3];
//            for (int x = 0; x < 3; x++) vc[x] = sht.coord(x, itp) * parameters_.atom(ia)->mt_radius();
//
//            double val_it = 0.0;
//            for (int ig = 0; ig < parameters_.num_gvec(); ig++) 
//            {
//                double vgc[3];
//                parameters_.get_coordinates<cartesian, reciprocal>(parameters_.gvec(ig), vgc);
//                val_it += real(rho_->f_pw(ig) * exp(double_complex(0.0, Utils::scalar_product(vc, vgc))));
//            }
//
//            double val_mt = 0.0;
//            for (int lm = 0; lm < parameters_.lmmax_rho(); lm++)
//                val_mt += rho_->f_rlm(lm, parameters_.atom(ia)->num_mt_points() - 1, ia) * sht.rlm_backward(lm, itp);
//
//            diff += fabs(val_it - val_mt);
//        }
//    }
//    printf("Total and average charge difference at MT boundary : %.12f %.12f\n", diff, diff / parameters_.num_atoms() / sht.num_points());
//}


void Density::save()
{
    if (ctx_.comm().rank() == 0)
    {
        HDF5_tree fout(storage_file_name, false);
        rho_->hdf5_write(fout["density"]);
        for (int j = 0; j < parameters_.num_mag_dims(); j++)
            magnetization_[j]->hdf5_write(fout["magnetization"].create_node(j));
    }
    ctx_.comm().barrier();
}

void Density::load()
{
    HDF5_tree fout(storage_file_name, false);
    rho_->hdf5_read(fout["density"]);
    for (int j = 0; j < parameters_.num_mag_dims(); j++)
        magnetization_[j]->hdf5_read(fout["magnetization"][j]);
}

void Density::generate_pw_coefs()
{
    fft_->input(&rho_->f_it<global>(0));
    fft_->transform(-1);
    fft_->output(fft_->num_gvec(), fft_->index_map(), &rho_->f_pw(0));
}

}
