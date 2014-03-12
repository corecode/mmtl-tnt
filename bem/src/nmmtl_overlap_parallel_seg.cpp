
/*

FACILITY:  NMMTL

MODULE DESCRIPTION:

Contains the function nmmtl_overlap_parallel_seg

AUTHOR(S):

Kevin J. Buchs

CREATION DATE:    1-AUG-1991 11:54:33

COPYRIGHT:   Copyright (C) 1991,1992 by Mayo Foundation. All rights reserved.

*/


/*
*******************************************************************
**  INCLUDE FILES
*******************************************************************
*/

#include "nmmtl.h"

/*
*******************************************************************
**  STRUCTURE DECLARATIONS AND TYPE DEFINTIONS
*******************************************************************
*/
/*
*******************************************************************
**  MACRO DEFINITIONS
*******************************************************************
*/
/*
*******************************************************************
**  PREPROCESSOR CONSTANTS
*******************************************************************
*/
/*
*******************************************************************
**  GLOBALS
*******************************************************************
*/
/*
*******************************************************************
**  FUNCTION DECLARATIONS
*******************************************************************
*/
/*
*******************************************************************
**  FUNCTION DEFINITIONS
*******************************************************************
*/


/*

FUNCTION NAME:  nmmtl_overlap_parallel_seg


FUNCTIONAL DESCRIPTION:

Find any potential overlap between to parallel dielectric subsegments.
They will both be vertical or horizontal.  If overlap is found, then
the extents of this overlap and any overhang beyond the overlap are
returned.  Now perhaps there is a more clever way of coding this problem,
but I did not find one.  So this is sort of a brute force method.

What we know:

We have four points in space, 

      S1, E1 (start and end of segment 1)
      S2, E2 (start and end of segment 2)

For the purposes of these comments, these points are though of as either
the X coordinates or the Y coordinates, but not both.  We know that the
end point of the segment will have a larger coordinate value than the
start point.  And so the following inequalities are true:

S1 < E1   and   S2 < E2

Now, the situation may occur where some of these points are the same,
such that the total number of unique points is only three, or only two. 
If we ennumerate all the unique point combinations, and assume that no
two points are the same, we end up with:

Four unique points:

(1)   S1  E1  S2  E2
(2)   S1  S2  E1  E2
(3)   S1  S2  E2  E1
(4)   S2  E2  S1  E1
(5)   S2  S1  E1  E2
(6)   S2  S1  E2  E1

Now taking the above list and combining pairs of points which might be 
located at the same coordinate, we get a list of combinations of three
unique points (take the above 1-6, and merge pairs, removing those that
violate the inequalities above):

(7)   S1    E1S2  E2
(8)   S1S2  E1    E2
(9)   S1    S2E1  E2
(10)  S1    S2    E1E2
(11)  S1S2  E2    E1
(12)  S1    S2    E2E1
(13)  S2    E2S1  E1
(14)  S2S1  E1    E2
(15)  S2    S1    E1E2
(16)  S2S1  E2    E1
(17)  S2    S1E2  E1
(18)  S2    S1    E2E1

E1S2 represents a common point, the end point of segment 1 and  start
point of segment 2.  Because order on a common point does not matter,
E1S2 is the same as S2E1.  So we find the following numbered states
from the above list are identical:

7,9
8,14
10,12
11,16
13,17
15,18

After removing the second of the identical pairs we are left with these
unqiue states:

  7, 8, 10, 11, 13, 15

These is one situation where we have two points which are common points:

(19)  S1S2 E1E2

Now we analyze the above states for possible overlap of segment 1 and 
segment 2.  These states do not have any overlap:  1, 4, 7, and 13.

For those states which have overlap, we can characterize the overlap by
which segment extends further than the other on the left and on the
right side.  This is termed overhang.  This is what kind of overhang is
leftover for the various states:

left overhang:

state    segment overhanging
2            1 
3            1 
5            2 
6            2 
10           1 
15           2

right overhang

state    segment overhanging
2            2 
3            1 
5            2 
6            1 
8            2 
11           1 

For the purposes of sorting this all out, we can assign a unique number
to each state based on the sorted order of the points.  We will break a
16 bit word into 4 nibbles (4 bits) and let each nibble represent a
unique point.  One or two nibbles may be zero if there are only three or
two unique points, respectively.  Within each nibble, let the following
numbers represent the segment start or end at that point:

1 = S1,   2 = S2,   4 = E1,   8 = E2

Here  is the map of states to hex codes:

1 = 1428
2 = 1248
3 = 1284
4 = 2814
5 = 2148
6 = 2184
7 = 0168
8 = 0348
10 = 012C
11 = 0384
13 = 0294
15 = 021C
19 = 003C

We will come up the hex code by sorting the points in ascending order. 
Once the code is computed, a switch statement to handle the appropriate
resulting overlap is executed.


FORMAL PARAMETERS:

struct dielectric_sub_segments *list1,
struct dielectric_sub_segments *list2,

             The two subsegments to be tested for overlap

double *overlap_left,
double *overlap_right,

             The returned region of overlap

int *left_overhang,
int *right_overhang

             The flags indicating if there is any overhang 
             beyond the region of overlap

RETURN VALUE:

0 = no overlap
1 = overlap
2 = error

CALLING SEQUENCE:

      overlap = nmmtl_overlap_parallel_seg(list1,list2,
                             &overlap_left,&overlap_right,
                             &left_overhang,&right_overhang);
*/


