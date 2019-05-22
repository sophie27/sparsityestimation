#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;
#define MAX_ITERATIONS 1e4 /* Maximum number of iterations per node */
int N;   /* Size of the vector to be sampled */
int M;   /* Number of measurements  */
double sigma, sigma2, k_hat, k_old, k_hat_lopes, median, SNRdB, thresh;
double val, lambda, gam, sum_aux;
int h,i,j, k,t, run, counter;
clock_t startTime, endTime;


//( (double) rand() / (RAND_MAX) )   generates a uniformly distributed random value in [0,1]
double Gaussian_Noise(double std) {
    return std*(sqrt(-2*log( ((double) rand() / (RAND_MAX))  )))*cos(2*pi* ((double) rand() / (RAND_MAX))  );
}

double absol(double n)
{
	if (n<0)
		n=-n;
	return n;
}

int main (int argc, char *argv[]) {
	if (argc < 5) {
		cerr << "Usage: " << argv[0] << "  <run> <signal sparsity k> <# of measurements M>  <dB>" << endl;
		return EXIT_FAILURE;
	}
	run=atoi(argv[1]);
	k = atoi (argv[2]);
	M = atoi (argv[3]);
        sigma=1;
	startTime = clock();
	N=20*k;
	srand(run*N);
	SNRdB=atof(argv[4]); //SNR dB (assumed sigma=1);
	lambda=sqrt( double( pow(10,double(SNRdB)/10 ))/double(k) );
	//cerr << lambda << endl;
	//gam=SNRdB/double(100*k);
	gam=1./double(3*k); 
	VectorXd y, x, y_lopes1, y_lopes2, noise;
	x.resize(k);
	y.resize(M);
	noise.resize(M);
	y_lopes1.resize(M/2);
	y_lopes2.resize((M+1)/2);
	for (h = 0; h < k; h++) {
		val= Gaussian_Noise(1);
		//x(h)= ( ( val > lambda) ? val : (  (val < -lambda) ? val :(  ( val > 0) ? lambda : -lambda))  );
		x(h)= ( ( val > 0) ? lambda : -lambda);
	}
	
	for (i = 0; i < M; i++) {
		//y(i)=Gaussian_Noise(sigma);
		noise(i)=Gaussian_Noise(sigma);
		y(i)=noise(i);
		( i<M/2 ) ? y_lopes1(i)=y(i) : y_lopes2(i-M/2)=y(i)  ;
		for (h = 0; h < k; h++) {
			if (unif() < gam ) {
				//val=Gaussian_Noise(1);
				y(i)=y(i)+Gaussian_Noise(1/sqrt(gam))*x(h);

			}
		}
	}
	/** 3. INITIALIZATION **/
	k_hat=N/2;
	k_old=0;
	VectorXd a(2);
	a(0) = 0.1;
	a(1)=k_hat+0.1;
	VectorXd p(2);
	MatrixXd pi(M,2);

	for (i = 0; i < M; i++) {
		//pi(i,0)= p(0)*f(y(i),0,a(0)) / ( p(0)*f(y(i),0,a(0)) + p(1)*f(y(i),0,a(1)) );
		pi(i,0)=0.5;
		pi(i,1)=1-pi(i,0);
	}

	/** 4. MAIN CYCLE **/
	for (t = 1; t <= MAX_ITERATIONS; t++)    {
		sum_aux=0;
		for (i = 0; i < M; i++) {
			sum_aux=sum_aux+pi(i,0);
		}
		k_hat=log(sum_aux/M) / log( 1-gam );

		p(0)=pow(1-gam,k_hat);
		p(1)=1-p(0); //this is the p in the algorithm of the article!

        	for (i = 0; i < M; i++) {
			pi(i,0)= p(0)*f(y(i),0,a(0)) / ( p(0)*f(y(i),0,a(0)) + p(1)*f(y(i),0,a(1)) );
			pi(i,1)=1-pi(i,0);
		}

		sum_aux=0;
		for (i = 0; i < M; i++) {
			sum_aux=sum_aux+pi(i,0);
		}
		a(0)=0;
		a(1)=0;
		for (i = 0; i < M; i++) {
			a(0)=a(0)+pi(i,0)*y(i)*y(i);
			a(1)=a(1)+pi(i,1)*y(i)*y(i);
		}
		a(0)=a(0)/sum_aux;
		a(1)=a(1)/(M-sum_aux);
		
		if (absol(k_hat-k_old)<1e-2) {
	    		break;
		}
		k_old=k_hat;
	}  /** end t **/
	k_hat=round(k_hat);

	/*****************************************/


	for (j = 0; j < M/2; j++) {
		for (h = 0; h < k; h++) {
		        y_lopes1(j)=y_lopes1(j)+tan(3.14159265358979323846*(unif()-0.5) )*x(h);
                        y_lopes2(j)=y_lopes2(j)+Gaussian_Noise(1)*x(h);  
		}
	}
	

	y_lopes1= y_lopes1.array().abs();
	sort(y_lopes1.data(), y_lopes1.data()+y_lopes1.size() );
	median= ( (M/2)%2 == 0) ? 0.5*(y_lopes1(M/4-1)+y_lopes1(M/4)) : y_lopes1(M/4) ;
	
	y_lopes2= y_lopes2.array().abs2();
	
	k_hat_lopes=median*median/  y_lopes2.mean()  ;
	k_hat_lopes=round(k_hat_lopes);
	
	/*****************************************/
	
// 	y=y-noise;
// 	y=y.array().abs2();
	//cerr <<" " << y <<" power "  <<y.mean() << endl;
	cout << absol(k_hat-k)/k  << "   " <<t << "  " <<absol(k_hat_lopes-k)/k << endl;
	//fX.close();
	endTime = clock();
	//cerr << " ci ho messo "<< (endTime-startTime)/CLOCKS_PER_SEC <<"  secondi" << endl;
	return EXIT_SUCCESS; 
}
