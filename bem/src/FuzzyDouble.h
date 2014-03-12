#ifndef FuzzyDouble_h
#define FuzzyDouble_h

//@M///////////////////////////////////////////////////////////////////////////
//
//  Module Name           FuzzyDouble.h
//  Description           The include file for the FuzzyDouble class.
//  Author                David O. Rowlands
//  Creation Date         April 30, 1997
//
//  Copyright (C) by Mayo Foundation for Medical Education and Research.  
//  All rights reserved.
//
//M@///////////////////////////////////////////////////////////////////////////

#include "float.h"
#include "math.h"

static const double EPSILON_C     = 10 * DBL_EPSILON;
static const double NEG_EPSILON_C = -EPSILON_C;

//@C///////////////////////////////////////////////////////////////////////////
//
//  Class Name            FuzzyDouble
//  Description           This class was mainly created to take care of
//                        floating point (in)equality headaches.  The 
//                        conversion method allows mathematical operations
//                        to function properly when using mixed doubles and 
//                        FuzzyDoubles.
//
//C@///////////////////////////////////////////////////////////////////////////

class FuzzyDouble
{

public:

  ///////////////
  // Constructor
  ///////////////
  FuzzyDouble() { }

  /////////////////////////
  // Alternate Constructor
  /////////////////////////
  FuzzyDouble(double value) 
  {  d_data = value;  }

  //////////////
  // Destructor
  //////////////
  ~FuzzyDouble() { }

  ////////////////////
  // Conversions
  ////////////////////
  operator double() const
  {  return d_data;  }
 
  /////////
  // Sets
  /////////
  void operator = (const FuzzyDouble& right)
  {  d_data = right.data();  }

  void operator = (const double right)
  {  d_data = right;  }

  /////////////////////////
  // Equality Operations
  /////////////////////////
  int operator == (const FuzzyDouble& right) const      // EQUAL
  { 
    double epsilon = selectEpsilon(right); 
    return(!((right.data() > (d_data + epsilon)) || 
             (right.data() < (d_data - epsilon))));
  }

  int operator != (const FuzzyDouble& right) const      // NOT EQUAL
  {    
    double epsilon = selectEpsilon(right); 
    return(((right.data() > (d_data + epsilon)) || 
            (right.data() < (d_data - epsilon))));
  }

  int operator < (const FuzzyDouble& right) const       // LESS
  {  
    double epsilon = selectEpsilon(right);
    return (right.data() > (d_data + epsilon));
  }

  int operator <= (const FuzzyDouble& right) const      // LESS or EQUAL
  {  
    double epsilon = selectEpsilon(right); 
    return (right.data() >= (d_data - epsilon));
  }

  int operator > (const FuzzyDouble& right) const       // GREATER
  {
    double epsilon = selectEpsilon(right); 
    return (right.data() < (d_data - epsilon));
  }

  int operator >= (const FuzzyDouble& right) const      // GREATER or EQUAL
  {
    double epsilon = selectEpsilon(right); 
    return (right.data() <= (d_data + epsilon));
  }

  ////////////////////
  // Gets
  ////////////////////
  double data() const
  {  return d_data;  }

private:

  inline double selectEpsilon(const FuzzyDouble value) const;

  double d_data; 

};


//@F///////////////////////////////////////////////////////////////////////////
//
//  Function Name         FuzzyDouble::selectEpsilon
//  Description           Selects the epsilon used for comparisans.
//  Formal Arguments      const FuzzyDouble value - is compared to this.
//  Return Value          double epsilon         - the selected epsilon. 
//
//F@///////////////////////////////////////////////////////////////////////////
inline double FuzzyDouble::selectEpsilon(const FuzzyDouble value) const
  {
    double epsilon1;
    double epsilon2;
    double epsilon;

    if(d_data < 0)                              // Normalize 1st epsilon
      epsilon1 = NEG_EPSILON_C * d_data;
    else  
      epsilon1 = EPSILON_C * d_data;

    if(value.data() < 0)                        // Normalize 2nd epsilon
      epsilon2 = NEG_EPSILON_C * value.data();
    else  
      epsilon2 = EPSILON_C * value.data();

    if(epsilon1 < epsilon2)                     // Select smaller epsilon
      epsilon = epsilon1;
    else
      epsilon = epsilon2;  

    return epsilon;
  }

#endif


