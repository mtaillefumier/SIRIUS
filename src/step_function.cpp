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

/** \file step_function.cpp
 *
 *  \brief Contains remaining implementation of sirius::Step_function class. 
 */

#include "step_function.h"

namespace sirius {

Step_function::Step_function(Unit_cell const& unit_cell__,
                             Reciprocal_lattice const* reciprocal_lattice__,
                             FFT3D<CPU>* fft__,
                             Communicator const& comm__)
    : unit_cell_(unit_cell__),
      reciprocal_lattice_(reciprocal_lattice__),
      comm_(comm__)
{
    init(fft__);
}

mdarray<double, 2> Step_function::get_step_function_form_factors(int num_gsh)
{
    mdarray<double, 2> ffac(unit_cell_.num_atom_types(), num_gsh);
    
    splindex<block> spl_num_gvec_shells(num_gsh, comm_.size(), comm_.rank());

    #pragma omp parallel for default(shared)
    for (int igsloc = 0; igsloc < (int)spl_num_gvec_shells.local_size(); igsloc++)
    {
        int igs = (int)spl_num_gvec_shells[igsloc];
        double G = reciprocal_lattice_->gvec_shell_len(igs);
        double g3inv = (igs) ? 1.0 / pow(G, 3) : 0.0;

        for (int iat = 0; iat < unit_cell_.num_atom_types(); iat++)
        {            
            double R = unit_cell_.atom_type(iat)->mt_radius();
            double GR = G * R;

            ffac(iat, igs) = (igs) ? (sin(GR) - GR * cos(GR)) * g3inv : pow(R, 3) / 3.0;
        }
    }
    
    int ld = unit_cell_.num_atom_types(); 
    comm_.allgather(ffac.at<CPU>(), static_cast<int>(ld * spl_num_gvec_shells.global_offset()), 
                    static_cast<int>(ld * spl_num_gvec_shells.local_size()));
    return ffac;
}

void Step_function::init(FFT3D<CPU>* fft__)
{
    Timer t("sirius::Step_function::init");

    if (unit_cell_.num_atoms() == 0) return;
    
    auto ffac = get_step_function_form_factors(reciprocal_lattice_->num_gvec_shells_total());

    step_function_pw_.resize(fft__->size());
    step_function_.resize(fft__->size());
    
    std::vector<double_complex> f_pw = reciprocal_lattice_->make_periodic_function(ffac, fft__->size());
    for (int ig = 0; ig < fft__->size(); ig++) step_function_pw_[ig] = -f_pw[ig];
    step_function_pw_[0] += 1.0;

    fft__->input(fft__->size(), fft__->index_map(), &step_function_pw_[0]);
    fft__->transform(1);
    fft__->output(&step_function_[0]);
    
    double vit = 0.0;
    for (int i = 0; i < fft__->size(); i++) vit += step_function_[i] * unit_cell_.omega() / fft__->size();
    
    if (std::abs(vit - unit_cell_.volume_it()) > 1e-10)
    {
        std::stringstream s;
        s << "step function gives a wrong volume for IT region" << std::endl
          << "  difference with exact value : " << std::abs(vit - unit_cell_.volume_it());
        warning_global(__FILE__, __LINE__, s);
    }
    #ifdef _PRINT_OBJECT_CHECKSUM_
    double_complex z1 = mdarray<double_complex, 1>(&step_function_pw_[0], fft_->size()).checksum();
    double d1 = mdarray<double, 1>(&step_function_[0], fft_->size()).checksum();
    DUMP("checksum(step_function): %18.10f", d1);
    DUMP("checksum(step_function_pw): %18.10f %18.10f", std::real(z1), std::imag(z1));
    #endif
}

}
