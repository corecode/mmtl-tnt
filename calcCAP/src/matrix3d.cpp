//#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include "matrix3d.h"
#include "vector.h"
#include "matrix.h"


/********************** constructors ****************************/

Matrix3d::Matrix3d(const Matrix3d& p) {

  d_i = p.d_i;
  d_j = p.d_j;
  d_k = p.d_k;    
  if (d_i > 0) {
    v = new double**[d_i];
    for(int i = 0; i < d_i; i++) {  
      v[i] = new double*[d_j]; 
      for(int j = 0; j < d_j; j++) {
        v[i][j] = new double[d_k]; 
        for(int k = 0; k < d_k; k++)  elem(i,j,k) = p.elem(i,j,k);
      }
    }    
  }
  else v = NULL;
}

Matrix3d::Matrix3d(const Matrix& mat) {

  d_i = mat.dim_i();
  d_j = mat.dim_j();
  d_k = 1;
  v = new double**[d_i];
  for(int i = 0; i < d_i; i++) {  
    v[i] = new double*[d_j]; 
    for(int j = 0; j < d_j; j++) {
      v[i][j] = new double[d_k]; 
      elem(i,j,0) = mat.elem(i,j);
    }  
  }
}

Matrix3d::Matrix3d(const Vector& vec) {

  d_i = vec.d;
  d_j = 1;
  d_k = 1;
  v = new double**[d_i];
  for(int i = 0; i < d_i; i++) {  
    v[i] = new double*[d_j]; 
    for(int j = 0; j < d_j; j++) {
      v[i][j] = new double[d_k]; 
      elem(i,0,0) = vec[i];
    }  
  }
}

Matrix3d::Matrix3d(int dim1, int dim2, int dim3, double a) {

  d_i = dim1; 
  d_j = dim2; 
  d_k = dim3;
  v = new double**[d_i];
  for(int i = 0; i < d_i; i++) {  
    v[i] = new double*[d_j]; 
    for(int j = 0; j < d_j; j++) {
      v[i][j] = new double[d_k]; 
      for(int k = 0; k < d_k; k++)  elem(i,j,k) = a;
    }
  }
}

Matrix3d::~Matrix3d() {

  if (v) {
    for(int i=0; i<d_i; i++)
       for(int j=0; j<d_j; j++)
          delete v[i][j];
    delete v;
  }
}

/************************ members *******************************/

void Matrix3d::free() {

  if (v) {
    for(int i=0; i<d_i; i++)
       for(int j=0; j<d_j; j++)
          delete v[i][j];
    delete v;
  }  
}

void Matrix3d::check_dimensions(const Matrix3d& mat) const {

  if (d_i != mat.d_i || d_j != mat.d_j || d_k != mat.d_k) {
    cerr << "Matrix3d:incompatible matrix types." << endl;
    exit(1);
  }
}

Matrix3d& Matrix3d::operator=(const Matrix3d& mat) {

  int i, j, k;
  if (d_i != mat.d_i || d_j != mat.d_j || d_k != mat.d_k) {
    for(i = 0; i < d_i; i++)
       for(j = 0; j < d_j; j++)
          delete v[i][j];
    delete v;
    d_i = mat.d_i;
    d_j = mat.d_j;
    d_k = mat.d_k;
    v = new double**[d_i];
    for(i = 0; i < d_i; i++) {  
      v[i] = new double*[d_j]; 
      for(j = 0; j < d_j; j++) {
        v[i][j] = new double[d_k];
      }
    }    
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) 
      for(k = 0; k < d_k; k++) 
        elem(i,j,k) = mat.elem(i,j,k);
  return (*this);
}

int Matrix3d::operator==(const Matrix3d& x) const {
 
  int i, j, k;
  if (d_i != x.d_i || d_j != x.d_j || d_k != x.d_k) return (0);

  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) 
      for(k = 0; k < d_k; k++) 
        if (elem(i,j,k) != x.elem(i,j,k)) return (0);
  return (1);
}

Vector Matrix3d::vec(int i, int j)  const {

  if (i < 0 || i >= d_i || j < 0 || j >= d_j ) {
    cerr << "Matrix3d: indexes in vec out of range" << endl;
    exit(1);
  }
  Vector result(d_k);
  int k = d_k;
  while (k--) result.v[k] = elem(i,j,k);
  return (result);
}

