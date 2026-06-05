#include "my_complex.h"
#include <math.h>
#include <stdio.h>

complex_double zero() {
    return (complex_double){0.0, 0.0};
}

complex_double complex_1() {
    return (complex_double){0.0, 1.0};
}

complex_double real_1() {
    return (complex_double){1.0, 0.0};
}

double re(complex_double x) {
    return x.re;
}

double im(complex_double x) {
    return x.im;
}

complex_double conjugate(complex_double x) {
    return (complex_double){
        .re = x.re,
        .im = - x.im
    };
}

complex_double multiply_complex(complex_double x, complex_double y) {
    return (complex_double){
        .re = x.re * y.re - x.im * y.im,
        .im = x.re * y.im + x.im * y.re
    };
}

complex_double add_complex(complex_double x, complex_double y) {
    return (complex_double){
        .re = x.re + y.re,
        .im = x.im + y.im
    };
}

complex_double multiply_real(complex_double x, double y) {
    return (complex_double){
        .re = y * x.re,
        .im = y * x.im,
    };
}

complex_double add_real(complex_double x, double y) {
    return (complex_double){
        .re = y + x.re,
        .im = x.im,
    };
}

double power(double x, int n) {
    if (x < 0) {
        if (x == -1) {
            return (2*(n%2==0) - 1);
        }
        else {
            return (2*(n%2==0) - 1) * power(-x, n);
        }
    }
    if (n == 0) return 1;
    else {
        double y = power(x, n/2);
        if (n % 2 == 0) return y * y;
        else return y * y * x;
    }
}

complex_double i_power(int n) {
    complex_double x = zero();
    if (n % 4 == 0) x.re = 1;
    else if (n % 4 == 1) x.im = 1;
    else if (n % 4 == 2) x.re = -1;
    else x.im = -1;
    return x; 
}

int sgn(double x) {
    return (x >= 0) - (x < 0);
}

complex_double e_i_m_theta(double x, double y, int m) {
    if (x == 0 && y == 0) return (complex_double){0.00, 0.00};
    complex_double z = zero();
    double phi = sgn(y)*acos(x / sqrt(x*x+y*y));
    z.re = cos(m * phi);
    z.im = sin(m * phi);
    return z;
}

complex_double e_i_m_theta_2(double x, double y, int m) {
    complex_double z = zero();
    for (int k = 0 ; k < m + 1 ; ++k) {
        if (k%2 == 0) {
            z.re += power(-1,k/2) * binomial(m,k) * power(x,m-k) * power(y,k);
        }
        else {
            z.im += power(-1,(k-1)/2) * binomial(m,k) * power(x,m-k) * power(y,k);
        }
    }
    return multiply_real(z, 1/power(sqrt(x*x+y*y),m));
}

int binomial(int n, int k) {
    if (k == 0) return 1;
    else if (k > n/2) return binomial(n, n-k);
    else return n * binomial(n-1, k-1) / k;
}
