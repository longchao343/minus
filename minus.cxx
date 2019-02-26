// 
// \author Ricardo Fabbri based on original code by Anton Leykin 
// \date Created: Fri Feb  8 17:42:49 EST 2019
// 
#include "minus.h"
#include <cstdio>
#include <iostream>
#include "Eigen/Dense"
#include "chicago.h"

/* Lapack .h */
#if 0
extern "C" {
int sgesv_(int *n,      // number of rows in A
           int *nrhs,   // number of right hand sides
           double *a,   // n by n matrix A, on exit L&U from A=PLU
           int *lda,    // n
           int *ipiv,   // indices defining permutation P
           double *b,   // right-hand-side
           int *ldb,    // n
           int *info);  // error info

};
#endif


// Place any specific-type functions here


#if 0
// Original code solve_via_lapack_without_transposition
//
// \returns  lapack info code. 
//      >0 -> matrix is singular
//      <0 -> illegal value of an argument passed to lapack
bool linear(
    const complex* A,  // NNN-by-NNN matrix of complex #s
    const complex* b,  // 1-by-NNN RHS of Ax=b  (bsize-by-NNN)
    complex* x   // solution
    )
{
  int info;
  static int permutation[NNN]; // unused
  static int bsize = 1;
  static int size = NNN;
  
  double *copyA = (double*)A;
  
  // TODO try to eliminate this memcpy and trash the original b if possible
  // memcpy  b -> x          NNN elements
  std::memcpy(x, b, 2*NNN*sizeof(double));

  double *copyb = (double*)x;  // result is stored in copyb
  
  sgesv_(&size, &bsize, copyA, &size, permutation, copyb, &size, &info);

  return info == 0;
}
#endif

bool linear_eigen(
    const complex* A,  // NNN-by-NNN matrix of complex #s
    const complex* b,  // 1-by-NNN RHS of Ax=b  (bsize-by-NNN)
    complex* x   // solution
    )
{
  using namespace Eigen;
  
  Map<Matrix<complex, NNN, 1> > xx(x);
  Map<const Matrix<complex, NNN, NNN> > AA(A,NNN,NNN);  // accessors for the data
  Map<const Matrix<complex, NNN, 1> > bb(b);
  
  xx = AA.colPivHouseholderQr().solve(bb);
  return true;
}
  

bool linear_eigen2(
    const complex* A,  // NNN-by-NNN matrix of complex #s
    const complex* b,  // 1-by-NNN RHS of Ax=b  (bsize-by-NNN)
    complex* x   // solution
    )
{
  using namespace Eigen;
  
  Map<Matrix<complex, NNN, 1> > xx(x);
  Map<const Matrix<complex, NNN, NNN> > AA(A,NNN,NNN);  // accessors for the data
  Map<const Matrix<complex, NNN, 1> > bb(b);
  
  xx = AA.partialPivLu().solve(bb);
  return true; // TODO: better error handling
}

bool linear_eigen3(
    const complex* A,  // NNN-by-NNN matrix of complex #s
    const complex* b,  // 1-by-NNN RHS of Ax=b  (bsize-by-NNN)
    complex* x   // solution
    )
{
  using namespace Eigen;
  
  Map<Matrix<complex, NNN, 1> > xx(x);
  Map<const Matrix<complex, NNN, NNN> > AA(A,NNN,NNN);  // accessors for the data
  Map<const Matrix<complex, NNN, 1> > bb(b);
  
  xx = AA.fullPivLu().solve(bb);
  return true;
}

