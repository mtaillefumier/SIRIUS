
//extern "C" void FORTRAN(genylm)(int* lmax, double* tp, complex16* ylm); 

namespace sirius {


const double gamma_factors[31][11] = {
{1.5,3.75,13.125,59.0625,324.84375,2111.484375,15836.1328125,134607.12890625,1.278767724609375e6,1.34270611083984375e7,1.5441120274658203125e8},
{2.5,8.75,39.375,216.5625,1407.65625,10557.421875,89738.0859375,852511.81640625,8.951374072265625e6,1.029408018310546875e8,1.2867600228881835938e9},
{3.5,15.75,86.625,563.0625,4222.96875,35895.234375,341004.7265625,3.58054962890625e6,4.1176320732421875e7,5.147040091552734375e8,6.9485041235961914062e9},
{4.5,24.75,160.875,1206.5625,10255.78125,97429.921875,1.0230141796875e6,1.176466306640625e7,1.47058288330078125e8,1.9852868924560546875e9,2.8786659940612792969e10},
{5.5,35.75,268.125,2279.0625,21651.09375,227336.484375,2.6143695703125e6,3.267961962890625e7,4.41174864990234375e8,6.3970355423583984375e9,9.9154050906555175781e10},
{6.5,48.75,414.375,3936.5625,41333.90625,475339.921875,5.9417490234375e6,8.021361181640625e7,1.163097371337890625e9,1.8028009255737304687e10,2.9746215271966552734e11},
{7.5,63.75,605.625,6359.0625,73129.21875,914115.234375,1.23405556640625e7,1.7893805712890625e8,2.773539885498046875e9,4.5763408110717773438e10,8.0085964193756103516e11},
{8.5,80.75,847.875,9750.5625,121882.03125,1.645407421875e6,2.38584076171875e7,3.6980531806640625e8,6.101787748095703125e9,1.0678128559167480469e11,1.9754537834459838867e12},
{9.5,99.75,1147.125,14339.0625,193577.34375,2.806871484375e6,4.35065080078125e7,7.1785738212890625e8,1.2562504187255859375e10,2.3240632746423339844e11,4.5319233855525512695e12},
{10.5,120.75,1509.375,20376.5625,295460.15625,4.579632421875e6,7.55639349609375e7,1.32236886181640625e9,2.4463823943603515625e10,4.7704456690026855469e11,9.7794136214555053711e12},
{11.5,143.75,1940.625,28139.0625,436155.46875,7.196565234375e6,1.259398916015625e8,2.32988799462890625e9,4.5432815895263671875e10,9.3137272585290527344e11,2.0024513605837463379e13},
{12.5,168.75,2446.875,37926.5625,625788.28125,1.0951294921875e7,2.025989560546875e8,3.95067964306640625e9,8.0988932682861328125e10,1.7412620526815185547e12,3.917839618533416748e13},
{13.5,195.75,3034.125,50063.0625,876103.59375,1.6207916484375e7,3.160543714453125e8,6.47911461462890625e9,1.3930096421452148438e11,3.1342716948267333984e12,7.3655384828428234863e13},
{14.5,224.75,3708.375,64896.5625,1.20058640625e6,2.3411434921875e7,4.799344158984375e8,1.031858994181640625e10,2.3216827369086914063e11,5.4559544317354248047e12,1.3367088357751790771e14},
{15.5,255.75,4475.625,82799.0625,1.61458171875e6,3.3098925234375e7,7.116268925390625e8,1.601160508212890625e10,3.7627271943002929687e11,9.2186816260357177734e12,2.3507638146391080322e14},
{16.5,288.75,5341.875,104166.5625,2.13541453125e6,4.5911412421875e7,1.0330067794921875e9,2.427565931806640625e10,5.9475365329262695312e11,1.5166218158961987305e13,4.0190478121249266357e14},
{17.5,323.75,6313.125,129419.0625,2.78250984375e6,6.2606471484375e7,1.4712520798828125e9,3.604567595712890625e10,9.1916473690678710938e11,2.4357865528029858398e13,6.6984130202082110596e14},
{18.5,360.75,7395.375,159000.5625,3.57751265625e6,8.4071547421875e7,2.0597529118359375e9,5.252369925181640625e10,1.3918780301731347656e12,3.8276645829761206055e13,1.0908844061481943726e15},
{19.5,399.75,8594.625,193379.0625,4.54440796875e6,1.11337995234375e8,2.8391188784765625e9,7.523665027962890625e10,2.0690078826897949219e12,5.8966724656659155273e13,1.7395183773714450806e15},
{20.5,440.75,9916.875,233046.5625,5.70964078125e6,1.45595839921875e8,3.8582897579296875e9,1.0610296834306640625e11,3.0239345977773925781e12,8.9206070634433081055e13,2.7207851543502089722e15},
{21.5,483.75,11368.125,278519.0625,7.10223609375e6,1.88209256484375e8,5.1757545533203125e9,1.4750900476962890625e11,4.3515156407040527344e12,1.327212270414736084e14,4.1807186518064186646e15},
{22.5,528.75,12954.375,330336.5625,8.75391890625e6,2.40732769921875e8,6.8608839427734375e9,2.0239607631181640625e11,6.1730803275104003906e12,1.944520303165776123e14,6.3196909852887723999e15},
{23.5,575.75,14681.625,389063.0625,1.069923421875e7,3.04928175234375e8,8.9953811694140625e9,2.7435912566712890625e11,8.6423124585145605469e12,2.8087515490172321777e14,9.4093176892077277954e15},
{24.5,624.75,16555.875,455286.5625,1.297566703125e7,3.82782177421875e8,1.16748564113671875e10,3.6775797695806640625e11,1.1952134251137158203e13,4.003964974130947998e14,1.3813679160751770593e16},
{25.5,675.75,18583.125,529619.0625,1.562376234375e7,4.76524751484375e8,1.50105296717578125e10,4.8784221433212890625e11,1.6342714180126318359e13,5.638236392143579834e14,2.0015739192109708411e16},
{26.5,728.75,20769.375,612696.5625,1.868724515625e7,5.88648222421875e8,1.91310672287109375e10,6.4089075216181640625e11,2.2110730949582666016e13,7.8493094871018464355e14,2.864997962792173949e16},
{27.5,783.75,23120.625,705179.0625,2.221314046875e7,7.21927065234375e8,2.41845566853515625e10,8.3436720564462890625e11,2.9620035800384326172e13,1.0811313067140279053e15,4.0542424001776046448e16},
{28.5,840.75,25642.875,807750.5625,2.625189328125e7,8.79438424921875e8,3.03406256598046875e10,1.0770922109230664062e12,3.9313865698691923828e13,1.4742699637009471436e15,5.6759393602486465027e16},
{29.5,899.75,28342.125,921119.0625,3.085748859375e7,1.064583356484375e9,3.77927091551953125e10,1.3794338841646289062e12,5.1728770656173583984e13,1.9915576702626829834e15,7.8666527975375977844e16},
{30.5,960.75,31224.375,1.0460165625e6,3.608757140625e7,1.281108784921875e9,4.67604706496484375e10,1.7535176493618164062e12,6.7510429500429931641e13,2.6666619652669822998e15,1.0799980959331278314e17},
{31.5,1023.75,34295.625,1.1831990625e6,4.200356671875e7,1.533130185234375e9,5.74923819462890625e10,2.2134567049321289062e12,8.7431539844819091797e13,3.5409773637151732178e15,1.4695056059417968854e17}};




/*! 
    \brief Generate effective potential from charge density and magnetization
   
*/
class Potential 
{
    private:

