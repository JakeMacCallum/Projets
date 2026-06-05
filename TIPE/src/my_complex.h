#include <math.h>


typedef struct {
    double re;
    double im;
} complex_double;

//get a zero complex number
complex_double zero();

//get a complex i
complex_double complex_1();

//get a complex 1
complex_double real_1();

//get real part of x
double re(complex_double x);

//get imaginary part of x
double im(complex_double x);

//get the conjugate of x
complex_double conjugate(complex_double x);

//multiply two complex numbers together
complex_double multiply_complex(complex_double x, complex_double y);

//add two complex numbers together
complex_double add_complex(complex_double x, complex_double y);

//multiply a complex number by a real number
complex_double multiply_real(complex_double x, double y);

//add a complex number with a real number
complex_double add_real(complex_double x, double y);

//x^n fast, even faster with -1
double power(double x, int n);

//i^n fast
complex_double i_power(int n);

//get exp(i*m*theta) from cartesian x and y
complex_double e_i_m_theta(double x, double y, int m);

//calculate binomial coefficient 
int binomial(int n, int k);
