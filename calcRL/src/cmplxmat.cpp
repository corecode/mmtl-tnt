//#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include "cmplxmat.h"


/******************** constructors *************************/

CmplxMatrix::CmplxMatrix(int dim1, int dim2, double a) {

  if (dim1 < 0 || dim2 < 0) 
    cerr << "complex matrx: negative dimension." << endl; 

  d_i = dim1; 
  d_j = dim2; 
  if (d_i > 0) {
    v = new CmplxVector*[d_i];
    for (int i = 0; i < d_i; i++) {
      v[i] = new CmplxVector(d_j); 
      for(int j = 0; j < d_j; j++)  elem(i,j) = a;
    }
  }
  else v = NULL;
}

CmplxMatrix::CmplxMatrix(int dim1, int dim2, Complex a) {

  if (dim1 < 0 || dim2 < 0) 
    cerr << "complex matrx: negative dimension." << endl; 

  d_i = dim1; 
  d_j = dim2; 
  if (d_i > 0) {
    v = new CmplxVector*[d_i];
    for (int i = 0; i < d_i; i++) {
      v[i] = new CmplxVector(d_j); 
      for(int j = 0; j < d_j; j++)  elem(i,j) = a;
    }
  }
  else v = NULL;
}

CmplxMatrix::CmplxMatrix(const CmplxMatrix& p) {
 
  d_i = p.d_i;
  d_j = p.d_j;
  if (d_i > 0) {
    v = new CmplxVector*[d_i];
    for (int i = 0; i < d_i; i++) v[i] = new CmplxVector(*p.v[i]); 
  }
  else v = NULL;
}

CmplxMatrix::CmplxMatrix(const CmplxVector& vec) {
  
  d_i = vec.dim();
  d_j = 1;
  v = new CmplxVector*[d_i];
  for(int i = 0; i < d_i; i++) {
    v[i] = new CmplxVector(1);
    elem(i,0) = vec[i];
  }  
}

CmplxMatrix::CmplxMatrix(const Matrix& p) { 
 
  d_i = p.dim_i();
  d_j = p.dim_j();
    
  if (d_i > 0) { 
    v = new CmplxVector*[d_i];
    for (int i = 0; i < d_i; i++) v[i] = new CmplxVector(p[i]); 
  }
  else v = NULL;
}

CmplxMatrix::CmplxMatrix(const Vector& vec) {
  
  d_i = vec.dim();
  d_j = 1;
  v = new CmplxVector*[d_i];
  for(int i = 0; i < d_i; i++) {
    v[i] = new CmplxVector(1);
    elem(i,0) = Complex(vec[i],0.);
  }    
}

CmplxMatrix::~CmplxMatrix()  
{ if (v) 
  { while(d_i--) delete v[d_i]; 
    delete v;
   }
}

/***************************** members ******************************/

/******************************************

void CmplxMatrix::free()
{
  if (v) {
    while(d_i--) delete v[d_i];
    delete v;
  }
}

void CmplxMatrix::check_dimensions(const CmplxMatrix& mat) const {

  if (d_i != mat.d_i || d_j != mat.d_j) {
    cerr << "incompatible complex matrix types." << endl;
    exit(1);
  }
}

void CmplxMatrix::flip_rows(int i,int j) {
 
  CmplxVector* p = v[i];
  v[i] = v[j];
  v[j] = p;
}

*************************************/