        mdarray<complex16,2> ylm_gvec_;

        mdarray<double,2> pseudo_mom_;
        mdarray<double,3> sbessel_mom_;

        mdarray<double,3> sbessel_pseudo_prod_;

        mdarray<double,3> sbessel_mt_;
        
        PeriodicFunction<double> hartree_potential_;
        
        PeriodicFunction<double> xc_potential_;
        
        PeriodicFunction<double> xc_energy_density_;
        
        PeriodicFunction<double> effective_potential_;

        SHT sht_;

        double pseudo_density_l(int l, double x)
        {
            return (1.0 + cos(2 * pi * (x - 0.5))) * exp(l * x);
        }

    public:
        // TODO: look at this: Integrate[SphericalBesselJ[l, a*x]*(1/R)^l*(1 - (x/R)^2)^n*x^(2 + l), {x, 0,R}, Assumptions -> {l >= 0, n > 0, R > 0, a > 0}]
        void init()
        {
            Timer t("sirius::Potential::init");
            
            int lmax = std::max(global.lmax_rho(), global.lmax_pot());
            
            // compute moments of pseudodensity
            pseudo_mom_.set_dimensions(global.lmax_rho() + 1, global.num_atom_types());
            pseudo_mom_.allocate();

            for (int iat = 0; iat < global.num_atom_types(); iat++)
            { 
                int nmtp = global.atom_type(iat)->num_mt_points();
                Spline<double> s(nmtp, global.atom_type(iat)->radial_grid()); 
                
                for (int l = 0; l <= global.lmax_rho(); l++)
                {
                    for (int ir = 0; ir < nmtp; ir++)
                        s[ir] = pseudo_density_l(l, global.atom_type(iat)->radial_grid()[ir] / global.atom_type(iat)->mt_radius());
                    s.interpolate();

                    pseudo_mom_(l, iat) = s.integrate(2 + l);
                }
            }

            // compute moments of spherical Bessel functions 
            sbessel_mom_.set_dimensions(global.lmax_rho() + 1, global.num_atom_types(), global.num_gvec_shells());
            sbessel_mom_.allocate();
            sbessel_mom_.zero();

            for (int igs = 0; igs < global.num_gvec_shells(); igs++)
                for (int iat = 0; iat < global.num_atom_types(); iat++)
                    if (igs == 0) 
                        sbessel_mom_(0, iat, 0) = pow(global.atom_type(iat)->mt_radius(), 3) / 3.0;
                    else
                        for (int l = 0; l <= global.lmax_rho(); l++)
                        {
                            double t = gsl_sf_bessel_Jnu(1.5 + l, global.gvec_shell_len(igs) * global.atom_type(iat)->mt_radius());
                            sbessel_mom_(l, iat, igs) = sqrt(pi / 2) * pow(global.atom_type(iat)->mt_radius(), 1.5 + l) * t / pow(global.gvec_shell_len(igs), 1.5);
                        }
            
            // compute spherical harmonics of G-vectors
            ylm_gvec_.set_dimensions(lmmax_by_lmax(lmax), global.num_gvec());
            ylm_gvec_.allocate();
            
            for (int ig = 0; ig < global.num_gvec(); ig++)
            {
                double cartc[3];
                double spc[3];
                global.get_coordinates<cartesian,reciprocal>(global.gvec(ig), cartc);
                SHT::spherical_coordinates(cartc, spc);
                SHT::spherical_harmonics(lmax, spc[1], spc[2], &ylm_gvec_(0, ig));
            }
            
            // compute product of spherical Bessel functions with pseudocharge density
            sbessel_pseudo_prod_.set_dimensions(global.lmax_rho() + 1, global.num_atom_types(), global.num_gvec_shells());
            sbessel_pseudo_prod_.allocate();
            
            sbessel_mt_.set_dimensions(global.lmax_pot() + 1, global.num_atom_types(), global.num_gvec_shells());
            sbessel_mt_.allocate();

            mdarray<double,2> jl(NULL, lmax + 1, global.max_num_mt_points());
            jl.allocate();

            for (int iat = 0; iat < global.num_atom_types(); iat++)
            {
                int nmtp = global.atom_type(iat)->num_mt_points();
                Spline<double> s(nmtp, global.atom_type(iat)->radial_grid()); 

                for (int igs = 0; igs < global.num_gvec_shells(); igs++)
                {
                    // compute spherical Bessel functions
                    for (int ir = 0; ir < nmtp; ir++)
                        gsl_sf_bessel_jl_array(lmax, global.gvec_shell_len(igs) * global.atom_type(iat)->radial_grid()[ir], &jl(0, ir));
                
                    for (int l = 0; l <= lmax ; l++)
                    {
                        // save value of the Bessel function at the MT boundary
                        if (l <= global.lmax_pot())
                            sbessel_mt_(l, iat, igs) = jl(l, nmtp - 1);

                        if (l <= global.lmax_rho())
                        {
                            for (int ir = 0; ir < nmtp; ir++)
                                s[ir] = jl(l, ir) * pseudo_density_l(l, global.atom_type(iat)->radial_grid()[ir] / global.atom_type(iat)->mt_radius());
                            s.interpolate();

                            sbessel_pseudo_prod_(l, iat, igs) = s.integrate(2);
                        }
                    }
                }
            }
#if 0
            std::ofstream out("sbessel_pseudo_prod.dat");
            for (int l = 0; l <= global.lmax_rho(); l++)
            {
                for (int igs = 0; igs < global.num_gvec_shells(); igs++)
                    out << global.gvec_shell_len(igs) << " " << sbessel_pseudo_prod_(l, 0, igs) << std::endl;
                out << std::endl;
            }
            out.close();
#endif
            effective_potential_.allocate(global.lmax_pot(), global.max_num_mt_points(), global.num_atoms(),
                                          global.fft().size(), global.num_gvec());

            sht_.set_lmax(lmax);
        }

        
        /*! 
            \brief Poisson solver
            
            plane wave expansion
            \f[
                e^{i{\bf g}{\bf r}}=4\pi e^{i{\bf g}{\bf r}_{\alpha}} \sum_{\ell m} i^\ell j_{\ell}(g|{\bf r}-{\bf r}_{\alpha}|)
                    Y_{\ell m}^{*}({\bf \hat g}) Y_{\ell m}(\widehat{{\bf r}-{\bf r}_{\alpha}})
            \f]

            Multipole moment:
            \f[
                q_{\ell m} = \int Y_{\ell m}^{*}(\hat {\bf r}) r^l \rho({\bf r}) d {\bf r}

            \f]

            Spherical Bessel function moments
            \f[
                \int_0^R j_{\ell}(a x)x^{2+\ell} dx = \frac{\sqrt{\frac{\pi }{2}} R^{\ell+\frac{3}{2}} 
                   J_{\ell+\frac{3}{2}}(a R)}{a^{3/2}}
            \f]
            for a = 0 the integral is \f$ \frac{R^3}{3} \delta_{\ell,0} \f$

            General solution to the Poisson equation with spherical boundary condition:
            \f[
                V({\bf x}) = \int \rho({\bf x'})G({\bf x},{\bf x'}) d{\bf x'} - \frac{1}{4 \pi} \int_{S} V({\bf x'}) \frac{\partial G}{\partial n'} d{\bf S'}
            \f]

            Green's function for a sphere
            \f[
                G({\bf x},{\bf x'}) = 4\pi \sum_{\ell m} \frac{Y_{\ell m}^{*}(\hat {\bf x'}) Y_{\ell m}(\hat {\bf x})}{2\ell + 1}
                    \frac{r_{<}^{\ell}}{r_{>}^{\ell+1}}\Biggl(1 - \Big( \frac{r_{>}}{R} \Big)^{2\ell + 1} \Biggr)
            \f]

        */
        void poisson()
        {
            Timer t("sirius::Potential::poisson");

            // convert to Ylm expansion
            density.charge_density().convert_to_ylm();
           
            // true multipole moments
            mdarray<complex16,2> qmt(NULL, global.lmmax_rho(), global.num_atoms());
            qmt.allocate();
            
            // compute MT part of the potential and MT multipole moments
            for (int ia = 0; ia < global.num_atoms(); ia++)
            {
                double R = global.atom(ia)->type()->mt_radius();
                int nmtp = global.atom(ia)->type()->num_mt_points();
                
                std::vector<complex16> g1;
                std::vector<complex16> g2;
   
                Spline<complex16> rholm(nmtp, global.atom(ia)->type()->radial_grid());

                for (int lm = 0; lm < global.lmmax_rho(); lm++)
                {
                    int l = l_by_lm(lm);

                    for (int ir = 0; ir < nmtp; ir++)
                        rholm[ir] = density.charge_density().f_ylm(lm, ir, ia);
                    rholm.interpolate();

                    // save multipole moment
                    qmt(lm, ia) = rholm.integrate(g1, l + 2);
                    
                    if (lm < global.lmmax_pot())
                    {
                        rholm.integrate(g2, 1 - l);
                        
                        for (int ir = 0; ir < nmtp; ir++)
                        {
                            double r = global.atom(ia)->type()->radial_grid()[ir];

                            complex16 vlm = (1.0 - pow(r / R, 2 * l + 1)) * g1[ir] / pow(r, l + 1) +
                                            (g2[nmtp - 1] - g2[ir]) * pow(r, l) - 
                                            (g1[nmtp - 1] - g1[ir]) * pow(r, l) / pow(R, 2 * l + 1);

                            hartree_potential_.f_ylm(lm, ir, ia) = fourpi * vlm / double(2 * l + 1);
                        }
                    }
                }
                
                // nuclear potential
                for (int ir = 0; ir < nmtp; ir++)
                {
                    double r = global.atom(ia)->type()->radial_grid()[ir];
                    hartree_potential_.f_ylm(0, ir, ia) -= fourpi * y00 * global.atom(ia)->type()->zn() * (1.0 / r - 1.0 / R);
                }

                // nuclear multipole moment
                qmt(0, ia) -= global.atom(ia)->type()->zn() * y00;
            }

            // compute multipoles of interstitial density in MT region
            mdarray<complex16,2> qit(NULL, global.lmmax_rho(), global.num_atoms());
            qit.allocate();
            qit.zero();

            std::vector<complex16> zil(global.lmax_rho() + 1);
            for (int l = 0; l <= global.lmax_rho(); l++)
                zil[l] = pow(zi, l);

            for (int ia = 0; ia < global.num_atoms(); ia++)
            {
                int iat = global.atom_type_index_by_id(global.atom(ia)->type_id());
                
                for (int ig = 0; ig < global.num_gvec(); ig++)
                {
                    complex16 zt = fourpi * global.gvec_phase_factor(ig, ia) * density.charge_density().f_pw(ig);

                    for (int lm = 0; lm < global.lmmax_rho(); lm++)
                    {
                        int l = l_by_lm(lm);

                        qit(lm, ia) += zt * zil[l] * conj(ylm_gvec_(lm, ig)) * sbessel_mom_(l, iat, global.gvec_shell(ig));
                    }
                }
            }

            for (int lm = 0; lm < global.lmmax_rho(); lm++)
            {
                complex16 q1 = qmt(lm, 0);
                complex16 q2 = qit(lm, 0);

                printf("lm=%i   qmt=%18.12f %18.12f   qit=%18.12f %18.12f \n", lm, real(q1), imag(q1), real(q2), imag(q2));
            }
            
            std::cout << "rho(0) = " << density.charge_density().f_pw(0) << std::endl;

            std::vector<complex16> pseudo_pw(global.num_gvec());
            memcpy(&pseudo_pw[0], density.charge_density().f_pw(), global.num_gvec() * sizeof(complex16));

            // add contribution from pseudocharge density
            for (int ia = 0; ia < global.num_atoms(); ia++)
            {
                int iat = global.atom_type_index_by_id(global.atom(ia)->type_id());
                
                for (int ig = 0; ig < global.num_gvec(); ig++)
                {
                    complex16 zt = fourpi * conj(global.gvec_phase_factor(ig, ia));

                    for (int lm = 0; lm < global.lmmax_rho(); lm++)
                    {
                        int l = l_by_lm(lm);

                        pseudo_pw[ig] += zt * conj(zil[l]) * ylm_gvec_(lm, ig) * sbessel_pseudo_prod_(l, iat, global.gvec_shell(ig)) * 
                            (qmt(lm, ia) - qit(lm, ia)) / pseudo_mom_(l, iat) / global.omega();
                    }
                }
            }

            std::cout << "rho(0) = " << pseudo_pw[0] << std::endl;

            qit.zero();
            for (int ia = 0; ia < global.num_atoms(); ia++)
            {
                int iat = global.atom_type_index_by_id(global.atom(ia)->type_id());
                
                for (int ig = 0; ig < global.num_gvec(); ig++)
                {
                    complex16 zt = fourpi * global.gvec_phase_factor(ig, ia);

                    for (int lm = 0; lm < global.lmmax_rho(); lm++)
                    {
                        int l = l_by_lm(lm);

                        qit(lm, ia) += pseudo_pw[ig] * zt * zil[l] * conj(ylm_gvec_(lm, ig)) * sbessel_mom_(l, iat, global.gvec_shell(ig));
                    }
                }
            }
           
            for (int lm = 0; lm < global.lmmax_rho(); lm++)
            {
                complex16 q1 = qmt(lm, 0);
                complex16 q2 = qit(lm, 0);

                printf("lm=%i   qmt=%18.12f %18.12f   qit=%18.12f %18.12f \n", lm, real(q1), imag(q1), real(q2), imag(q2));
            }

            double d = 0.0;
            for (int ia = 0; ia < global.num_atoms(); ia++)
                for (int lm = 0; lm < global.lmmax_rho(); lm++)
                    d += abs(qmt(lm, ia) - qit(lm, ia));
            std::cout << "total q diff : " << d << std::endl;


 
            // compute pw coefficients of Hartree potential
            pseudo_pw[0] = 0.0;
            hartree_potential_.f_pw(0) = 0.0;
            for (int ig = 1; ig < global.num_gvec(); ig++)
                hartree_potential_.f_pw(ig) = pseudo_pw[ig] * fourpi / pow(global.gvec_shell_len(global.gvec_shell(ig)), 2);

            // compute V_lm at the MT boundary
            mdarray<complex16,2> vmtlm(NULL, global.lmmax_pot(), global.num_atoms());
            vmtlm.allocate();
            vmtlm.zero();
            
            for (int ia = 0; ia < global.num_atoms(); ia++)
            {
                int iat = global.atom_type_index_by_id(global.atom(ia)->type_id());

                for (int ig = 0; ig < global.num_gvec(); ig++)
                {
                    complex16 zt = fourpi * global.gvec_phase_factor(ig, ia) * hartree_potential_.f_pw(ig);

                    for (int lm = 0; lm < global.lmmax_pot(); lm++)
                    {
                        int l = l_by_lm(lm);

                        vmtlm(lm, ia) += zt * zil[l] * sbessel_mt_(l, iat, global.gvec_shell(ig)) * conj(ylm_gvec_(lm, ig));
                    }
                }
            }

            // add boundary condition
            for (int ia = 0; ia < global.num_atoms(); ia++)
            {
                double R = global.atom(ia)->type()->mt_radius();

                for (int lm = 0; lm < global.lmmax_pot(); lm++)
                {
                    int l = l_by_lm(lm);

                    for (int ir = 0; ir < global.atom(ia)->type()->num_mt_points(); ir++)
                        hartree_potential_.f_ylm(lm, ir, ia) += vmtlm(lm, ia) * pow(global.atom(ia)->type()->radial_grid()[ir] / R, l);
                }
            }
            
            hartree_potential_.convert_to_rlm();

            density.charge_density().deallocate_ylm();

            global.fft().input(global.num_gvec(), global.fft_index(), hartree_potential_.f_pw());
            global.fft().backward();
            global.fft().output(hartree_potential_.f_it());
        }

