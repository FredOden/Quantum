/*
 * try to find wether 1+2+3+......+n tends to -/12
 * and why this limit is used for quantum physics
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>


long gSeed;
const double invFastMax = 1.0/(unsigned int)0xFFFF;

inline double fastrand() {
		gSeed = (2140133*gSeed +2531011);
		return (double)((gSeed >> 16) & 0xFFFF)*invFastMax -.5;
}
extern inline double fastrand();

double precision;
double invRAND_MAX = ((long double)1/RAND_MAX);


#define DELTA_RAND(i) (long double)((((long double)(rand())*invRAND_MAX) -.5)*precision)
#define DELTA_FASTRAND(i) fastrand()*precision
#define DELTA_HALF(i) ((fastrand()>0)?.5:-.5)*precision
#define DELTA DELTA_FASTRAND

/*
   long double u1(int i) {
   return DELTA(i) + (long double)(((i + 1)%2 == 0)?1.0:-1.0);
   }
 */
inline long double doSigma(int id, long double(*u)(unsigned long long), unsigned long long from, unsigned long long to) {
		register long double sigma = 0.0; //u(from);
		register unsigned long long i;
		for(i = from -1; i < to;) {
			 	sigma +=  ((long double)++i)*(1 + DELTA(i));
				//sigma += u(++i);
		}
		return sigma;
}
extern inline long double doSigma(int id, long double(*u)(unsigned long long), unsigned long long from, unsigned long long to);


inline long double u2(unsigned long long i) {
		return ((long double)i)*(1 + DELTA(i));
}
extern inline long double u2(unsigned long long i);


inline double timeInMilliseconds() {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		double ret =  tv.tv_sec + (double)tv.tv_usec/1000000; // + (tv.tv_usec/1000));
		return ret;
}
extern double timeInMilliseconds();

int main(int argc, char **argv) {
		if (argc != 6) {
				fprintf(stderr, "Usage %s from to randomPrecision loop nbThreads\n", argv[0]);
				return -1;
		}

		const unsigned long long FROM = atoi(argv[1]);
		const unsigned long long TO = atoi(argv[2]);
		precision = 1/strtold(argv[3], NULL);
		const int LOOP = atoi(argv[4]);
		const int NB_THREADS = atoi(argv[5]);

		omp_set_num_threads(NB_THREADS);
		srand(time(0));
		gSeed = time(0);

		long double mSigma = 0.0;

		const long double(*u)(unsigned long long) = u2;

		int nbThreads;
		nbThreads = omp_get_max_threads();
		double start = timeInMilliseconds();
		double begin[LOOP];
		double sigma;
		//for (int i = 0; i < LOOP; i++)
#pragma omp parallel for private(sigma) shared(mSigma)
		for (int i = 0; i < LOOP; i++) {
						begin[i] = timeInMilliseconds();
						printf("%f::<%d:%d>::doSygma(u,%llu,%llu) START\n", timeInMilliseconds() - begin[i], omp_get_thread_num(), i, FROM, TO);
						sigma = doSigma(i, u, FROM, TO);
						printf("%f::<%d:%d>::doSygma(u,%llu,%llu) => %Lf\n", timeInMilliseconds() - begin[i], omp_get_thread_num(), i, FROM, TO, sigma);
#pragma atomic
						mSigma += sigma;
		}

		double top = timeInMilliseconds() - start;

		printf("%f::%f(s)::mSygma(u) => %Lf on %d threads\n", top, top/LOOP, mSigma/LOOP, LOOP);

}