/*****************************************************

CmplxMatrix& CmplxMatrix::operator=(const CmplxMatrix& mat) {
  
  int i, j;
  if (d_i != mat.d_i || d_j != mat.d_j) {
    for(i = 0; i < d_i; i++) delete v[i];
    delete v;
    d_i = mat.d_i;
    d_j = mat.d_j;
    v = new CmplxVector*[d_i];
    for(i = 0; i < d_i; i++) v[i] = new CmplxVector(d_j);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) elem(i,j) = mat.elem(i,j);
  return (*this);
}

CmplxMatrix& CmplxMatrix::operator=(const Matrix& mat) {
  
  int i, j;
  CmplxMatrix tmp(mat);
  if (d_i != mat.dim_i() || d_j != mat.dim_j()) {
    for(i = 0; i < d_i; i++) delete v[i];
    delete v;
    d_i = mat.dim_i();
    d_j = mat.dim_j();
    v = new CmplxVector*[d_i];
    for(i = 0; i < d_i; i++) v[i] = new CmplxVector(d_j);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) elem(i,j) = tmp.elem(i,j);
  return (*this);
}

int CmplxMatrix::operator==(const CmplxMatrix& x) const {

  int i, j;
  if (d_i != x.d_i || d_j != x.d_j) return (0);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      if (elem(i,j) != x.elem(i,j)) return (0);
  return (1);
}

CmplxVector& CmplxMatrix::row(int i) const {

  if (i < 0 || i >= d_i) {  
    cerr << "complex matrix: row index out of range" << endl;
    exit(1);
  }
  return (*v[i]);
}

Complex& CmplxMatrix::operator()(int i, int j) {

  if (i < 0 || i >= d_i) { 
    cerr << "complex matrix: row index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) { 
    cerr << "complex matrix: col index out of range" << endl;
    exit(1);
  }
  return (elem(i,j));
}

Complex& CmplxMatrix::operator()(int i, int j) const {

  if (i < 0 || i >= d_i) { 
    cerr << "complex matrix: row index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) { 
    cerr << "complex matrix: col index out of range" << endl;
    exit(1);
  }
  return (elem(i,j));
}

CmplxVector CmplxMatrix::col(int i) const {
 
  if (i < 0 || i >= d_j) {  
    cerr << "complex matrix: col index out of range" << endl;
    exit(1);
  }
  CmplxVector result(d_i);
  int j = d_i;
  while (j--) result.v[j] = elem(j,i);
  return (result);
}

CmplxMatrix::operator CmplxVector() const {

  if (d_j != 1) { 
    cerr << "error: cannot make complex vector from complex matrix" << endl;
    exit(1);
  }
  return (col(0));
}

CmplxMatrix CmplxMatrix::operator+(const CmplxMatrix& mat) {

  int i, j;
  check_dimensions(mat);
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) + mat.elem(i,j);
  return (result);
}

CmplxMatrix CmplxMatrix::operator+(const Matrix& mat) {

  int i, j;
  if (d_i != mat.dim_i() || d_j != mat.dim_j()) {
    cerr << "incompatible complex matrix types. +" << endl;
    exit(1);
  }
  CmplxMatrix result(d_i,d_j);
  CmplxMatrix tmp(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) + tmp.elem(i,j);
  return (result);
}

CmplxMatrix CmplxMatrix::operator-(const CmplxMatrix& mat) {

  int i, j;
  check_dimensions(mat);
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) - mat.elem(i,j);
  return (result);
}

CmplxMatrix CmplxMatrix::operator-(const Matrix& mat) {

  int i, j;
  if (d_i != mat.dim_i() || d_j != mat.dim_j()) {
    cerr << "incompatible complex matrix types in -." << endl;
    exit(1);
  }
  CmplxMatrix result(d_i,d_j);
  CmplxMatrix tmp(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) - tmp.elem(i,j);
  return (result);
}

CmplxMatrix& CmplxMatrix::operator+=(const CmplxMatrix& mat) {

  int i, j;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) += mat.elem(i,j);
  return (*this);
}

CmplxMatrix& CmplxMatrix::operator-=(const CmplxMatrix& mat) {

  int i, j;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) -= mat.elem(i,j);
  return (*this);
}

CmplxMatrix CmplxMatrix::operator-() {

  int i, j;
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = -elem(i,j);
  return (result);
}

CmplxMatrix CmplxMatrix::operator*(double f) {

  int i, j;
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) * f;
  return (result);
}

CmplxMatrix CmplxMatrix::operator*(const Complex& f) {

  int i, j;
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) * f;
  return (result);
}

CmplxMatrix CmplxMatrix::operator*(const Matrix& mat) {

  if (d_j != mat.dim_i()) {
    cerr << "complex matrix: incompatible matrix types" << endl;
    exit(1);
  }
  CmplxMatrix result(d_i, mat.dim_j());
  int i, j;
  for(i = 0;i < mat.dim_j(); i++)
    for(j = 0; j < d_i; j++) result.elem(j,i) = *v[j] * mat.col(i);
  return (result);
}

CmplxMatrix CmplxMatrix::operator*(const CmplxMatrix& mat) {

  if (d_j != mat.d_i) {
    cerr << "complex matrix: incompatible matrix types" << endl;
    exit(1);
  }
  CmplxMatrix result(d_i, mat.d_j);
  int i,j;
  for(i = 0; i < mat.d_j; i++)
    for(j = 0; j < d_i; j++) result.elem(j,i) = *v[j] * mat.col(i);
  return (result);
}

CmplxMatrix CmplxMatrix::operator/(double a) {
 
  if (a==0) {
    cerr << "complex matrix: divided by zero" << endl;
    exit(1);
  }
  a = 1. / a;
  return (CmplxMatrix(*this * a));
}

CmplxMatrix CmplxMatrix::operator/(const Complex& a) {

  if (a==0) { 
    cerr << "complex matrix:  divided by zero" << endl;
    exit(1);
  }
  Complex tmp = 1. / a;
  return (CmplxMatrix(*this * a));
}
*******************************/


CmplxMatrix CmplxMatrix::trans() const {

  CmplxMatrix result(d_j,d_i);
  for(int i = 0; i < d_j; i++)
    for(int j = 0; j < d_i; j++)
      result.elem(i,j) = elem(j,i);
  return (result);
}

Matrix CmplxMatrix::real() const {

  Matrix result(d_i,d_j);
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      result(i,j)=::real(elem(i,j));
  return (result);
}

Matrix CmplxMatrix::imag() const {

  Matrix result(d_i,d_j);
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      result(i,j)=::imag(elem(i,j));
  return (result);
}

Matrix CmplxMatrix::abs() const {

  Matrix result(d_i,d_j);
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      result(i,j)=::cabs(elem(i,j));
  return (result);
}

CmplxMatrix CmplxMatrix::conjg() const {

  CmplxMatrix result(d_i,d_j);
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      result(i,j)=::conjg(elem(i,j));
  return (result);
}
     
CmplxMatrix& CmplxMatrix::resize(int new_d_i, int new_d_j) {
  int i, j;
  if (d_i != new_d_i || d_j != new_d_j) {
    for(i = 0; i < d_i; i++) delete v[i];
      delete v;
      d_i = new_d_i;
      d_j = new_d_j;
      v = new CmplxVector*[d_i];
      for(i = 0; i < d_i; i++) v[i] = new CmplxVector(d_j);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) elem(i,j) = 0.0;
  return (*this);
}

/************************** friends ************************/

ostream& operator<<(ostream& out, const CmplxMatrix& M) {

  //int i;
  //for (i = 0; i < M.d_i; i++) out << M[i] ; 

  int i,j;
  for(j=0; j < M.d_j; j++)
     for(i=0; i < M.d_i; i++)
       out << " [" << i << "][" << j << "]=" << M[i][j] << endl;   
  
  return (out);
}

istream& operator>>(istream& in, CmplxMatrix& M) {

  int i=0;
  while (i < M.d_i && in >> M[i++]);
  return (in);
}