        void xc()
        {
            mdarray<double,2> rhotp(sht_.num_points(), global.max_num_mt_points());

            mdarray<double,2> vxctp(sht_.num_points(), global.max_num_mt_points());
            
            mdarray<double,2> exctp(sht_.num_points(), global.max_num_mt_points());

            for (int ia = 0; ia < global.num_atoms(); ia++)
            {
                int nmtp = global.atom(ia)->type()->num_mt_points();

                sht_.rlm_backward_transform(&density.charge_density().f_rlm(0, 0, ia), global.lmmax_rho(), nmtp, &rhotp(0, 0));
                
                xc_potential::get(sht_.num_points() * nmtp, &rhotp(0, 0), &vxctp(0, 0), &exctp(0, 0));

                sht_.rlm_forward_transform(&vxctp(0, 0), global.lmmax_rho(), nmtp, &xc_potential_.f_rlm(0, 0, ia));
                
                sht_.rlm_forward_transform(&exctp(0, 0), global.lmmax_rho(), nmtp, &xc_energy_density_.f_rlm(0, 0, ia));
            }

            xc_potential::get(global.fft().size(), density.charge_density().f_it(), xc_potential_.f_it(), xc_energy_density_.f_it());
        }

        void generate_effective_potential()
        {
            effective_potential_.zero();
            
            // generate and add Hartree potential
            hartree_potential_.allocate(global.lmax_pot(), global.max_num_mt_points(), global.num_atoms(),
                                        global.fft().size(), global.num_gvec());
            hartree_potential_.allocate_ylm();

            poisson();
            
            effective_potential_.add(hartree_potential_, add_rlm | add_it);

            hartree_potential_.deallocate();

            // generate and add XC potential
            xc_potential_.allocate(global.lmax_pot(), global.max_num_mt_points(), global.num_atoms(),
                                   global.fft().size(), global.num_gvec());
            
            xc_energy_density_.allocate(global.lmax_pot(), global.max_num_mt_points(), global.num_atoms(),
                                        global.fft().size(), global.num_gvec());

            xc();
            
            effective_potential_.add(xc_potential_, add_rlm | add_it);
            
            xc_potential_.deallocate();
            
            xc_energy_density_.deallocate();

            for (int ir = 0; ir < global.fft().size(); ir++)
                 effective_potential_.f_it(ir) *= global.step_function(ir);

            global.fft().input(effective_potential_.f_it());
            global.fft().forward();
            global.fft().output(global.num_gvec(), global.fft_index(), effective_potential_.f_pw());

#if 0            
            std::ofstream out("pot.dat");

            int nmtp = global.atom(0)->type()->num_mt_points();
            for (int ir = 0; ir < nmtp; ir++)
                out << global.atom(0)->type()->radial_grid()[ir] << " " << global.atom(0)->type()->free_atom_potential(ir) / y00 << std::endl; 
            
            out << std::endl;

            for (int lm = 0; lm < global.lmmax_pot(); lm++)
            {
                for (int ir = 0; ir < nmtp; ir++)
                    out << global.atom(0)->type()->radial_grid()[ir] << " " << effective_potential_.f_rlm(lm, ir, 0) << std::endl;
                out << std::endl;
            }
            
            out.close();
#endif
        }

        inline PeriodicFunction<double>& effective_potential()
        {
            return effective_potential_;
        }
};

Potential potential;

};
