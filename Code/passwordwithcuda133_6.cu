#include <stdio.h>
#include <cuda_runtime_api.h>
#include <time.h>
/****************************************************************************
  This program gives an example of a poor way to implement a password cracker
  in CUDA C. It is poor because it acheives this with just one thread, which
  is obviously not good given the scale of parallelism available to CUDA
  programs.
  
  The intentions of this program are:
    1) Demonstrate the use of __device__ and __global__ functions
    2) Enable a simulation of password cracking in the absence of library 
       with equivalent functionality to libcrypt. The password to be found
       is hardcoded into a function called is_a_match.   

  Compile and run with:
    nvcc -o passwordwithcuda133_6 passwordwithcuda133_6.cu
    ./passwordwithcuda133_6
   
  Dr Kevan Buckley, University of Wolverhampton, 2018
*****************************************************************************/

/****************************************************************************
  This function returns 1 if the attempt at cracking the password is 
  identical to the plain text password string stored in the program. 
  Otherwise,it returns 0.
*****************************************************************************/

__device__ int is_a_match(char *attempt) {
	char plain_password1[] = "SU1234";
	char plain_password2[] = "NI5678";
	char plain_password3[] = "LS9101";
	char plain_password4[] = "TA1213";


	char *ab = attempt;
	char *bc = attempt;
	char *cd = attempt;
	char *de = attempt;
	char *ab1 = plain_password1;
	char *bc2 = plain_password2;
	char *cd3 = plain_password3;
	char *de4 = plain_password4;

	while(*ab == *ab1) { 
		if(*ab == '\0') 
		{
			printf("Password: %s\n",plain_password1);
			break;
		}

		ab++;
		ab1++;
	}
	
	while(*bc == *bc2) { 
		if(*bc == '\0') 
		{
			printf("Password: %s\n",plain_password2);
			break;
		}

		bc++;
		bc2++;
	}

	while(*cd == *cd3) { 
		if(*cd == '\0') 
		{
			printf("Password: %s\n",plain_password3);
			break;
		}

		cd++;
		cd3++;
	}

	while(*de == *de4) { 
		if(*de == '\0') 
		{
			printf("Password: %s\n",plain_password4);
			return 1;
		}

		de++;
		de4++;
	}
	return 0;

}

__global__ void  kernel() {
	char i1,i2,i3,i4;

	char password[7];
	password[6] = '\0';

	int i = blockIdx.x+65;
	int j = threadIdx.x+65;
	char firstMatch = i; 
	char secondMatch = j; 

	password[0] = firstMatch;
	password[1] = secondMatch;
	for(i1='0'; i1<='9'; i1++){
		for(i2='0'; i2<='9'; i2++){
			for(i3='0'; i3<='9'; i3++){
				for(i4='0'; i4<='9'; i4++){
					password[2] = i1;
					password[3] = i2;
					password[4] = i3;
					password[5] = i4; 
					if(is_a_match(password)) {
					} 
					else {
	     			//printf("tried: %s\n", password);		  
					}
				}
			}
		}
	}
}

// Calculate the difference between two times. Returns zero on
// success and the time difference through an argument. It will 
// be unsuccessful if the start time is after the end time.

int time_difference(struct timespec *start, 
                    struct timespec *finish, 
                    long long int *difference) {
  long long int d_sec =  finish->tv_sec - start->tv_sec; 
  long long int d_nsec =  finish->tv_nsec - start->tv_nsec; 

  if(d_nsec < 0 ) {
    d_sec--;
    d_nsec += 1000000000; 
  } 
  *difference = d_sec * 1000000000 + d_nsec;
  return !(*difference > 0);
}


int main() {

	struct  timespec start, finish;
	long long int time_elapsed;
	clock_gettime(CLOCK_MONOTONIC, &start);

	kernel <<<26,26>>>();
	cudaThreadSynchronize();

	clock_gettime(CLOCK_MONOTONIC, &finish);
	time_difference(&start, &finish, &time_elapsed);
	printf("Time elapsed was %lldns or %0.9lfs\n", time_elapsed, (time_elapsed/1.0e9)); 

	return 0;
}