bool linear_eigen4(
    const complex* A,  // NNN-by-NNN matrix of complex #s
    const complex* b,  // 1-by-NNN RHS of Ax=b  (bsize-by-NNN)
    complex* x   // solution
    )
{
  using namespace Eigen;
  
  Map<Matrix<complex, NNN, 1> > xx(x);
  Map<const Matrix<complex, NNN, NNN> > AA(A,NNN,NNN);  // accessors for the data
  Map<const Matrix<complex, NNN, 1> > bb(b);
  
  xx = AA.householderQr().solve(bb);
  return true;
}
// from compute_Grabner_basis, 5point.c
#if 0
bool linear_bundler(
    const complex* A,  // NNN-by-NNN matrix of complex #s
    const complex* b,  // 1-by-NNN RHS of Ax=b  (bsize-by-NNN)
    complex* x   // solution
    )
{
    for (unsigned i = 0; i < 10; i++) {
        /* Make the leading coefficient of row i = 1 */
        double leading = A[20 * i + i];
        matrix_scale(20, 1, A + 20 * i, 1.0 / leading, A + 20 * i);

        /* Subtract from other rows */
        for (j = i+1; j < 10; j++) {
            double leading2 = A[20 * j + i];
            double scaled_row[20];
            matrix_scale(20, 1, A + 20 * i, leading2, scaled_row);
            matrix_diff(20, 1, 20, 1, A + 20 * j, scaled_row, A + 20 * j);
        }
    }

    /* Now, do the back substitution */
    for (i = 9; i >= 0; i--) {
        for (j = 0; j < i; j++) {
            double scale = A[20 * j + i];
            double scaled_row[20];
            matrix_scale(20, 1, A + 20 * i, scale, scaled_row);
            matrix_diff(20, 1, 20, 1, A + 20 * j, scaled_row, A + 20 * j);
        }
    }
    
    /* Copy out results */
    for (i = 0; i < 10; i++) {
        memcpy(Gbasis + i * 10, A + i * 20 + 10, sizeof(double) * 10);
    }
}
#endif

static const double the_smallest_number = 1e-13;
static const double dbgtol = 1e-2;

void
array_print(std::string name, const complex *a)
{
  std::cerr << name + "[NNN]: ";
  std::cerr << a[0] << " , " << a[1] << " , ... " <<  a[NNN-1] << std::endl;
}

void
array_print_NNNplus1(std::string name, const complex *a)
{
  std::cerr << name + "[NNN+1]: ";
  std::cerr << a[0] << " , " << a[1] << " , ... " <<  a[NNN] << std::endl;
}

void
array_print_n(std::string name, const complex *a, unsigned n)
{
  assert(n > 3);
  std::cerr << name + "[" + std::to_string(n) + "]: ";
  std::cerr << a[0] << " , " << a[1] << " , ... " <<  a[n-1] << std::endl;
}

void
array_print_H(std::string name, const complex *a)
{
  std::cerr << name + "[][]: ";
  std::cerr << a[0] << " , " << a[1] << " , ... " <<  a[NNN*NNN-1] << std::endl;
  std::cerr << "\tlast col: ";
  std::cerr << a[NNN*NNN] << " , " << a[NNN*NNN+1] << " , ... " <<  a[NNN*(NNN+1)-1] << std::endl;
  std::cerr << "\tlast col line-based: ";
  std::cerr << a[NNN] << " , " << a[2*NNN+1] << " , ... " <<  a[NNN*(NNN+1)-1] << std::endl;
}

inline void 
array_multiply_scalar_to_self(complex *a, complex b)
{
  // TODO: optimize. Idea: use Eigen's parallizable structures or VNL SSE
  for (unsigned i = 0; i < NNN; ++i, ++a) *a = *a * b;
}

inline void
array_negate_self(complex *a)
{
  // TODO: optimize. Idea: use Eigen's parallizable structures or VNL SSE
  for (unsigned i = 0; i < NNN; ++i, ++a) *a = -*a;
}


inline void 
array_multiply_self(complex *a, const complex *b)
{
  for (unsigned int i=0; i < NNN; ++i,++a,++b) *a *= *b;
}

inline void 
array_add_to_self(complex *a, const complex *b)
{
  for (unsigned int i=0; i < NNN; ++i,++a,++b) *a += *b;
}

inline void 
array_add_to_self_NNNplus1(complex *a, const complex *b)
{
  for (unsigned int i=0; i < NNNPLUS1; ++i,++a,++b) *a += *b;
}

