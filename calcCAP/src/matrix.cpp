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

Matrix& Matrix::operator=(const Matrix& mat) {

  int i, j;
  if (d_i != mat.d_i || d_j != mat.d_j) {
    for(i = 0; i < d_i; i++) delete v[i];
    delete v;
    d_i = mat.d_i;
    d_j = mat.d_j;
    v = new Vector*[d_i];
    for(i = 0; i < d_i; i++) v[i] = new Vector(d_j);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) elem(i,j) = mat.elem(i,j);
  return (*this);
}

int Matrix::operator==(const Matrix& x) const {
 
  int i, j;
  if (d_i != x.d_i || d_j != x.d_j) return (0);

  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      if (elem(i,j) != x.elem(i,j)) return (0);
  return (1);
}

Vector& Matrix::row(int i) const {

  if (i < 0 || i >= d_i)  {
    cerr << "Matrix: row index out of range" << endl;
    exit(1);
  }
  return (*v[i]);
}

double& Matrix::operator()(int i, int j) {

  if (i < 0 || i >= d_i) {
    cerr << "Matrix: row index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) {
    cerr << "Matrix: col index out of range" << endl;
    exit(1);
  }
  return (elem(i,j));
}

double& Matrix::operator()(int i, int j) const {

  if (i < 0 || i >= d_i) {
    cerr << "Matrix: row index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) {
    cerr << "Matrix: col index out of range" << endl;
    exit(1);
  }
  return (elem(i,j));
}

Vector Matrix::col(int i)  const {

  if (i < 0 || i >= d_j) {
    cerr << "Matrix: col index out of range" << endl;
    exit(1);
  }
  Vector result(d_i);
  int j = d_i;
  while (j--) result.v[j] = elem(j,i);
  return (result);
}

Vector Matrix::diag()  const {

  if (d_i != d_j) {
    cerr << "Matrix: diag defined only if d_i = d_j" << endl;
    exit(1);
  }
  Vector result(d_i);
  int j = d_i;
  while (j--) result.v[j] = elem(j,j);
  return (result);
}

Matrix::operator Vector() const {

  if (d_j != 1) {
    cerr << "error: cannot make vector from matrix" << endl;
    exit(1);
  }
  return (col(0));
}

Matrix Matrix::operator+(const Matrix& mat) {

  int i, j;
  check_dimensions(mat);
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) + mat.elem(i,j);
  return (result);
}

Matrix Matrix::operator-(const Matrix& mat) {

  int i, j;
  check_dimensions(mat);
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) - mat.elem(i,j);
  return (result);
}

Matrix Matrix::operator-( ) {

  int i, j;
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j= 0; j < d_j; j++)
      result.elem(i,j) = -elem(i,j);
  return (result);
}

Matrix& Matrix::operator+=(const Matrix& mat) {

  int i, j;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) += mat.elem(i,j);
  return (*this);
}

Matrix& Matrix::operator-=(const Matrix& mat) {

  int i, j;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) -= mat.elem(i,j);
  return (*this);
}

Matrix& Matrix::operator/=(double x) {

  int i, j;
  if (x == 0) {
    cerr << "Matrix/=: divided by zero" << endl;
    exit(1);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) /= x;
  return (*this);
}

Matrix& Matrix::operator*=(double x) {

  int i, j;
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) *= x;
  return (*this);
}

Matrix Matrix::operator*(double f) {

  int i, j;
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) * f;
  return (result);
}

Matrix Matrix::operator*(double f) const {

  int i, j;
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) * f;
  return (result);
}

Matrix Matrix::operator*(const Matrix& mat) {

  if (d_j != mat.d_i) {
    cerr << "matrix multiplication: incompatible matrix types" << endl;
    exit(1);
  }
  Matrix result(d_i, mat.d_j);
  int i,j;
  for (i = 0; i < mat.d_j; i++)
    for (j = 0; j < d_i; j++) result.elem(j,i) = *v[j] * mat.col(i);
 return (result);
}

Matrix Matrix::operator/(double a) {

  if(a == 0) {
    cerr << "Matrix: divided by zero" << endl;
    exit(1);
  }
  a = 1. / a;
  
  return (Matrix(*this *a ));
}

Matrix Matrix::operator^(const Matrix& mat) const {

  if (d_i != mat.d_i || d_j != mat.d_j) {
    cerr << "Matrix=: matrixes have different sizes" << endl;
    exit(1);
  }
  Matrix res(d_i,d_j); 
  for(int i = 0; i < d_i; i++)
     for(int j = 0; j < d_j; j++)
        res.elem(i,j) = elem(i,j)*mat.elem(i,j);
  return (res);
}

Matrix Matrix::trans() const {

  Matrix result(d_j,d_i);
  for(int i = 0; i < d_j; i++)
    for(int j = 0; j < d_i; j++)
        result.elem(i,j) = elem(j,i);
  return (result);
}

Matrix Matrix::abs() const {

  Matrix result(d_i,d_j);
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      result(i,j) = fabs(elem(i,j));
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

Matrix& Matrix::zeros() {
  int i, j;
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
