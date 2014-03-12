#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "vector.h"
//#include "stdafx.h"

#define EPS 1.0e-12

/********************** constructors ****************************/

Matrix::Matrix(const Matrix& p) {

  d_i = p.d_i;
  d_j = p.d_j;
    
  if (d_i > 0) {
    v = new Vector*[d_i];
    for (int i = 0; i < d_i; i++)
      v[i] = new Vector(*p.v[i]); 
  }
  else v = NULL;
}

Matrix::Matrix(const Vector& vec) {

  d_i = vec.d;
  d_j = 1;
  v = new Vector*[d_i];
  for(int i = 0; i < d_i; i++) {
    v[i] = new Vector(1);
    elem(i,0) = vec[i];
  }
}

Matrix::Matrix(int dim1, int dim2, double a) {

  d_i = dim1; 
  d_j = dim2; 
  v = new Vector*[d_i];
  for(int i = 0; i < d_i; i++) {
    v[i] = new Vector(d_j); 
    for(int j = 0; j < d_j; j++)  elem(i,j) = a;
  }
}

Matrix::~Matrix() {

  if (v) {
    while(d_i--) delete v[d_i]; 
    delete v;
  }
}

/************************ members *******************************/

void Matrix::flip_rows(int i,int j)
{ Vector* p = v[i];
  v[i] = v[j];
  v[j] = p;
 }

void Matrix::free() {

  if (v) {
    while(d_i--) delete v[d_i];
    delete v;
  }
}

void Matrix::check_dimensions(const Matrix& mat) const {

  if (d_i != mat.d_i || d_j != mat.d_j) {
    cerr << "Matrix:incompatible matrix types." << endl;
    exit(1);
  }
}

Matrix Matrix::trans() const {

  Matrix result(d_j,d_i);
  for(int i = 0; i < d_j; i++)
    for(int j = 0; j < d_i; j++)
        result.elem(i,j) = elem(j,i);
  return (result);
}


Matrix& Matrix::resize(int new_d_i, int new_d_j) {
  int i, j;
  if (d_i != new_d_i || d_j != new_d_j) {
    for(i = 0; i < d_i; i++) delete v[i];
      delete v;
      d_i = new_d_i;
      d_j = new_d_j;
      v = new Vector*[d_i];
      for(i = 0; i < d_i; i++) v[i] = new Vector(d_j);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) elem(i,j) = 0.0;
  return (*this);
}

/***************************** friends **************************/

ostream& operator<<(ostream& s, const Matrix& M) {
  //int i;
  //for (i = 0; i < M.d_i; i++) s << M[i]; 
  
  int i,j;
  for(j=0; j < M.d_j; j++)
     for(i=0; i < M.d_i; i++)
       s << " [" << i << "][" << j << "] = " << M[i][j]<< endl;   
  
  return (s);
}

istream& operator>>(istream& s, Matrix& M) {

  int i = 0;
  while (i < M.d_i && s >> M[i++]);
  return (s);
}

Matrix operator*(double a, const Matrix& mat) {
  
  return(mat*a);
}

double max(const Matrix& M) {
  
  int n = M.dim_j();
  Vector c(n); 
  while (n--) c[n] = max(M.col(n)); 
  return max(c);
}

double min(const Matrix& M) {
  
  int n = M.dim_j();
  Vector c(n);
  while (n--) c[n] = min(M.col(n));
  return(min(c));
}

double norm(const Matrix& mat) {

  register double result = 0.0;
  for(int i = 0; i < mat.dim_i(); i++)
     for(int j = 0; j < mat.dim_j(); j++)
        result += mat.elem(i,j);  
  return (result);
}

double norm1(const Matrix& mat) {

  return (norm(mat.abs()));
}

double norm2(const Matrix& mat) {

  register double result = 0.0;
  for(int i = 0; i < mat.dim_i(); i++)
    for(int j = 0; j < mat.dim_j(); j++)
        result += mat.elem(i,j)*mat.elem(i,j);
  return (sqrt(result));
}

double norm2_sqv(const Matrix& mat) {
   
  register double result = 0.0;
  for(int i = 0; i < mat.dim_i(); i++)
    for(int j = 0; j < mat.dim_j(); j++)
        result += mat.elem(i,j)*mat.elem(i,j);
  return (result);
}

void Matrix::swap_rows(int m, int n) {

  flip_rows(m,n);
}

void del(Matrix& mat) {
  
  mat.resize(0,0);
}
