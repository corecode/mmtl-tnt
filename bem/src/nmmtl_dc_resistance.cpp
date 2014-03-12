///////////////////////////////////////////////////////////////////
// file: nmmtl_dc_resistance
//
//
//
//////////////////////////////////////////////////////////////////

#include "nmmtl.h"

/////////////////////////////////////////////////////////////////
//
// calculate the area for a polygon, this formula is derived from
// the surface integral converted to a line integral
////////////////////////////////////////////////////////////////
static float nmmtlPolyArea(CONTOURS_P item)
{
  POLYPOINTS *point;
  float area;
  float x0;
  float y0;
  
  area = 0;
  point = item->points;
  x0 = point->x;
  y0 = point->y;
  point = point->next;
  while (point != NULL)
    {
      area += x0 * point->y - y0 * point->x;
      x0 = point->x;
      y0 = point->y;
      point = point->next;
    }
  return fabs(area/2.0);
}
  
void nmmtl_dc_resistance(float the_conductivity,
			 CONTOURS_P signals, 
			 float **Rdc,
			 FILE *fp1,
			 FILE *fp2)
{
  CONTOURS_P item;
  CONTOURS_P sig_line1;
  CONTOURS_P sig_line2;;
  float area;
  int i;
  int j;
  FILE *output_fp;
  
  float conductivity = the_conductivity;

  for (i =0,sig_line1 = signals; 
       sig_line1 != NULL; 
       i++,sig_line1 = sig_line1->next)
    for (j=0,sig_line2 = signals; 
	 sig_line2 != NULL; 
	 j++,sig_line2 = sig_line2->next)
      Rdc[i][j] = 0.0;

  j = 0;
  item = signals;
  for (item = signals; item != NULL; item = item->next)
    {
      if ( item->conductivity != 0.0 )
	conductivity = item->conductivity;

      switch (item->primitive)
	{
	case RECTANGLE:
	  area = (item->x1 - item->x0) * (item->y1 - item->y0);
	  break;
	case CIRCLE:
	  area = PI * item->x1 * item->x1;
	  break;
	case POLYGON:
	  area = nmmtlPolyArea(item);
	  break;
	}
      Rdc[j][j] = 1.0 / (conductivity * area);
      j++;
    }

  // write it out //

  if (fp1 != NULL)
    output_fp =fp1;
  else
    output_fp = fp2;
  while (output_fp != 0)
    {
      /********************************************************************
       *                                                                   *
       * Output Rdc matrix [Rdc].					*
       *                                                                   *
       ********************************************************************/
      fprintf(output_fp, "\nRdc:\n");
      fprintf(output_fp, "Rdc(Active Signal , Passive Signal) Ohms/Meter\n");
      for (sig_line1 = signals, i = 0;
	   sig_line1 != NULL;
	   sig_line1 = sig_line1->next, i++)
	{
	  for (sig_line2 = signals, j = 0;
	       sig_line2 != NULL;
	       sig_line2 = sig_line2->next, j++)
	    {
	      fprintf(output_fp, "Rdc( ::%s , ::%s )= %15.7e\n",
		      sig_line1->name, sig_line2->name, Rdc[i][j]);
	    }
	}
      if (output_fp != fp2)
	output_fp = fp2;
      else
	output_fp = 0;
    }
}










