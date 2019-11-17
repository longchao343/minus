#ifndef debug_common_h_
#define debug_common_h_

#include <minus/minus.hxx>


#define Float double
typedef minus<chicago14a> M;
typedef minus_util<Float> util;
typedef minus_io<chicago14a> io;
typedef std::complex<Float> complex;
using namespace std::chrono;

static constexpr Float eps_ = 1e-3;

template <typename F>
inline void
print(const F *v, unsigned n, bool newline=false)
{
  for (unsigned i=0; i < n; ++i)
    std::cout << v[i] << ((newline)? "\n" : " ");
  std::cout << std::endl;
}

inline void
print(const Float *v, unsigned nrows, unsigned ncols)
{
  for (unsigned i=0; i < nrows; ++i)
    print(v + i*ncols, ncols);
}

// test if two vectors of the same size are equal up to tolerance
inline bool
same_vectors(const Float *v, const Float *w, unsigned n, Float tol=eps_)
{
  for (unsigned i=0; i < n; ++i)
    if (std::fabs(v[i] - w[i]) > eps_) {
      std::cout << "v: \n";
      print(v, n);
      std::cout << "w: \n";
      print(w, n);
      printf("offending element i, v[i], w[i] = [%d], %g, %g\n", i, v[i], w[i]);
      return false;
    }
  return true;
}

// test if two vectors of the same size are equal up to tolerance
inline bool
same_matrices(const Float *vp, const Float *wp, unsigned nrows, unsigned ncols, Float tol=eps_)
{
  const Float (*v)[ncols] = (Float (*)[ncols]) vp, (*w)[ncols] = (Float (*)[ncols]) wp;
  for (unsigned i=0; i < nrows; ++i)
    for (unsigned j=0; j < ncols; ++j)
      if (std::fabs(v[i][j] - w[i][j]) > eps_) {
        std::cout << "v: \n";
        print((Float *) v, nrows, ncols);
        std::cout << "w: \n";
        print((Float *) w, nrows, ncols); 
        printf("offending element [i][j] v[i][j], w[i,j] = [%d][%d], %g, %g\n", i, j, v[i][j], w[i][j]);
        return false;
      }
  return true;
}

// test if two vectors of the same size are equal up to tolerance
inline bool
same_matrices_up_to_row_scale(const Float *vp, const Float *wp, unsigned nrows, unsigned ncols, Float tol=eps_)
{
  const Float (*v)[ncols] = (Float (*)[ncols]) vp, (*w)[ncols] = (Float (*)[ncols]) wp;
  for (unsigned i=0; i < nrows; ++i) {
    bool match = true;
    for (unsigned j=0; j < ncols; ++j) 
      if (std::fabs(v[i][j] - w[i][j]) > eps_)  {
        match = false;
        break;
      }
    if (!match) {
      for (unsigned j=0; j < ncols; ++j) 
        if (std::fabs(v[i][j] + w[i][j]) > eps_ && std::fabs(v[i][j] - w[i][j]) > eps_)  {
            std::cout << "v: \n";
            print((Float *) v, nrows, ncols);
            std::cout << "w: \n";
            print((Float *) w, nrows, ncols); 
            printf("offending element [i][j] v[i][j], w[i][j] = [%d][%d], %g, %g\n", i, j, v[i][j], w[i][j]);
            return false;
        }
    }
  }
  return true;
}
#if 0
template <unsigned N, typename F /* no complex number, only float types */>
struct minus_array_util { // Not speed critical -----------------------------------------
  static inline void 
  multiply_scalar_to_self(F *__restrict__ a, F b)
  {
    for (unsigned i = 0; i < N; ++i, ++a) *a = *a * b;
  }

  static inline void
  negate_self(F * __restrict__ a)
  {
    for (unsigned i = 0; i < N; ++i, ++a) *a = -*a;
  }

  static inline void 
  multiply_self(F * __restrict__ a, const F * __restrict__ b)
  {
    for (unsigned int i=0; i < N; ++i,++a,++b) *a *= *b;
  }

  static inline void 
  add_to_self(F * __restrict__ a, const F * __restrict__ b)
  {
    for (unsigned int i=0; i < N; ++i,++a,++b) *a += *b;
  }

  static inline void 
  add_scalar_to_self(F * __restrict__ a, F b)
  {
    for (unsigned int i=0; i < N; ++i,++a) *a += b;
  }

  static inline void 
  copy(const F * __restrict__ a, F * __restrict__ b)
  {
    memcpy(b, a, N*sizeof(F));
  }

  static inline F
  norm2(const F *__restrict__ a)
  {
    F val = 0;
    F const* __restrict__ end = a+N;
    while (a != end) val += std::abs(*a++);
    return val;
  }
};

typedef minus_array_util<Float> mu; 
#endif

#endif // debug_common_h_