Vector  Matrix3d::row(int i) const {

  if (i < 0 || i >= d_i)  {
    cerr << "Matrix3d: row index out of range" << endl;
    exit(1);
  }
  Vector result(d_j);
  int j = d_j; 
  while(j--) result.v[j] = elem(i,j,0);
  return(result);
}

Vector Matrix3d::col(int i)  const {

  if (i < 0 || i >= d_j) {
    cerr << "Matrix3d: col index out of range" << endl;
    exit(1);
  }
  Vector result(d_i);
  int j = d_i;
  while (j--) result.v[j] = elem(j,i,0);
  return (result);
}

Matrix Matrix3d::mtr(int k) const {
  
  if(k < 0 || k >= d_k ) {
    cerr << "Matrix3d: k index out of range" << endl;
    exit(1);
  } 
  Matrix result(d_i,d_j);
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      result(i,j) = elem(i,j,k); 
  return(result);
}

double& Matrix3d::operator()(int i, int j, int k) {

  if (i < 0 || i >= d_i) {
    cerr << "Matrix3d: i index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) {
    cerr << "Matrix3d: j index out of range" << endl;
    exit(1);
  }
  if (k < 0 || k >= d_k) {
    cerr << "Matrix3d: k index out of range" << endl;
    exit(1);
  }
  return (elem(i,j,k));
}

double& Matrix3d::operator()(int i, int j, int k) const {

  if (i < 0 || i >= d_i) {
    cerr << "Matrix3d: i index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) {
    cerr << "Matrix3d: j index out of range" << endl;
    exit(1);
  }
  if (k < 0 || k >= d_k) {
    cerr << "Matrix3d: k index out of range" << endl;
    exit(1);
  }
  return (elem(i,j,k));
}

Matrix3d Matrix3d::operator+(const Matrix3d& mat) {

  int i, j, k;
  check_dimensions(mat);
  Matrix3d result(d_i,d_j,d_k);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      for(k = 0; k < d_k; k++)
        result.elem(i,j,k) = elem(i,j,k) + mat.elem(i,j,k);
  return (result);
}

Matrix3d Matrix3d::operator-(const Matrix3d& mat) {

  int i, j, k;
  check_dimensions(mat);
  Matrix3d result(d_i,d_j,d_k);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      for(k = 0; k < d_k; k++)
      result.elem(i,j,k) = elem(i,j,k) - mat.elem(i,j,k);
  return (result);
}

Matrix3d Matrix3d::operator-( ) {

  int i, j, k;
  Matrix3d result(d_i,d_j,d_k);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      for(k = 0; k < d_k; k++)
        result.elem(i,j,k) = -elem(i,j,k);
  return (result);
}

Matrix3d& Matrix3d::operator+=(const Matrix3d& mat) {

  int i, j, k;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      for(k = 0; k < d_k; k++)
        elem(i,j,k) += mat.elem(i,j,k);
  return (*this);
}

Matrix3d& Matrix3d::operator-=(const Matrix3d& mat) {

  int i, j, k;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      for(k = 0; k < d_k; k++)
        elem(i,j,k) -= mat.elem(i,j,k);
  return (*this);
}

Matrix3d Matrix3d::operator*(double f) {

  int i, j, k;
  Matrix3d result(d_i,d_j,d_k);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      for(k = 0; k < d_k; k++)
        result.elem(i,j,k) = elem(i,j,k) * f;
  return (result);
}

Matrix3d Matrix3d::operator*(double f) const {

  int i, j, k;
  Matrix3d result(d_i,d_j,d_k);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      for(k = 0; k < d_k; k++)
        result.elem(i,j,k) = elem(i,j,k) * f;
  return (result);
}

Matrix3d Matrix3d::operator/(double a) {

  if(a == 0) {
    cerr << "Matrix3d: divided by zero" << endl;
    exit(1);
  }
  a = 1. / a;
  
  return (Matrix3d(*this *a ));
}

