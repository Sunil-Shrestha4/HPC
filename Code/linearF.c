#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

/******************************************************************************
 * This program takes an initial estimate of m and c and finds the associated 
 * rms error. It is then as a base to generate and evaluate 8 new estimates, 
 * which are steps in different directions in m-c space. The best estimate is 
 * then used as the base for another iteration of "generate and evaluate". This 
 * continues until none of the new estimates are better than the base. This is
 * a gradient search for a minimum in mc-space.
 * 
 * To compile:
 *   cc -o linearF linearF.c -lm -pthread
 * 
 * To run:
 *   ./linearF
 * 
 * Dr Kevan Buckley, University of Wolverhampton, 2018
 *****************************************************************************/
int i;
  double bm = 1.3;
  double bc = 10;
  double be;
  double dm[8];
  double dc[8];
  double e[8];
  double step = 0.01;
  double best_error = 999999999;
  int best_error_i;
  int minimum_found = 0;
  double om[] = {0,1,1, 1, 0,-1,-1,-1};
  double oc[] = {1,1,0,-1,-1,-1, 0, 1};

typedef struct point_t {
  double x;
  double y;
} point_t;

int n_data = 1000;
point_t data[];

double residual_error(double x, double y, double m, double c) {
  double e = (m * x) + c - y;
  return e * e;
}

double rms_error(double m, double c) {
  int i;
  double mean;
  double error_sum = 0;
  
  for(i=0; i<n_data; i++) {
    error_sum += residual_error(data[i].x, data[i].y, m, c);  
  }
  
  mean = error_sum / n_data;
  
  return sqrt(mean);
}
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

void *linear_regression_thread(void *args){

  int *a = args;
  int i = *a;

  dm[i] = bm +(om[i] * step);
      dc[i] = bc + (oc[i] * step);
  e[i] = rms_error(dm[i], dc[i]);
  if(e[i] < best_error) {
  best_error = e[i];
  best_error_i = i;
  pthread_exit(NULL);
  }
}  

int main() {
   struct timespec start, finish;   
  long long int time_elapsed;
  clock_gettime(CLOCK_MONOTONIC, &start);

  int i;
  pthread_t p_threads[8];
  
  be = rms_error(bm, bc);

  while(!minimum_found) {
    for(i=0;i<8;i++) {
      pthread_create(&p_threads[i], NULL, (void*)linear_regression_thread, &i);
      pthread_join(p_threads[i], NULL); 
    }

     //printf("best m,c is %lf,%lf with error %lf in direction %d\n", 
       //dm[best_error_i], dc[best_error_i], best_error, best_error_i);
    if(best_error < be) {
      be = best_error;
      bm = dm[best_error_i];
      bc = dc[best_error_i];
    } else {
      minimum_found = 1;
    }
  }
  printf("minimum m,c is %lf,%lf with error %lf\n", bm, bc, be);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  time_difference(&start, &finish, &time_elapsed);
  printf("Time elapsed was %lldns or %0.9lfs\n", time_elapsed, 
         (time_elapsed/1.0e9)); 
return 0;
}

