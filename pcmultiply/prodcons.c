/*
 *  prodcons module
 *  Producer Consumer module
 *
 *  Implements routines for the producer consumer module based on
 *  chapter 30, section 2 of Operating Systems: Three Easy Pieces
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Fall 2016
 */

// Include only libraries for this module
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "counter.h"
#include "matrix.h"
#include "pcmatrix.h"
#include "prodcons.h"

/*
prodcons myProdCons;

myProdCons.sumtotal = 1;
myProdCons.multtotal = 2;
myProdCons.matrixtotal = 3:
*/
typedef struct prodcons *gStats;
int sumTotal = 0;
int multiplyTotal = 0;
int matrixProduced = 0;
int matrixConsumed = 0;
int matrixTotal = 0;
int elementsProduced = 0;
int elementsConsumed = 0;



//ProdConsStats.sumtotal = 1;
//ProdConsStats.multtotal = 2;
//ProdConsStats.matrixtotal = 3;

int totalSum() {
	return sumTotal;
}
int multiplySum() {
	return multiplyTotal;
}

int matrixProd() {
	return matrixProduced;
}

int matrixCons() {
	return matrixConsumed;
}

int matrixSum() {
	return matrixTotal;
}

int elementProducedSum() {
	return elementsProduced;
}
int elementConsumedSum() {
	return elementsConsumed;
}


// Define Locks and Condition variables here
pthread_mutex_t mutex;
pthread_cond_t empty, fill;

// Producer consumer data structures
Matrix * bigmatrix[MAX];

int count = 0;
int fill_ptr = 0;
int use_ptr = 0;

// Bounded buffer put() get()

//put matrix on bounded buffer, increase the count of amount of matrices on buffer
void put(Matrix * value)
{
  bigmatrix[fill_ptr] = value;
  fill_ptr = (fill_ptr + 1) % MAX;
  count++;
  matrixProduced++; // total produced matrixes
  matrixTotal++;

}

//get matrix from the bounded buffer, decrease the count of matrices on buffer
Matrix * get()
{
  Matrix * tmp = bigmatrix[use_ptr];
  use_ptr = (use_ptr + 1) % MAX;
  count--;
  matrixConsumed++; // total consumed matrixes
  matrixTotal++;
  return tmp;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  int i = 0;
  //generate a random matrix and put on buffer if less than LOOPS
  while (i < LOOPS) {

    pthread_mutex_lock(&mutex);
    //if the bounded buffer is full (MAX) then wait on for consumer
    while (count == MAX) {
        pthread_cond_wait(&empty, &mutex);
    }
    Matrix *m = GenMatrixRandom();
    put(m);
		elementsProduced +=  SumMatrix(m);
    i++;
    pthread_cond_signal(&fill);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  //test for amount of m3 matrices created
  //int metricscounter = 1;
  int i = 0;
  int gate = 0;
  Matrix *tmp1;
  Matrix *tmp2;
  //get matrix for amount of LOOPS from buffer
  while (i < LOOPS) {
    pthread_mutex_lock(&mutex);
    //wait when the bounded buffer is empty
    while (count == 0) {
        pthread_cond_wait(&fill, &mutex);
    }
    //if gate = 0 then it is the first matrix (m1)
    if(gate == 0) {
      tmp1 = get();
      gate = 1;
			elementsConsumed += SumMatrix(tmp1);

    }
    //else it is the second matrix
    else {
      tmp2 = get();
      Matrix *ans = MatrixMultiply(tmp1, tmp2);
			elementsConsumed += SumMatrix(tmp2);

      //if the result of multiplying is NULL then get rid of m2 and run the loop again
      if(ans == NULL) {
        FreeMatrix(tmp2);
      }
      //otherwise we have a real answer, display the matrix, turn the gate back to 0 and free all
      else {
        //printf("Matrix number %d\n", metricscounter);
        //metricscounter++;
        DisplayMatrix(ans, stdout);
        SumMatrix(ans);
        multiplyTotal++;
        gate = 0;
        FreeMatrix(tmp1);
        FreeMatrix(tmp2);
        FreeMatrix(ans);
      }
    }
    //increase i counter and signal
    i++;
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}
