/*
 *  pcmatrix module
 *  Primary module providing control flow for the pcMatrix program
 * 
 *  Producer consumer bounded buffer program to produce random matrices in parallel
 *  and consume them while searching for valid pairs for matrix multiplication.
 *  Matrix multiplication requires the first matrix column count equal the 
 *  second matrix row count.  
 *
 *  A matrix is consumed from the bounded buffer.  Then matrices are consumed
 *  from the bounded buffer, one at a time, until an eligible matrix for multiplication
 *  is found.
 *
 *  Totals are tracked using the ProdConsStats Struct for:
 *  - the total number of matrices multiplied (multtotal from consumer threads)
 *  - the total number of matrices produced (matrixtotal from producer threads)
 *  - the total number of matrices consumed (matrixtotal from consumer threads)
 *  - the sum of all elements of all matrices produced and consumed (sumtotal from producer and consumer threads)
 *
 *  Correct programs will produce and consume the same number of matrices, and
 *  report the same sum for all matrix elements produced and consumed. 
 *
 *  For matrix multiplication only ~25% may be e
 *  and consume matrices.  Each thread produces a total sum of the value of
 *  randomly generated elements.  Producer sum and consumer sum must match.
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Fall 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include "matrix.h"
#include "counter.h"
#include "prodcons.h"
#include "pcmatrix.h"

int main (int argc, char * argv[])
{
  time_t t;
  int numw = NUMWORK;

  // Seed the random number generator with the system time
  srand((unsigned) time(&t));

  //
  // Demonstration code to show the use of matrix routines

  // ------------------------------------------------------------

  printf("Producing %d %dx%d matrices.\n",LOOPS, ROW, COL);
  printf("Using a shared buffer of size=%d\n", MAX);
  printf("With %d producer and consumer thread(s).\n",numw);
  printf("\n");

  //pthread_t pr;
  //pthread_t co;

  //declare variables for an array of threads
  pthread_t *prarray;
  pthread_t *coarray;

  //allocate memory based on size of thread * number of threads used
  prarray = malloc(sizeof(pthread_t)*numw);
  coarray = malloc(sizeof(pthread_t)*numw);  

  //create and join producer and consumer threads based on the number specified in pcmatrix.h
  for (int i = 0; i < numw; i++) {
    pthread_create(&prarray[i], NULL, prod_worker, LOOPS);
    pthread_create(&coarray[i], NULL, cons_worker, LOOPS);
    pthread_join(prarray[i], NULL);
    pthread_join(coarray[i], NULL);
  }


  int prs = 0;
  int cos = 0;
  int prodtot = 0;
  int constot = 0;
  int consmul = 0;

  prs = elementProducedSum();
  cos = elementConsumedSum();
  prodtot = matrixProd();
  constot = matrixCons();
  consmul = multiplySum();

  // consume ProdConsStats from producer and consumer threads
  // add up total matrix stats in prs, cos, prodtot, constot, consmul 

  printf("Sum of Matrix elements --> Produced=%d = Consumed=%d\n",prs,cos);
  printf("Matrices produced=%d consumed=%d multiplied=%d\n",prodtot,constot,consmul);

  return 0;
}