point_t data[] = {

  {82.75,102.99},{65.49,98.18},{78.56,105.35},{68.67,85.29},
  {65.81,112.85},{82.84,113.68},{65.55,98.46},{71.98,114.52},
  {73.16,126.43},{76.40,134.02},{76.82,97.29},{47.47,79.78},
  {41.13,78.60},{64.13,97.66},{34.81,71.87},{67.40,124.95},
  {78.92,119.15},{70.50,120.57},{68.55,101.00},{36.91,75.79},
  {48.94,91.26},{57.95,105.08},{66.28,93.09},{ 6.10,26.68},
  {49.17,86.16},{42.11,71.14},{59.66,95.41},{87.00,120.27},
  {19.21,52.28},{26.03,39.19},{78.42,102.92},{39.60,62.76},
  {31.86,67.82},{98.79,134.45},{41.63,79.01},{47.04,79.06},
  {87.79,118.72},{ 8.41,35.11},{12.18,26.31},{66.63,96.34},
  {85.67,126.04},{95.24,124.55},{91.59,133.08},{18.71,35.87},
  {50.54,88.79},{55.59,80.88},{57.45,80.08},{69.68,107.06},
  {68.48,98.14},{42.52,68.34},{60.75,82.69},{63.82,95.62},
  {83.82,117.19},{ 7.19,17.98},{ 0.55,20.03},{29.40,42.51},
  {35.42,66.35},{75.00,117.98},{82.07,115.32},{13.45,33.99},
  {55.14,97.79},{33.86,51.12},{75.48,106.36},{33.96,60.19},
  {53.84,98.26},{46.84,76.33},{64.56,111.80},{92.83,130.92},
  {11.02,24.55},{14.14,24.08},{34.19,69.54},{62.61,109.86},
  {92.29,125.64},{ 1.14,23.05},{55.58,83.02},{19.23,54.48},
  {43.58,69.39},{45.14,74.43},{81.14,114.66},{82.99,118.22},
  {66.82,94.85},{97.53,133.25},{ 7.24, 9.11},{15.88,33.92},
  {53.36,87.02},{42.76,77.23},{92.48,119.31},{68.85,103.81},
  {91.48,151.27},{39.59,80.63},{15.13,57.50},{27.61,54.82},
  {53.97,89.81},{15.40,51.54},{73.72,111.38},{64.97,100.00},
  {50.37,86.82},{ 5.64,34.85},{72.08,117.17},{69.41,97.42},
  {59.60,74.65},{43.19,67.89},{ 7.66,30.58},{46.36,70.09},
  {95.11,127.80},{17.26,32.74},{ 7.08,32.31},{77.80,108.66},
  {61.24,110.63},{ 1.66, 9.23},{ 5.52,26.67},{76.55,112.34},
  {39.08,57.20},{85.69,125.46},{ 2.24, 5.80},{11.60,15.73},
  {46.33,78.07},{55.99,101.98},{18.18,52.18},{77.58,126.60},
  {15.98,56.85},{38.76,65.84},{30.17,50.44},{41.82,81.73},
  { 1.95,21.59},{90.31,123.68},{35.47,54.71},{44.59,93.65},
  {25.49,43.71},{ 9.83,24.65},{31.18,52.48},{83.18,126.20},
  {17.74,53.18},{97.83,122.81},{55.36,91.53},{22.74,40.82},
  {27.93,47.07},{81.69,118.05},{23.42,52.64},{16.32,63.78},
  { 5.78,36.54},{72.98,89.76},{84.67,107.40},{45.96,79.32},
  { 1.27,19.92},{18.92,40.49},{29.68,58.82},{59.24,74.82},
  {92.61,135.87},{16.72,25.27},{23.61,66.84},{29.50,55.91},
  { 2.87,33.00},{97.05,135.91},{86.20,115.59},{17.93,50.76},
  {72.71,116.81},{86.57,104.69},{26.62,62.47},{56.14,108.56},
  {65.06,114.35},{85.39,121.43},{50.94,63.59},{18.78,43.67},
  {80.15,114.23},{53.90,75.31},{12.39,29.39},{85.21,127.79},
  {14.88,46.83},{18.51,43.35},{45.74,83.98},{ 0.35,17.38},
  {52.10,89.27},{52.42,85.78},{61.40,101.05},{69.11,113.63},
  { 3.04,28.84},{48.39,90.64},{74.91,102.50},{72.62,90.44},
  {77.33,112.74},{76.72,108.72},{ 2.41,35.62},{15.70,31.81},
  {40.00,61.68},{78.27,107.97},{24.81,48.71},{28.06,49.45},
  {64.60,86.09},{53.78,91.20},{61.22,94.24},{48.19,93.42},
  {52.73,82.14},{66.47,97.38},{80.60,111.94},{24.51,60.94},
  {30.75,48.97},{54.78,85.37},{93.04,132.50},{97.82,146.30},
  {84.91,110.92},{89.46,118.87},{80.13,106.04},{87.76,109.80},
  { 6.40,40.57},{91.82,143.91},{66.91,93.58},{33.66,55.38},
  {95.29,127.14},{31.28,54.01},{53.42,94.35},{22.10,39.01},
  {36.65,60.30},{29.88,65.04},{50.30,85.98},{24.08,36.90},
  {65.82,112.54},{85.64,122.04},{77.64,105.16},{23.94,45.68},
  {52.39,76.04},{22.15,53.97},{79.57,120.30},{95.43,136.74},
  {14.39,48.47},{75.22,111.67},{10.62,25.75},{39.12,55.13},
  {47.23,68.62},{68.64,97.47},{24.34,67.88},{73.02,113.79},
  {93.37,133.01},{65.53,89.76},{12.01,43.27},{36.27,61.91},
  {96.04,142.04},{54.88,94.29},{54.53,79.12},{ 6.39,20.93},
  {14.10,31.46},{74.48,110.53},{63.15,108.52},{86.43,130.80},
  {68.92,95.13},{93.66,129.94},{22.34,27.48},{ 7.95,28.43},
  {89.16,113.06},{54.45,89.24},{30.19,39.65},{47.21,81.98},
  {23.26,72.13},{ 5.23,18.61},{98.30,126.22},{76.67,121.33},
  {15.19,36.75},{32.28,53.94},{54.94,93.75},{29.80,68.23},
  {13.34,31.69},{25.80,49.03},{53.92,86.03},{59.11,87.03},
  {66.36,98.96},{41.04,58.88},{53.57,75.94},{66.66,111.10},
  {77.79,106.68},{59.73,96.21},{35.25,57.42},{16.90,49.89},
  {10.91,44.14},{ 9.64,38.78},{16.21,46.00},{ 9.67,17.50},
  {69.42,102.25},{13.27,41.15},{ 5.33,21.43},{ 8.75,34.65},
  {11.58,31.34},{39.03,79.02},{40.36,71.67},{51.64,84.18},
  { 8.57,48.42},{61.06,90.15},{34.60,62.81},{88.37,146.39},
  {35.34,58.07},{90.26,139.45},{67.17,105.11},{46.90,83.26},
  {13.17,38.91},{12.60,38.97},{94.70,138.47},{ 4.83,17.95},
  {41.33,71.29},{72.06,108.44},{29.23,64.11},{80.96,119.78},
  {50.54,88.19},{73.60,112.55},{15.55,24.35},{71.89,119.62},
  {22.45,52.55},{23.78,56.89},{67.82,101.94},{45.71,80.62},
  {46.84,77.39},{72.09,108.04},{96.37,137.79},{21.86,44.94},
  {90.04,121.40},{50.13,75.41},{31.81,72.94},{23.31,73.25},
  {27.65,47.31},{77.28,110.69},{56.96,99.42},{53.82,80.22},
  {26.66,41.22},{24.19,47.52},{59.29,96.29},{27.29,53.19},
  {48.35,92.38},{47.26,67.62},{24.53,40.51},{25.60,58.04},
  {49.16,76.45},{34.91,44.95},{43.46,48.32},{44.54,57.42},
  {86.03,133.00},{69.17,112.61},{32.05,55.72},{53.62,86.49},
  {74.95,108.81},{58.84,86.60},{80.26,110.16},{95.48,130.99},
  {91.88,112.19},{37.76,64.48},{60.10,81.74},{79.85,120.60},
  {26.67,45.29},{ 2.08,20.18},{ 5.88,43.00},{18.58,56.21},
  {26.33,61.54},{26.83,60.58},{91.43,112.97},{59.40,87.65},
  {56.91,87.60},{28.15,40.73},{52.30,88.49},{20.06,54.95},
  {76.87,115.44},{29.19,56.74},{78.26,114.69},{32.37,77.12},
  {91.77,131.29},{58.90,95.06},{ 5.20,21.96},{85.77,105.24},
  {82.17,112.11},{90.34,133.28},{38.02,60.51},{18.05,42.97},
  {93.14,141.01},{93.52,135.73},{59.07,81.50},{61.35,113.01},
  {47.01,72.83},{99.82,141.53},{ 7.86,35.35},{20.26,50.45},
  {74.17,94.34},{33.32,42.39},{16.65,19.59},{86.71,133.97},
  {28.44,78.55},{24.51,58.01},{46.07,70.68},{57.03,97.18},
  {86.40,119.32},{42.64,63.65},{35.08,92.64},{11.32,40.00},
  {40.40,80.05},{42.22,76.46},{29.24,50.94},{ 5.32,22.49},
  {96.61,135.37},{13.59,35.32},{98.40,134.57},{19.69,43.81},
  {67.27,113.56},{71.06,107.98},{ 1.06,26.45},{54.78,84.04},
  {45.64,76.93},{87.77,126.37},{33.46,51.98},{22.89,48.40},
  {38.64,60.20},{60.97,101.94},{47.41,73.39},{60.58,95.67},
  {88.12,106.82},{66.37,100.55},{81.12,115.71},{52.49,76.55},
  {85.05,125.56},{32.06,60.44},{60.84,103.55},{43.76,67.29},
  {62.33,88.25},{40.77,70.46},{94.72,121.74},{34.02,73.96},
  {32.40,61.97},{41.78,64.30},{89.28,121.32},{53.05,80.41},
  {27.30,68.46},{75.74,120.62},{65.47,93.80},{82.22,116.83},
  {97.54,150.07},{16.82,36.26},{18.42,41.45},{87.32,133.69},
  {77.52,109.91},{77.99,104.68},{25.88,61.94},{ 5.27,34.14},
  {74.93,106.74},{82.90,120.76},{37.96,82.08},{68.52,105.20},
  {39.62,65.56},{65.81,78.87},{29.38,36.77},{81.97,111.49},
  {21.94,56.46},{38.19,55.31},{70.62,92.61},{ 4.07,14.27},
  {58.56,92.06},{41.04,69.16},{64.68,99.07},{71.11,121.16},
  { 5.18,41.83},{88.83,126.77},{90.90,130.91},{87.74,123.22},
  {44.98,75.14},{38.64,70.51},{71.87,96.28},{68.72,107.04},
  {40.52,56.31},{70.48,98.16},{56.00,85.11},{30.70,58.96},
  {44.16,78.72},{94.58,145.43},{13.56,37.67},{40.48,53.91},
  {38.35,63.89},{85.90,127.17},{71.48,109.19},{16.73,54.54},
  {41.59,70.35},{51.48,85.62},{85.93,127.63},{15.94,25.57},
  {79.96,105.08},{80.48,122.53},{56.56,77.18},{ 0.37, 3.28},
  {53.10,95.77},{56.80,95.59},{17.64,45.56},{34.34,50.47},
  {31.32,59.70},{68.24,109.98},{71.80,107.38},{67.99,111.04},
  {41.43,60.69},{13.88,42.31},{84.77,101.64},{49.69,72.67},
  {22.84,59.96},{11.46,14.03},{ 6.25,29.29},{ 7.28,31.53},
  {74.19,88.55},{33.73,57.99},{61.34,109.98},{47.19,83.93},
  {43.72,61.28},{21.77,52.41},{76.98,100.87},{35.81,66.20},
  {43.75,79.14},{59.39,79.98},{49.23,95.35},{80.09,96.68},
  {48.21,87.57},{75.91,110.03},{47.81,91.15},{94.49,152.25},
  {81.66,127.38},{12.89,40.54},{22.74,61.81},{62.68,89.80},
  {56.24,91.10},{30.46,55.71},{36.86,70.38},{95.50,120.18},
  {69.14,98.70},{95.94,147.08},{ 8.60,27.55},{62.82,106.65},
  {30.90,46.78},{47.67,77.17},{32.79,45.39},{55.30,75.56},
  {80.68,119.01},{38.33,67.72},{87.41,123.32},{37.30,61.33},
  {66.09,105.27},{67.50,96.94},{ 6.82,28.21},{ 4.34,29.01},
  {80.83,125.28},{32.25,54.26},{97.62,132.83},{49.25,76.67},
  {96.51,121.49},{33.79,70.52},{83.52,129.24},{27.06,60.63},
  { 1.41,12.59},{13.22,57.64},{17.45,36.09},{71.35,76.96},
  {87.88,130.79},{50.39,83.88},{89.14,138.52},{11.44,41.46},
  {93.17,130.39},{67.21,95.80},{29.16,62.17},{92.98,143.45},
  {47.99,76.13},{27.43,71.97},{51.93,96.56},{91.03,134.99},
  {96.22,133.30},{41.24,49.18},{20.27,41.71},{75.72,106.00},
  {50.23,93.58},{70.66,112.03},{88.12,124.27},{75.71,125.10},
  {62.75,90.70},{ 9.55,35.97},{64.15,103.11},{15.63,51.96},
  {15.97,49.70},{16.27,37.28},{36.98,53.67},{98.92,133.42},
  {87.92,140.97},{49.12,84.19},{54.28,89.62},{71.33,96.50},
  {54.58,94.85},{68.25,112.61},{66.23,96.79},{49.85,98.43},
  {48.26,84.08},{ 6.28,38.20},{39.44,96.00},{92.27,133.34},
  {79.68,110.59},{62.13,108.32},{54.00,91.27},{27.57,67.10},
  {53.49,91.53},{20.29,36.56},{28.41,48.46},{65.46,83.93},
  {93.88,144.37},{61.47,90.48},{39.03,74.45},{52.76,92.72},
  {19.56,50.89},{34.67,61.01},{34.25,61.48},{69.18,92.59},
  {88.76,122.35},{ 6.16,31.03},{10.21,46.60},{97.95,135.81},
  {83.07,120.37},{66.03,105.65},{63.71,99.56},{17.93,35.56},
  {78.71,119.64},{ 1.26,29.44},{97.17,131.88},{24.25,55.43},
  {47.36,74.18},{81.31,113.86},{37.68,50.18},{26.47,37.91},
  {74.66,115.30},{ 5.37,32.89},{ 9.25,14.24},{32.83,57.97},
  { 9.01,34.09},{31.82,67.05},{75.57,110.86},{13.20,58.84},
  {38.74,58.61},{34.55,49.28},{ 8.63,18.41},{94.31,153.83},
  {59.27,100.95},{68.09,95.94},{33.94,56.80},{22.81,52.08},
  {17.09,51.37},{20.13,53.11},{23.53,45.18},{36.24,54.80},
  {84.20,111.33},{32.35,77.34},{38.68,73.17},{ 6.00,24.34},
  {30.14,57.76},{18.18,25.99},{48.93,70.53},{63.37,85.23},
  {34.74,71.07},{22.82,60.05},{54.59,82.07},{63.82,102.78},
  {70.17,106.61},{ 1.74,21.29},{54.94,91.34},{78.52,104.39},
  {57.23,113.05},{66.98,97.65},{17.06,39.70},{36.63,64.22},
  {51.49,67.72},{76.94,123.46},{98.72,140.65},{19.58,48.67},
  {25.91,54.75},{18.70,25.83},{80.77,119.17},{88.11,128.37},
  {78.37,132.97},{66.91,105.66},{33.54,56.14},{49.66,86.32},
  {32.00,58.47},{40.01,69.54},{56.12,73.78},{96.04,141.93},
  {31.96,68.36},{94.41,121.09},{76.95,109.13},{55.68,88.70},
  { 3.88,44.47},{ 7.22,36.32},{65.27,96.32},{88.18,93.03},
  {60.77,63.94},{63.81,84.81},{47.29,80.62},{82.50,127.64},
  {31.43,46.32},{13.18,32.13},{93.39,136.75},{85.27,142.09},
  {43.24,108.17},{75.25,125.18},{33.46,93.07},{90.49,142.00},
  {79.88,118.01},{20.84,62.60},{63.69,99.74},{76.84,126.38},
  {11.54,30.15},{76.43,118.27},{35.50,66.80},{45.32,66.67},
  {66.48,100.04},{73.72,99.06},{12.85,35.73},{62.84,91.74},
  { 6.88,37.00},{89.26,132.68},{85.01,107.91},{23.87,56.07},
  {38.03,56.40},{92.03,130.52},{42.02,89.57},{13.17,30.48},
  {82.16,130.63},{22.68,58.86},{ 8.52,38.41},{16.25,42.46},
  {65.35,97.29},{31.45,49.97},{59.30,87.47},{65.11,117.40},
  {29.39,50.87},{22.55,63.15},{36.95,65.12},{83.54,123.62},
  {11.75,25.18},{63.39,102.99},{61.42,83.35},{34.12,68.55},
  {90.74,152.52},{51.08,82.09},{20.28,41.21},{76.19,110.13},
  {37.99,67.26},{91.05,147.73},{11.77,19.86},{28.29,67.38},
  {66.26,98.05},{22.57,34.01},{16.30,25.37},{44.77,80.61},
  {46.39,82.78},{74.08,115.65},{53.10,92.48},{ 0.04,28.78},
  {29.00,42.26},{ 4.81,25.29},{28.74,47.05},{33.14,69.33},
  {87.96,129.02},{54.49,75.32},{37.37,58.59},{99.58,147.95},
  {21.68,56.37},{ 1.48,37.32},{91.01,134.77},{88.29,129.72},
  {32.73,59.08},{96.57,135.48},{90.10,130.41},{20.21,48.08},
  {52.88,70.85},{89.69,133.44},{24.74,51.81},{73.87,104.53},
  {61.84,91.90},{65.37,84.10},{41.40,74.81},{38.22,54.51},
  {18.24,41.00},{90.06,124.68},{ 2.89,34.07},{80.14,106.74},
  {89.91,127.71},{60.63,102.62},{35.00,68.27},{70.63,100.02},
  {15.65,34.17},{71.64,95.74},{18.05,40.60},{38.08,50.51},
  {88.71,110.01},{16.08,55.91},{80.55,111.48},{36.13,62.87},
  {85.88,108.71},{50.75,102.58},{87.31,144.42},{23.96,42.23},
  {71.56,117.05},{88.79,139.37},{98.10,122.99},{26.68,52.26},
  {12.51,18.84},{66.50,109.63},{19.18,58.66},{78.17,120.33},
  {14.11,41.17},{31.29,56.93},{39.37,61.22},{ 0.85,43.30},
  {48.95,72.30},{81.94,126.11},{69.96,100.68},{48.34,76.21},
  {66.52,123.56},{15.95,33.56},{ 5.51,24.27},{28.75,76.63},
  {13.20,30.41},{51.47,86.79},{84.84,128.08},{50.96,74.05},
  {76.41,119.73},{75.18,105.68},{ 3.85,24.64},{42.69,70.68},
  {86.75,133.83},{ 8.31,17.44},{73.34,106.15},{32.65,66.74},
  {44.83,73.09},{ 8.14,59.97},{49.55,75.57},{30.72,48.02},
  {92.47,134.65},{81.10,133.44},{29.67,58.25},{84.63,131.65},
  {16.49,38.95},{77.48,128.30},{42.96,74.42},{86.71,108.12},
  {26.98,38.76},{60.41,91.56},{91.91,130.19},{86.14,128.67},
  {91.64,141.09},{55.81,97.06},{48.26,78.28},{41.49,75.39},
  {36.58,59.50},{76.44,85.43},{72.56,106.57},{72.87,105.68},
  {15.32,10.87},{ 7.18,30.28},{92.52,124.86},{93.91,136.79},
  {31.38,64.56},{91.97,130.64},{24.10,62.56},{42.05,65.55},
  {18.59,41.41},{41.94,62.05},{ 0.75,45.24},{91.66,121.28},
  {88.75,111.57},{13.14,53.87},{67.12,96.45},{55.66,89.07},
  {13.93,25.23},{ 6.10,26.44},{23.28,48.96},{14.55,22.55},
  {31.85,44.45},{37.61,63.29},{22.94,42.72},{57.14,89.52},
  {52.50,77.71},{99.59,154.03},{36.59,49.47},{26.34,55.29},
  {96.63,148.57},{91.80,135.06},{94.11,134.16},{55.52,95.86},
  {25.79,52.48},{84.44,118.14},{ 6.44,29.89},{35.95,48.28},
  {31.95,58.34},{40.99,69.64},{12.35,26.00},{67.47,98.87},
  {57.15,92.49},{68.42,96.52},{14.48,29.33},{33.90,71.00},
  { 0.59,24.46},{24.79,35.20},{69.86,96.24},{77.34,121.23},
  {16.53,28.60},{96.95,141.18},{80.66,110.56},{45.52,90.34},
  {63.06,106.13},{42.90,74.99},{51.66,79.20},{80.40,115.11},
  {25.35,45.56},{68.82,94.10},{14.07,26.03},{30.36,59.02},
  {39.87,65.31},{ 5.17,24.49},{77.51,113.81},{ 9.52,29.77},
  { 8.30,24.24},{44.04,92.38},{ 8.16,14.50},{47.82,73.90},
  {90.61,105.32},{72.52,91.53},{45.44,69.37},{75.18,100.34},
  { 9.15,21.48},{12.40,27.27},{32.11,57.98},{63.31,94.25},
  {69.98,100.76},{98.21,145.59},{46.96,58.96},{26.95,45.25},
  {84.89,126.56},{11.73,30.97},{94.06,122.71},{77.29,127.08},
  {62.73,101.50},{99.86,138.24},{46.33,72.20},{22.49,54.21},
  {95.73,134.34},{90.17,128.09},{68.55,92.58},{71.82,100.12},
  {32.57,71.35},{41.17,72.70},{ 6.60,36.63},{94.76,135.53},
  {29.34,80.82},{89.52,140.31},{96.29,127.96},{55.33,89.82},
  { 9.28,23.22},{96.95,127.20},{65.60,108.09},{82.77,122.12},
  {93.00,110.03},{82.74,111.85},{93.82,140.22},{94.54,124.90},
  {45.18,102.72},{25.24,35.61},{22.80,58.73},{79.85,121.12},
  { 8.31,41.44},{99.01,131.20},{ 0.59,15.39},{64.58,107.07},
  {88.62,121.65},{81.62,137.18},{94.06,132.65},{96.52,123.62},
  { 2.46,36.78},{22.52,43.56},{13.21,35.58},{ 7.60,16.69},
  {66.39,96.44},{ 5.81,22.40},{25.76,48.90},{95.19,146.27},
  {43.21,82.26},{67.96,111.70},{85.21,127.00},{ 4.99,23.50},
  {68.93,105.76},{63.58,81.18},{81.53,105.11},{63.83,90.91},
  {54.74,94.01},{61.84,109.68},{56.63,91.84},{24.59,51.08},
  {62.64,88.02},{88.04,132.49},{88.78,135.86},{18.67,49.25},
  {74.69,120.78},{64.37,103.79},{21.19,60.29},{52.40,90.69},
  {11.87,25.06},{ 7.19,21.90},{36.87,66.72},{61.64,91.63}  
};