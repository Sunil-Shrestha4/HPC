#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <time.h>

/***********************************************************************
*******
  Demonstrating  how to crack an encrypted password using a simple
  "brute force" algorithm which works on passwords that consist  of 3
uppercase letters and a 2 digit integer. 

   To compile:
    cc -o CrackAZ99-With-Data CrackAZ99-With-Data.c -lcrypt

  To run the program:

    ./CrackAZ99-With-Data > results.txt

  
************************************************************************
******/
int n_passwords = 4;

char *encrypted_passwords[] = {

"$6$KB$lmT/FXt3yIcz6uxQYo4UqnlIffCELx/.d1bBsOz7vaV2UyF4jxhkXkUZiyhpv6mARbOU5qcNcdVYTimGGJXza/",
"$6$KB$UdTiX2663WE1TcmJ/DcQJidhZ02buovLqtICPdESYBWx04j/WRxlxuAfO4mK5Z2qzLcgbTVd0.nL2kVYs4Uhc0",
"$6$KB$uCjck9edUx5X85JaXg2MKW.aEZbO5Yd4eofiHKzixWj6rNIOpdvoUiBFSrtrethPwOXbsFPSYOnaPhqthvjPt.",
"$6$KB$bIdxJvGlqEa2s/9eS1R8Tg9V/S1DENMdUenibmdz8IkYJs7lRqneXPxMtgguf5/Eeh9MUUSsqUK6hy50/royb0"
};

/**
 Required by lack of standard function in C.   
*/

void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}

/**
 This function can crack the kind of password explained above. All combinations
 that are tried are displayed and when the password is found, #, is put at the start of the line. Note that one of the most time consuming operations that
 it performs is the output of intermediate results, so performance experiments for this kind of program should not include this. i.e. comment out the
printfs.
*/

void crack(char *salt_and_encrypted){
  int x, y, s, z;     // Loop counters
  char salt[7];    
  char plain[7];   // The combination of letters currently being checked
  char *enc;       // Pointer to the encrypted password
  int count = 0;   // The number of combinations explored so far

  substr(salt, salt_and_encrypted, 0, 6);

  for(x='A'; x<='Z'; x++){
    for(y='A'; y<='Z'; y++){
      for(s='A';s<='Z';s++){
        for(z=0; z<=99; z++){
              
        sprintf(plain, "%c%c%c%02d", x, y, s, z);
        enc = (char *) crypt(plain, salt);
        count++;
        if(strcmp(salt_and_encrypted, enc) == 0){
          printf("#%-8d%s %s\n", count, plain, enc);
        } else {
          printf(" %-8d%s %s\n", count, plain, enc);
        }
       }
      }
      
    }
  }
  printf("%d solutions explored\n", count);
}


//Calculating time

int time_difference(struct timespec *start, struct timespec *finish, long long int *difference)
 {
	  long long int ds =  finish->tv_sec - start->tv_sec; 
	  long long int dn =  finish->tv_nsec - start->tv_nsec; 

	  if(dn < 0 ) {
	    ds--;
	    dn += 1000000000; 
  } 
	  *difference = ds * 1000000000 + dn;
	  return !(*difference > 0);
}
int main(int argc, char *argv[])
{
  	int i;
	struct timespec start, finish;   
  	long long int time_elapsed;

  	clock_gettime(CLOCK_MONOTONIC, &start);

  	for(i=0;i<n_passwords;i<i++) 
	{
    		crack(encrypted_passwords[i]);
  	}
	clock_gettime(CLOCK_MONOTONIC, &finish);
	  time_difference(&start, &finish, &time_elapsed);
	  printf("Time elapsed was %lldns or %0.9lfs\n", time_elapsed,
		                                 (time_elapsed/1.0e9)); 
  return 0;
}