double Matrix3d::operator*(const Matrix3d& mat) const {

  if (d_i != mat.d_i || d_j != mat.d_j || d_k != mat.d_k) {
    cerr << "Matrix3d: matrix3d have different length" << endl;
    exit(1);
  }
  double result = 0;
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      for(int k = 0; k < d_k; k++)
        result += elem(i,j,k) * mat.elem(i,j,k); 
  return(result);
}

Matrix3d Matrix3d::operator^(const Matrix3d& mat) const {

  if (d_i != mat.d_i || d_j != mat.d_j || d_k != mat.d_k) {
    cerr << "Matrix3d: matrix3d have different length" << endl;
    exit(1);
  }
  Matrix3d result(d_i,d_j,d_k);
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      for(int k = 0; k < d_k; k++)
        result.elem(i,j,k) = elem(i,j,k) * mat.elem(i,j,k); 
  return(result);
}

Matrix3d Matrix3d::abs() const {

  Matrix3d result(d_i,d_j,d_k);
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      for(int k = 0; k < d_k; k++)
        result(i,j,k) = fabs(elem(i,j,k));
  return (result);
}

Matrix3d& Matrix3d::zeros() {

  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      for(int k = 0; k < d_k; k++)
        elem(i,j,k) = 0.0;
  return (*this);
}

Matrix3d& Matrix3d::resize(int new_d_i, int new_d_j, int new_d_k) {
  
  int i, j, k;
  if (d_i != new_d_i || d_j != new_d_j || d_k != new_d_k) {
    for(i = 0; i < d_i; i++)
       for(j = 0; j < d_j; j++)
          delete v[i][j];
    delete v;
    d_i = new_d_i;
    d_j = new_d_j;
    d_k = new_d_k;
    v = new double**[d_i];
    for(i = 0; i < d_i; i++) {  
      v[i] = new double*[d_j]; 
      for(j = 0; j < d_j; j++) {
        v[i][j] = new double[d_k];
      }
    }    
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) 
      for(k = 0; k < d_k; k++) 
        elem(i,j,k) = 0;
  return (*this);
}

/***************************** friends **************************/

ostream& operator<<(ostream& s, const Matrix3d& mat) {
  
  int i,j,k;
  for(i = 0; i < mat.d_i; i++)
    for(j = 0; j < mat.d_j; j++)
      for(k = 0; k < mat.d_k; k++)
	s << " [" << i << "][" << j << "][" << k 
          << "] = " << mat.elem(i,j,k) << endl;     
  return (s);
}

Matrix3d operator*(double a, const Matrix3d& mat) {
  
  return(mat*a);
}

double max(const Matrix3d& mat) {
  
  int n = mat.dim_i();
  int m = mat.dim_j();
  Matrix c(n,m); 
  for(int i = 0; i < n; i++)   
    for(int j = 0; j < m; j++) 
      c(i,j) = max(mat.vec(i,j)); 
  return max(c);
}

double min(const Matrix3d& mat) {
  
  int n = mat.dim_i();
  int m = mat.dim_j();
  Matrix c(n,m);
  for(int i = 0; i < n; i++)   
    for(int j = 0; j < m; j++) 
      c(i,j) = min(mat.vec(i,j));
  return(min(c));
}

double norm(const Matrix3d& mat) {

  double result = 0;
  for(int i = 0; i < mat.dim_i(); i++)
    for(int j = 0; j < mat.dim_j(); j++)
      for(int k = 0; k < mat.dim_k(); k++)
        result += mat.elem(i,j,k);
  return (result);
}

double norm1(const Matrix3d& mat) {

  return (norm(mat.abs()));
}

double norm2(const Matrix3d& mat) {

  double result = 0;
  for(int i = 0; i < mat.dim_i(); i++)
    for(int j = 0; j < mat.dim_j(); j++)
      for(int k = 0; k < mat.dim_k(); k++)
        result += mat.elem(i,j,k)*mat.elem(i,j,k);
  return (sqrt(result));
}

double norm2_sqv(const Matrix3d& mat) {
   
  register double result = 0;
  for(int i = 0; i < mat.dim_i(); i++)
    for(int j = 0; j < mat.dim_j(); j++)
      for(int k = 0; k < mat.dim_k(); k++)
        result += mat.elem(i,j,k)*mat.elem(i,j,k);
  return (result);
}
