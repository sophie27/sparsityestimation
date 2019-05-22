#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <iomanip>
using namespace std;


#define pi 3.14159265
int M;   /* Number of measurements  */
double k_hat, val, lambda,  gam,x,y;
int h,i,j, k, norm0, run;

//( (double) rand() / (RAND_MAX) )   generates a uniformly distributed random value in [0,1]
double Gaussian_Noise(double std) {
    return std*(sqrt(-2*log( ((double) rand() / (RAND_MAX))  )))*cos(2*pi* ((double) rand() / (RAND_MAX))  );
}

int main (int argc, char *argv[]) {
	run=atoi(argv[1]);
	M=4000;

	for (k = 500; k <= 12000 ; k=k+500) {
		srand(run*k);
		//gam = 0.5*log( double(k)/log(k) )/k; // REGIME a)
		gam = 1./k; // REGIME b)
		//gam= pow(log(k)/k, 1./3 ) /k; // REGIME c)
        //psi=gam*k;

		double x[k];
		for (h = 0; h < k; h++) {
			x[h]=Gaussian_Noise(1);
		}
		norm0=0;
		for (i = 1; i < M+1; i++) {
			y=0;
			for (h = 0; h < k; h++) {
				if ( ((double) rand() / (RAND_MAX)) < gam ) {
				y=y+Gaussian_Noise(1./sqrt(gam))*x[h];
				}
			}
			norm0=norm0+(y != 0 );
			if ((i % 500) == 0) {
				k_hat = log( 1-double(norm0)/double(i)) / log( 1-gam );
				cout <<  ((abs(k_hat-k)/k )< 5e-2) << "  " <<  ((abs(k_hat-k)/k )< 1e-2) <<endl;
			}
	}
	}
	return EXIT_SUCCESS; 
}
