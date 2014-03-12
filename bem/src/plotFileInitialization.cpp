#include "nmmtl.h"
/****************************************************************
* plotFileInitialization
*
*  call this routine to write the first part of the data to the plotFile
*  after calling nmmtl_parse_graphic
*
*
*/

void plotFileInitialization(FILE *plotFile,
			    int units,
			    int gnd_planes,
			    float top_ground_plane_thickness,
			    float bottom_ground_plane_thickness
			  )
{
  if (plotFile != 0)
    {
      switch(units)
	{
	case UNITS_MILS :
	  fprintf(plotFile,"Display Units: mils\n");
	  break;
	case UNITS_MICRONS :
	  fprintf(plotFile,"Display Units: microns\n");
	  break;
	case UNITS_INCHES :
	  fprintf(plotFile,"Display Units: inches\n");
	  break;
	case UNITS_METERS :
	  fprintf(plotFile,"Display Units: meters\n");
	  break;
	case UNITS_NO_UNITS :
	default :
	  fprintf(plotFile,"Display Units: mils\n");
	  break;
	}
      
      
      fprintf(plotFile,"Lower Ground Plane Thickness: %e\n",
	      bottom_ground_plane_thickness);
      if (gnd_planes == 2)
	{
	  fprintf(plotFile,"Top Ground Plane Thickness: %e\n",
		  top_ground_plane_thickness);
	}
    }
  
}