int nmmtl_overlap_parallel_seg(struct dielectric_sub_segments *list1,
                               struct dielectric_sub_segments *list2,
															 double *overlap_left, double *overlap_right,
                               int *left_overhang, int *right_overhang)
{

  int overlap = 0;
	double point[4],tempf;
  int pointcode[4],tempi,pts;
  unsigned int code;

  /* first, unless the segments fall on the same line, there can
     be no overlap */

  if(list1->at == list2->at)
  {
    /* two arrays to be sorted are set up - one with the coordinate
       value for the point (point) and the second with the code value 
       for what that point represents (pointcode). */

    point[0] = list1->start;
    pointcode[0] = 1;
    point[1] = list1->end;
    pointcode[1] = 4;
    point[2] = list2->start;
    pointcode[2] = 2;
    point[3] = list2->end;
    pointcode[3] = 8;
    pts = 4;

    /* Now order the points based on the coordinate value.  We do this with
       bubble sort - expanded out here - what a lovely piece of code! */

    if(point[0] > point[1])
    {
      tempf = point[0];
      tempi = pointcode[0];
      point[0] = point[1];
      pointcode[0] = pointcode[1];
      point[1] = tempf;
      pointcode[1] = tempi;
    }
    if(point[1] > point[2])
    {
      tempf = point[1];
      tempi = pointcode[1];
      point[1] = point[2];
      pointcode[1] = pointcode[2];
      point[2] = tempf;
      pointcode[2] = tempi;
    }
    if(point[2] > point[3])
    {
      tempf = point[2];
      tempi = pointcode[2];
      point[2] = point[3];
      pointcode[2] = pointcode[3];
      point[3] = tempf;
      pointcode[3] = tempi;
    }

    /* second pass */
    if(point[0] > point[1])
    {
      tempf = point[0];
      tempi = pointcode[0];
      point[0] = point[1];
      pointcode[0] = pointcode[1];
      point[1] = tempf;
      pointcode[1] = tempi;
    }
    if(point[1] > point[2])
    {
      tempf = point[1];
      tempi = pointcode[1];
      point[1] = point[2];
      pointcode[1] = pointcode[2];
      point[2] = tempf;
      pointcode[2] = tempi;
    }

    /* third and last pass */
    if(point[0] > point[1])
    {
      tempf = point[0];
      tempi = pointcode[0];
      point[0] = point[1];
      pointcode[0] = pointcode[1];
      point[1] = tempf;
      pointcode[1] = tempi;
    }

    /* find any identical points.  Do a binary OR operation for the point
       code in the first array pair entry.  Shift all the entries toward the
       higher numbered array pairs - nulling out the ones not used.  */

    if(point[0] == point[1]) 
    {
      pointcode[1] |= pointcode[0]; 
      pointcode[0] = 0;
    }
    if(point[1] == point[2]) 
    { 
      pointcode[2] |= pointcode[1];
      point[1] = point[0];
      pointcode[1] = pointcode[0];
      pointcode[0] = 0; 
    } 
    if(point[2] == point[3]) 
    {
      pointcode[3] |= pointcode[2];
      point[2] = point[1];
      pointcode[2] = pointcode[1];
      point[1] = point[0];
      pointcode[1] = pointcode[0];
      pointcode[0] = 0;
    }

    /* make one single code word from the four point codes in order */
    code = pointcode[0] << 12 | pointcode[1] << 8 |
      pointcode[2] << 4 | pointcode[3];

    /* take action based on which state the points are in. */
    switch(code)
    {
    case 0x1428:
      overlap = 0; break;

    case 0x1248:
      overlap = 1;
      *overlap_left = list2->start;
      *left_overhang = 1;
      *overlap_right = list1->end;
      *right_overhang = 2;
      break;

    case 0x1284:
      overlap = 1;
      *overlap_left = list2->start;
      *left_overhang = 1;
      *overlap_right = list2->end;
      *right_overhang = 1;
      break;

    case 0x2814:
      overlap = 0; break;

    case 0x2148:
      overlap = 1;
      *overlap_left = list1->start;
      *left_overhang = 2;
      *overlap_right = list1->end;
      *right_overhang = 2;
      break;

    case 0x2184:
      overlap = 1;
      *overlap_left = list1->start;
      *left_overhang = 2;
      *overlap_right = list2->end;
      *right_overhang = 1;
      break;

    case 0x0168:
      overlap = 0; break;

    case 0x0348:
      overlap = 1;
      *overlap_left = list2->start;
      *left_overhang = 0;
      *overlap_right = list1->end;
      *right_overhang = 2;
      break;

    case 0x012C:
      overlap = 1;
      *overlap_left = list2->start;
      *left_overhang = 1;
      *overlap_right = list1->end;
      *right_overhang = 0;
      break;

    case 0x0384:
      overlap = 1;
      *overlap_left = list2->start;
      *left_overhang = 0;
      *overlap_right = list2->end;
      *right_overhang = 1;
      break;

    case 0x0294:
      overlap = 0; break;

    case 0x021C:
      overlap = 1;
      *overlap_left = list1->start;
      *left_overhang = 2;
      *overlap_right = list1->end;
      *right_overhang = 0;
      break;

    case 0x003C:
      overlap = 1;
      *overlap_left = list1->start;
      *left_overhang = 0;
      *overlap_right = list1->end;
      *right_overhang = 0;
      break;

    default:
      fprintf(stderr,"ELECTRO-F-INTERNAL Internal error Corrupt data structures when determining overlap of dielectric segments; illegal state for check of parallel segments\n");
      overlap = 2;
      break;

    }
  }
  return(overlap);
}


