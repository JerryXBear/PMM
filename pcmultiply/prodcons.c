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


// Define Locks and Condition variables here
pthread_mutex_t mutex;
pthread_cond_t empty, fill;

// Producer consumer data structures
Matrix * bigmatrix[MAX];

int count = 0;
int fill_ptr = 0;
int use_ptr = 0;
// Bounded buffer put() get()
int put(Matrix * value)
{
  bigmatrix[fill_ptr] = value;
  fill_ptr = (fill_ptr + 1) % MAX;
  count++;
  return count;
}

Matrix * get() 
{
  int tmp = bigmatrix[use_ptr];
  use_ptr = (use_ptr + 1) % MAX;
  count--;
  return tmp;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  int i = 0;
  //int j = 0;
  //int crow = -1;
  //int ccol = 0;
  //int gate = 0;
  //while (j < LOOPS) {
    //Matrix m = GenRandomMatrix();
    //if (gate == 0) {
      //i++;
      //j++;
      //ccol = m->cols;
      //put(m);
      //gate = 1;
    //}
    //else {
      //i++;
      //crow = m->rows;
      //if(crow == ccol) {
        //put(m);
        //j++;
        //gate = 0;
      //}      
  //}
  //generate a random matrix and put on buffer if less than LOOPS
  while (i < LOOPS) {
    Matrix *m = GenMatrixRandom();
    put(m);
    i++;
  }
  return NULL;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  int i = 0;
  int gate = 0;
  Matrix *tmp1;
  Matrix *tmp2;
  while (i < LOOPS) {
    if(gate == 0) {
      tmp1 = get();
      i++;
      gate = 1;
    }
    else {
      tmp2 = get();
      i++;
      Matrix *ans = MatrixMultiply(tmp1, tmp2);
      if(ans == NULL) {
        FreeMatrix(tmp2);
      }
      else {
        DisplayMatrix(ans, stdout);
        gate = 0;
      }
    }
  }
  return NULL;
}