inline void 
array_add_scalar_to_self(complex *a, complex b)
{
  for (unsigned int i=0; i < NNN; ++i,++a) *a += b;
}

void 
array_copy(
  const complex *a,
  complex *b)
{
  // for (int i = 0; i < n; i++, a++, b++) *b = *a;
  memcpy(b, a, NNN*sizeof(complex));
}

void 
array_copy_NNNplus1(
  const complex *a,
  complex *b)
{
  // for (int i = 0; i < n; i++, a++, b++) *b = *a;
  memcpy(b, a, NNNPLUS1*sizeof(complex));
}


double
array_norm2(const complex *a)
{
  double val = 0;
  complex const* end = a+NNN;
  while (a != end)
    val += std::norm(*a++);
  return val;
}

//void 
//array_copy_n(size_t n, const complex *a, complex *b)
//{
//   for (int i = 0; i < n; i++, a++, b++) *b = *a;
//  memcpy(b, a, n*sizeof(complex));
//}

// t: tracker settings
// s_sols: start sols      
// params: params of target as specialized homotopy params - P01 in SolveChicago
unsigned    
ptrack(const TrackerSettings *s, const complex s_sols[NNN*NSOLS], const complex params[2*NPARAMS], Solution raw_solutions[NSOLS])
{
  // TODO: test by making variables static for a second run, some of these arrays may have to be zeroed
  // One huge clear instruction will work as they are sequential in mem.
  const complex one_half(0.5, 0);
  const double t_step = s->init_dt_;  // initial step
  complex x0t0[NNNPLUS1];  // t = real running in [0,1]
  complex *x0 = x0t0;
  complex *t0 = x0t0 + NNN;
  //  complex* x1 =  x1t1;
  //  complex* t1 = x1t1+NNN;
  complex dxdt[NNNPLUS1], *dx = dxdt, *dt = dxdt + NNN;
  complex Hxt[NNNPLUS1 * NNN], HxH[NNNPLUS1 * NNN], Hxt_tr[NNNPLUS1 * NNN];
  complex *LHS, *RHS;
  complex xt[NNNPLUS1+2*NPARAMS];  // concatenating parameters to x and t, used together in evals
  complex dx1[NNN], dx2[NNN], dx3[NNN], dx4[NNN];
  complex *x1t1 = xt;  // reusing xt's space to represent x1t1

  memcpy(xt+NNNPLUS1, params, 2*NPARAMS*sizeof(complex));
  
  Solution* t_s = raw_solutions;  // current target solution
  const complex* s_s = s_sols;          // current start solution
  for (unsigned sol_n = 0; sol_n < NSOLS; ++sol_n, s_s += NNN, ++t_s) { // outer loop
    std::cerr << "Trying solution #" << sol_n << std::endl;
    t_s->make(s_s);  // cook a Solution: copy s_s to start node of path
    t_s->status = PROCESSING;
    bool end_zone = false;
    array_copy(s_s, x0);
    *t0 = 0;
    *dt = t_step;
    unsigned predictor_successes = 0, count = 0;  // number of steps

    // print start solution

    // PASS array_print("s_s",s_s);
    
    // track H(x,t) for t in [0,1]
    while (t_s->status == PROCESSING && 1 - t0->real() > the_smallest_number) {
      if (1 - t0->real() <= s->end_zone_factor_ + the_smallest_number && !end_zone)
        end_zone = true; // TODO: see if this path coincides with any other path on entry to the end zone
      if (end_zone) {
          if (dt->real() > 1 - t0->real()) *dt = 1 - t0->real();
      } else if (dt->real() > 1 - s->end_zone_factor_ - t0->real()) *dt = 1 - s->end_zone_factor_ - t0->real();
      // PREDICTOR in: x0t0,dt
      //           out: dx
      // Runge Kutta
      /*  top-level code for Runge-Kutta-4
          dx1 := solveHxTimesDXequalsMinusHt(x0,t0);
          dx2 := solveHxTimesDXequalsMinusHt(x0+(1/2)*dx1*dt,t0+(1/2)*dt);
          dx3 := solveHxTimesDXequalsMinusHt(x0+(1/2)*dx2*dt,t0+(1/2)*dt);
          dx4 := solveHxTimesDXequalsMinusHt(x0+dx3*dt,t0+dt);
          (1/6)*dt*(dx1+2*dx2+2*dx3+dx4)
      */
      std::cerr << "\tEntered while loop" << std::endl;
      std::cerr << "t0 real" << t0->real() << std::endl;
      
      bool Axb_success = true;
      array_copy_NNNplus1(x0t0, xt);
      array_print_NNNplus1("x0t0", x0t0);
      array_print_NNNplus1("xt", xt);
      array_print_n("xt(params)", xt+NNNPLUS1,2*NPARAMS);
      
      // dx1
      evaluate_Hxt(xt, Hxt); // Outputs Hxt
      array_print_H("Hxt",Hxt);

      if (std::abs(Hxt[0] - complex(-.0142658,+.0314315)) <= dbgtol &&
          std::abs(Hxt[NNN*NNN-1] - complex(0,0)) <= dbgtol)
        std::cerr << "Hx from Hxt PASS\n";
      else
        std::cerr << "Hx from Hxt FAIL\n";
      
      if (std::abs(Hxt[NNN*NNN] - complex(.0901308,.0653859)) <= dbgtol &&
          std::abs(Hxt[NNN*NNN+NNN-1] - complex(-.0644011,-.470543)) <= dbgtol)
        std::cerr << "Ht from Hxt PASS \n";
      else {
        std::cerr << "Ht from Hxt FAIL\n";
        std::cerr << "\t" << Hxt[NNN*NNN] - complex(.0901308,.0653859)
          << Hxt[NNN*NNN+NNN-1] - complex(-.0644011,-.470543);
      }
      
      LHS = Hxt;
      array_print_n("LHS",LHS, NNN*NNN);
      RHS = Hxt + NNN2;
      array_negate_self(RHS);
      array_print("minus RHS",RHS);
      // was: solve_via_lapack_without_transposition(n, LHS, 1, RHS, dx1);
      Axb_success &= linear_eigen3(LHS,RHS,dx1);
      array_print("dx1",dx1);
      // TODO: once this is working, use eigen2 for LU partial pivots, faster.
      // this is QR full collumn pivoting, which is as fast as lapack LU
      
      if (std::abs(dx1[0] - complex(120.129,-61.1552)) <= dbgtol &&
          std::abs(dx1[NNN-1] - complex(25.3738,3.0885)) <= dbgtol)
        std::cerr << "dx1 PASS\n";
      else
        std::cerr << "dx1 FAIL\n";

      // dx2
      complex one_half_dt = one_half* *dt;
      array_multiply_scalar_to_self(dx1, one_half_dt);
      array_add_to_self(xt, dx1); // x0+.5dx1*dt
      xt[NNN] += one_half_dt;  // t0+.5dt
      //
      evaluate_Hxt(xt, Hxt);

      
      LHS = Hxt;
      RHS = Hxt + NNN2;
      array_negate_self(RHS);
      Axb_success &= linear_eigen3(LHS,RHS,dx2);
      std::cerr << "second eval ---------" << std::endl;
      array_print_NNNplus1("xt", xt);
      array_print_H("Hxt",Hxt);
      array_print("dx2", dx2);
      
      if (std::abs(dx2[0] - complex(414.419,-394.218)) <= dbgtol &&
          std::abs(dx2[NNN-1] - complex(-246.31,-120.222)) <= dbgtol)
        std::cerr << "dx2 PASS\n";
      else
        std::cerr << "dx2 FAIL\n";

      // dx3
      array_multiply_scalar_to_self(dx2, one_half_dt);
      array_copy(x0t0, xt);
      array_add_to_self(xt, dx2); // x0+.5dx2*dt
      // xt[n] += one_half*(*dt); // t0+.5dt (SAME)
      //
      evaluate_Hxt(xt, Hxt);
      LHS = Hxt;
      RHS = Hxt + NNN2;
      array_negate_self(RHS);
      Axb_success &= linear_eigen3(LHS,RHS,dx3);
      std::cerr << "third eval ---------" << std::endl;
      array_print_NNNplus1("xt", xt);
      array_print_H("Hxt",Hxt);
      array_print("dx3", dx3);
      if (std::abs(dx3[0] - complex(-11197.9,+14804.2)) <= 10*dbgtol &&
          std::abs(dx3[NNN-1] - complex(-17653.8,-4747.93)) <= dbgtol)
        std::cerr << "dx3 PASS\n";
      else {
        std::cerr << "dx3 FAIL\n";
        std::cerr << "\t\t" << dx3[0] - complex(-11197.9,+14804.2)
          << dx3[NNN-1] - complex(-17653.8,-4747.93) << std::endl;
      }

      // dx4
      array_multiply_scalar_to_self(dx3, *dt);
      array_copy_NNNplus1(x0t0, xt);
      array_add_to_self(xt, dx3); // x0+dx3*dt
      xt[NNN] += *dt;               // t0+dt
      //
      evaluate_Hxt(xt, Hxt);
      std::cerr << "fourth eval ---------" << std::endl;
      
      if (std::abs(Hxt[0] - complex(72802.2,-81551.9)) <= 10*dbgtol &&
          std::abs(Hxt[NNN*NNN-1] - complex(0,0)) <= dbgtol)
        std::cerr << "Hx from Hxt PASS\n";
      else
        std::cerr << "Hx from Hxt FAIL\n";
      
      if (std::abs(Hxt[NNN*NNN] - complex(-248323000+163093000)) <= 1 &&
          std::abs(Hxt[NNN*NNN+NNN-1] - complex(-.221521,-.47423)) <= dbgtol)
        std::cerr << "Ht from Hxt PASS \n";
      else
        std::cerr << "Ht from Hxt FAIL\n";
      LHS = Hxt;
      RHS = Hxt + NNN2;
      array_negate_self(RHS);
      Axb_success &= linear_eigen3(LHS,RHS,dx4);
      array_print_NNNplus1("xt", xt);
      array_print_H("Hxt",Hxt);
      array_print("dx4", dx4);
      
      if (std::abs(dx4[0] - complex(-559.896,+740.211)) <= dbgtol &&
          std::abs(dx4[NNN-1] - complex(-882.69,-237.396)) <= dbgtol)
        std::cerr << "dx4 PASS\n";
      else
        std::cerr << "dx4 FAIL\n";

      // "dx1" = .5*dx1*dt, "dx2" = .5*dx2*dt, "dx3" = dx3*dt
      // TODO: make this into a single function loop directly
      array_multiply_scalar_to_self(dx4, *dt);
      array_multiply_scalar_to_self(dx1, 2);
      array_multiply_scalar_to_self(dx2, 4);
      array_multiply_scalar_to_self(dx3, 2);
      array_add_to_self(dx4, dx1);
      array_add_to_self(dx4, dx2);
      array_add_to_self(dx4, dx3);
      array_multiply_scalar_to_self(dx4, 1./6.);
      array_copy(dx4, dx);

      // make prediction
      array_copy_NNNplus1(x0t0, x1t1);
      array_add_to_self_NNNplus1(x1t1, dxdt);
      
      // CORRECTOR
      unsigned n_corr_steps = 1;  // since its a do-while, it starts at 1
      bool is_successful;
      do {
        std::cerr << "\tCorrection step " << n_corr_steps << std::endl;
        evaluate_HxH(x1t1, HxH);
        array_print_NNNplus1("x1t1",x1t1);
        
        if (n_corr_steps = 1) {
          if (std::abs(HxH[0] - complex(26113200,11590100)) <= dbgtol &&
              std::abs(HxH[NNN*NNN-1] - complex(0,0)) <= dbgtol)
            std::cerr << "Hx from HxH PASS\n";
          else
            std::cerr << "Hx from HxH FAIL\n";
          
          if (std::abs(HxH[NNN*NNN] - complex(+1.741e11,-15110700000)) <= dbgtol &&
              std::abs(HxH[NNN*NNN+NNN-1] - complex(-3.71869e-13,+4.6629e-15)) <= dbgtol)
            std::cerr << "H from HxH PASS \n";
          else
            std::cerr << "H from HxH FAIL \n";
        }
        
        array_print_H("HxH",HxH);
        LHS = HxH;         // Hx
        RHS = HxH + NNN2;  // H
        array_negate_self(RHS); // TODO: put into eval function
        Axb_success &= linear_eigen3(LHS,RHS,dx);
        array_print_n("LHS corr",LHS,NNN2);
        array_print("RHS corr",RHS);
        array_print("dx",dx);
        if (n_corr_steps = 1) {
          if (std::abs(dx[0] - complex(-2206.68,-2378.66)) <= dbgtol &&
              std::abs(dx[NNN-1] - complex(-1731.72,+6593.05)) <= dbgtol)
            std::cerr << "dx from first correction PASS \n";
          else
            std::cerr << "dx from first correction FAIL \n";
        }
        array_add_to_self(x1t1, dx);
        is_successful = array_norm2(dx) < s->epsilon2_ * array_norm2(x1t1);
        // printf("c: |dx|^2 = %lf\n",
        // norm2_complex_array<ComplexField>(n,dx));
      } while (!is_successful && n_corr_steps++ < s->max_corr_steps_);
      
      if (!is_successful) { // predictor failure
        std::cerr << "\tPred failure" << std::endl;
        predictor_successes = 0;
        *dt *= s->dt_decrease_factor_;
        if (dt->real() < s->min_dt_) t_s->status = MIN_STEP_FAILED; // slight difference to SLP-imp.hpp:612
        std::cerr << "decreasing dt: " << dt;
      } else { // predictor success
        ++predictor_successes; count++;
        array_copy(x1t1, x0t0);
        if (predictor_successes >= s->num_successes_before_increase_) {
          predictor_successes = 0;
          *dt *= s->dt_increase_factor_;
          std::cerr << "increasing dt: " << dt;
        }
      }
      if (array_norm2(s_s) > s->infinity_threshold2_) // isn't it supposed to be t_s->x?
        t_s->status = INFINITY_FAILED;
      if (!Axb_success) t_s->status = SINGULAR;
      std::cerr << "\tAxb_success" << Axb_success << std::endl;
    } // while 
    // record the solution
    array_copy(s_s, t_s->x);
    t_s->t = t0->real();
    if (t_s->status == PROCESSING) t_s->status = REGULAR;
    // evaluate_HxH(x0t0, HxH);
    // cond_number_via_svd(HxH /*Hx*/, t_s->cond);
    t_s->num_steps = count;
  } // outer solution loop

  return 0;  // in the past, n_sols was returned, which now is NSOLS
}
  
#if 0
  /*
    printf(
        "epsilon2 = %e, t_step = %lf, dt_min_dbl = %lf, dt_increase_factor_dbl "
        "= %lf, dt_decrease_factor_dbl = %lf\n",
        epsilon2,
        t_step,
        dt_min_dbl,
        dt_increase_factor_dbl,
        dt_decrease_factor_dbl);
        */
    return 0;
}
                            //    if (M2_numericalAlgebraicGeometryTrace > 0)
                            //      {
                            //        if (sol_n % 50 == 0) printf("\n");
                            //        switch (t_s->status)
                            //          {
                            //            case REGULAR:
                            //              printf(".");
                            //              break;
                            //            case INFINITY_FAILED:
                            //              printf("I");
                            //              break;
                            //            case MIN_STEP_FAILED:
                            //              printf("M");
                            //              break;
                            //            case SINGULAR:
                            //              printf("S");
                            //              break;
                            //            default:
                            //              printf("-");
                            //          }
                            //        fflush(stdout);
                            //      }


//int
//linear_bundle
#endif