#ifdef BUILT_IN_SELF_TEST

void main()
{
  int loop;
  struct dielectric_sub_segments list1;
  struct dielectric_sub_segments list2;
  int overlap;
	double overlap_left,overlap_right;
  int left_overhang,right_overhang;

  struct x 
  {
		double s1;
		double e1;
		double s2;
		double e2;
  }        data[19] = {
  /* four unique points */
  { 1.0, 2.0, 3.0, 4.0 },
  { 1.0, 3.0, 2.0, 4.0 },
  { 1.0, 4.0, 2.0, 3.0 },
  { 3.0, 4.0, 1.0, 2.0 },
  { 2.0, 3.0, 1.0, 4.0 },
  { 2.0, 4.0, 1.0, 3.0 },
  /* three unique points */
  { 1.0, 2.0, 2.0, 3.0 },
  { 1.0, 2.0, 1.0, 3.0 },
  { 1.0, 2.0, 2.0, 3.0 },
  { 1.0, 3.0, 2.0, 3.0 },
  { 1.0, 3.0, 1.0, 2.0 },
  { 1.0, 3.0, 2.0, 3.0 },
  { 2.0, 3.0, 1.0, 2.0 },
  { 1.0, 2.0, 1.0, 3.0 },
  { 2.0, 3.0, 1.0, 3.0 },
  { 1.0, 3.0, 1.0, 2.0 },
  { 2.0, 3.0, 1.0, 2.0 },
  { 2.0, 3.0, 1.0, 3.0 },
  /* two unique points */
  { 1.0, 2.0, 1.0, 2.0 }
  };


struct y 
{
  int o;
	double lo,ro;
  int ol,or;
}           expected[19] =
  {
    /*A*/   {0,0,0,0,0},
    /*B*/   {1,2.0,3.0,1,2},
    /*C*/   {1,2.0,3.0,1,1},
    /*D*/   {0,0.0,0.0,0,0},
    /*E*/   {1,2.0,3.0,2,2},
    /*F*/   {1,2.0,3.0,2,1},
    /*1*/   {0,0.0,0.0,0,0},
    /*2*/   {1,1.0,2.0,0,2},
    /*1*/   {0,0.0,0.0,0,0},
    /*3*/   {1,2.0,3.0,1,0},
    /*4*/   {1,1.0,2.0,0,1},
    /*3*/   {1,2.0,3.0,1,0},
    /*5*/   {0,0.0,0.0,0,0},
    /*2*/   {1,1.0,2.0,0,2},
    /*6*/   {1,2.0,3.0,2,0},
    /*4*/   {1,1.0,2.0,0,1},
    /*5*/   {0,0.0,0.0,0,0},
    /*6*/   {1,2.0,3.0,2,0},
    /*G*/   {1,1.0,2.0,0,0}
	  }; 



/* for reference **************************
   typedef struct dielectric_sub_segments
   {
   struct dielectric_sub_segments *next;
	 double at;
	 double start, end;
	 double epsilon;
   int divisions;
   } DIELECTRIC_SUB_SEGMENTS;
   ******************************************/

  list1.at = 1.0;

  list2.at = 1.0;

  for(loop = 0; loop < 19; loop++)
  {
    list1.start = data[loop].s1;
    list1.end =   data[loop].e1;
    list2.start = data[loop].s2;
    list2.end =   data[loop].e2;
    
    overlap = nmmtl_overlap_parallel_seg(&list1,&list2,
					 &overlap_left,&overlap_right,
					 &left_overhang,&right_overhang);
    printf("loop: %d\n",loop);
    if(overlap != expected[loop].o)
      printf("  overlap failed\n");
    else
      printf("  overlap suceeded\n");
    
    if(overlap)
    {
      if(overlap_left != expected[loop].lo)
	printf("  overlap_left  failed,  wanted: %f, got: %f\n",
	       expected[loop].ol,overlap_left);
      if(overlap_right != expected[loop].ro)
	printf("  overlap_right failed,  wanted: %f, got: %f\n",
	       expected[loop].or,overlap_right);
      if(left_overhang != expected[loop].ol)
	printf("  left_overhang failed,  wanted: %d, got: %d\n",
	       expected[loop].lo,left_overhang);
      if(right_overhang != expected[loop].or)
	printf("  right_overhang failed, wanted: %d, got: %d\n",
	       expected[loop].ro,right_overhang);
    }
  }
}

#endif
