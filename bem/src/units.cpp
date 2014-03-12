//@M///////////////////////////////////////////////////////////////////////////
//
//  Module Name           units.cxx
//  Description       
//  Author            
//  Creation Date         
//
//  Copyright (C) by Mayo Foundation for Medical Education and Research.  
//  All rights reserved.
//
static const char rcsid[] = 
"$Id: units.cpp,v 1.1 2002/10/01 19:03:38 zahn Exp $";
//
//M@///////////////////////////////////////////////////////////////////////////

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<limits.h>


#define VERSION "1.0"

#define TRUE 1
#define FALSE 0
#define SUCCESS 1
#define FAIL 0

#define MAXUNITS 1000
#define MAXPREFIXES 50

#define MAXSUBUNITS 500

#define PRIMITIVECHAR '!'

static const char *powerstring="^";

static struct
  {
  const char *uname;
  const char *uval;
  } unittable[MAXUNITS];

struct unittype
  {
  char *numerator[MAXSUBUNITS];
  char *denominator[MAXSUBUNITS];
  double factor;
  };

static struct
  {
  const char *prefixname;
  const char *prefixval;
  } prefixtable[MAXPREFIXES];


static char *NULLUNIT=(char *)"";

static int unitcount=0;
static int prefixcount=0;

void setunits(void);


char *dupstr(const char *str)
{
  char *ret;
  ret = (char*)malloc(strlen(str)+1);
  if (!ret)
    {
      fprintf(stderr,"Memory allocation error\n");
      exit(3);
    }
  strcpy(ret,str);
  return(ret);
} 


void setunits(void)
{

  unittable[0].uname = "m";
  unittable[0].uval = "!a!";
  unittable[1].uname = "kg";
  unittable[1].uval = "!b!";
  unittable[2].uname = "sec";
  unittable[2].uval = "!c!";
  unittable[3].uname = "coul";
  unittable[3].uval = "!d!";
  unittable[4].uname = "candela";
  unittable[4].uval = "!e!";
  unittable[5].uname = "dollar";
  unittable[5].uval = "!f!";
  unittable[6].uname = "bit";
  unittable[6].uval = "!h!";
  unittable[7].uname = "erlang";
  unittable[7].uval = "!i!";
  unittable[8].uname = "K";
  unittable[8].uval = "!j!";
  prefixtable[0].prefixname = "yotta";
  prefixtable[0].prefixval = "1e24";
  prefixtable[1].prefixname = "zetta";
  prefixtable[1].prefixval = "1e21";
  prefixtable[2].prefixname = "exa";
  prefixtable[2].prefixval = "1e18";
  prefixtable[3].prefixname = "peta";
  prefixtable[3].prefixval = "1e15";
  prefixtable[4].prefixname = "tera";
  prefixtable[4].prefixval = "1e12";
  prefixtable[5].prefixname = "giga";
  prefixtable[5].prefixval = "1e9";
  prefixtable[6].prefixname = "Giga";
  prefixtable[6].prefixval = "1e9";
  prefixtable[7].prefixname = "mega";
  prefixtable[7].prefixval = "1e6";
  prefixtable[8].prefixname = "Meg";
  prefixtable[8].prefixval = "1e6";
  prefixtable[9].prefixname = "Mega";
  prefixtable[9].prefixval = "1e6";
  prefixtable[10].prefixname = "myria";
  prefixtable[10].prefixval = "1e4";
  prefixtable[11].prefixname = "kilo";
  prefixtable[11].prefixval = "1e3";
  prefixtable[12].prefixname = "hecto";
  prefixtable[12].prefixval = "1e2";
  prefixtable[13].prefixname = "deka";
  prefixtable[13].prefixval = "1e1";
  prefixtable[14].prefixname = "deci";
  prefixtable[14].prefixval = "1e-1";
  prefixtable[15].prefixname = "centi";
  prefixtable[15].prefixval = "1e-2";
  prefixtable[16].prefixname = "milli";
  prefixtable[16].prefixval = "1e-3";
  prefixtable[17].prefixname = "micro";
  prefixtable[17].prefixval = "1e-6";
  prefixtable[18].prefixname = "nano";
  prefixtable[18].prefixval = "1e-9";
  prefixtable[19].prefixname = "pico";
  prefixtable[19].prefixval = "1e-12";
  prefixtable[20].prefixname = "femto";
  prefixtable[20].prefixval = "1e-15";
  prefixtable[21].prefixname = "atto";
  prefixtable[21].prefixval = "1e-18";
  prefixtable[22].prefixname = "zopto";
  prefixtable[22].prefixval = "1e-21";
  prefixtable[23].prefixname = "yocto";
  prefixtable[23].prefixval = "1e-24";
  prefixtable[24].prefixname = "semi";
  prefixtable[24].prefixval = ".5";
  prefixtable[25].prefixname = "demi";
  prefixtable[25].prefixval = ".5";
  prefixtable[26].prefixname = "Y";
  prefixtable[26].prefixval = "yotta";
  prefixtable[27].prefixname = "Z";
  prefixtable[27].prefixval = "zetta";
  prefixtable[28].prefixname = "E";
  prefixtable[28].prefixval = "exa";
  prefixtable[29].prefixname = "P";
  prefixtable[29].prefixval = "peta";
  prefixtable[30].prefixname = "T";
  prefixtable[30].prefixval = "tera";
  prefixtable[31].prefixname = "G";
  prefixtable[31].prefixval = "giga";
  prefixtable[32].prefixname = "M";
  prefixtable[32].prefixval = "mega";
  prefixtable[33].prefixname = "S";
  prefixtable[33].prefixval = "siemen";
  prefixtable[34].prefixname = "k";
  prefixtable[34].prefixval = "kilo";
  prefixtable[35].prefixname = "h";
  prefixtable[35].prefixval = "hecto";
  prefixtable[36].prefixname = "da";
  prefixtable[36].prefixval = "deka";
  prefixtable[37].prefixname = "d";
  prefixtable[37].prefixval = "deci";
  prefixtable[38].prefixname = "c";
  prefixtable[38].prefixval = "centi";
  prefixtable[39].prefixname = "m";
  prefixtable[39].prefixval = "milli";
  prefixtable[40].prefixname = "u";
  prefixtable[40].prefixval = "micro";
  prefixtable[41].prefixname = "n";
  prefixtable[41].prefixval = "nano";
  prefixtable[42].prefixname = "p";
  prefixtable[42].prefixval = "pico";
  prefixtable[43].prefixname = "f";
  prefixtable[43].prefixval = "femto";
  prefixtable[44].prefixname = "a";
  prefixtable[44].prefixval = "atto";
  prefixtable[45].prefixname = "z";
  prefixtable[45].prefixval = "zopto";
  prefixtable[46].prefixname = "y";
  prefixtable[46].prefixval = "yocto";
  unittable[9].uname = "fuzz";
  unittable[9].uval = "1";
  unittable[10].uname = "pi";
  unittable[10].uval = "3.14159265358979323846";
  unittable[11].uname = "c";
  unittable[11].uval = "2.99792458e+8 m/sec fuzz";
  unittable[12].uname = "g";
  unittable[12].uval = "9.80665 m/sec2";
  unittable[13].uname = "au";
  unittable[13].uval = "1.49597871e+11 m fuzz";
  unittable[14].uname = "mole";
  unittable[14].uval = "6.022169e+23 fuzz";
  unittable[15].uname = "e";
  unittable[15].uval = "1.6021917e-19 coul fuzz";
  unittable[16].uname = "radian";
  unittable[16].uval = ".5 / pi";
  unittable[17].uname = "degree";
  unittable[17].uval = "1|180 pi-radian";
  unittable[18].uname = "circle";
  unittable[18].uval = "2 pi-radian";
  unittable[19].uname = "second";
  unittable[19].uval = "sec";
  unittable[20].uname = "s";
  unittable[20].uval = "sec";
  unittable[21].uname = "minute";
  unittable[21].uval = "60 sec";
  unittable[22].uname = "min";
  unittable[22].uval = "minute";
  unittable[23].uname = "hour";
  unittable[23].uval = "60 min";
  unittable[24].uname = "hr";
  unittable[24].uval = "hour";
  unittable[25].uname = "day";
  unittable[25].uval = "24 hr";
  unittable[26].uname = "da";
  unittable[26].uval = "day";
  unittable[27].uname = "week";
  unittable[27].uval = "7 day";
  unittable[28].uname = "year";
  unittable[28].uval = "365.24219879 day fuzz";
  unittable[29].uname = "yr";
  unittable[29].uval = "year";
  unittable[30].uname = "month";
  unittable[30].uval = "1|12 year";
  unittable[31].uname = "meter";
  unittable[31].uval = "m";
  unittable[32].uname = "cm";
  unittable[32].uval = "centimeter";
  unittable[33].uname = "CM";
  unittable[33].uval = "centimeter";
  unittable[34].uname = "mm";
  unittable[34].uval = "millimeter";
  unittable[35].uname = "km";
  unittable[35].uval = "kilometer";
  unittable[36].uname = "nm";
  unittable[36].uval = "nanometer";
  unittable[37].uname = "um";
  unittable[37].uval = "micrometer";
  unittable[38].uname = "micron";
  unittable[38].uval = "micrometer";
  unittable[39].uname = "angstrom";
  unittable[39].uval = "decinanometer";
  unittable[40].uname = "inch";
  unittable[40].uval = "2.54 cm";
  unittable[41].uname = "in";
  unittable[41].uval = "inch";
  unittable[42].uname = "IN";
  unittable[42].uval = "inch";
  unittable[43].uname = "foot";
  unittable[43].uval = "12 in";
  unittable[44].uname = "feet";
  unittable[44].uval = "foot";
  unittable[45].uname = "ft";
  unittable[45].uval = "foot";
  unittable[46].uname = "yard";
  unittable[46].uval = "3 ft";
  unittable[47].uname = "yd";
  unittable[47].uval = "yard";
  unittable[48].uname = "mil";
  unittable[48].uval = "1e-3 in";
  unittable[49].uname = "newton";
  unittable[49].uval = "kg-m/sec2";
  unittable[50].uname = "nt";
  unittable[50].uval = "newton";
  unittable[51].uname = "N";
  unittable[51].uval = "newton";
  unittable[52].uname = "joule";
  unittable[52].uval = "nt-m";
  unittable[53].uname = "cal";
  unittable[53].uval = "4.1868 joule";
  unittable[54].uname = "coulomb";
  unittable[54].uval = "coul";
  unittable[55].uname = "C";
  unittable[55].uval = "coul";
  unittable[56].uname = "ampere";
  unittable[56].uval = "coul/sec";
  unittable[57].uname = "amp";
  unittable[57].uval = "ampere";
  unittable[58].uname = "A";
  unittable[58].uval = "ampere";
  unittable[59].uname = "watt";
  unittable[59].uval = "joule/sec";
  unittable[60].uname = "volt";
  unittable[60].uval = "watt/amp";
  unittable[61].uname = "V";
  unittable[61].uval = "volt";
  unittable[62].uname = "ohm";
  unittable[62].uval = "volt/amp";
  unittable[63].uname = "Ohm";
  unittable[63].uval = "volt/amp";
  unittable[64].uname = "kilohm";
  unittable[64].uval = "kiloohm";
  unittable[65].uname = "Megohm";
  unittable[65].uval = "megaohm";
  unittable[66].uname = "megohm";
  unittable[66].uval = "megaohm";
  unittable[67].uname = "mho";
  unittable[67].uval = "/ohm";
  unittable[68].uname = "siemen";
  unittable[68].uval = "/ohm";
  unittable[69].uname = "Siemen";
  unittable[69].uval = "/ohm";
  unittable[70].uname = "farad";
  unittable[70].uval = "coul/volt";
  unittable[71].uname = "Farad";
  unittable[71].uval = "coul/volt";
  unittable[72].uname = "F";
  unittable[72].uval = "farad";
  unittable[73].uname = "nf";
  unittable[73].uval = "nanofarad";
  unittable[74].uname = "pf";
  unittable[74].uval = "picofarad";
  unittable[75].uname = "ff";
  unittable[75].uval = "femtofarad";
  unittable[76].uname = "henry";
  unittable[76].uval = "sec2/farad";
  unittable[77].uname = "Henry";
  unittable[77].uval = "sec2/farad";
  unittable[78].uname = "H";
  unittable[78].uval = "henry";
  unittable[79].uname = "mh";
  unittable[79].uval = "millihenry";
  unittable[80].uname = "weber";
  unittable[80].uval = "volt-sec";
  unittable[81].uname = "maxwell";
  unittable[81].uval = "1e-8 weber";
  unittable[82].uname = "hertz";
  unittable[82].uval = "/sec";
  unittable[83].uname = "Hertz";
  unittable[83].uval = "/sec";
  unittable[84].uname = "Hz";
  unittable[84].uval = "hertz";
  unittable[85].uname = "kHz";
  unittable[85].uval = "kilohertz";
  unittable[86].uname = "GHz";
  unittable[86].uval = "gigahertz";
  unittable[87].uname = "MHz";
  unittable[87].uval = "megahertz";
  unittable[88].uname = "hz";
  unittable[88].uval = "/sec";
  unittable[89].uname = "khz";
  unittable[89].uval = "1e+3 /sec";
  unittable[90].uname = "mhz";
  unittable[90].uval = "1e+6 /sec";

 unitcount = 91;
 prefixcount = 47;
}


void initializeunit(struct unittype *theunit)
{
  theunit->factor=1.0;
  theunit->numerator[0]=theunit->denominator[0]=NULL;
}


int addsubunit(char *product[], char *toadd)
{
  char **ptr;
  
  for(ptr=product;*ptr && *ptr!=NULLUNIT;ptr++);
  if (ptr>=product+MAXSUBUNITS)
    {
      fprintf(stderr,"Memory overflow in unit reduction\n");
      return 1;
    }
  if (!*ptr) *(ptr+1)=0;
  *ptr=dupstr(toadd);
  return 0;
}


void showunit(struct unittype *theunit)
{
  char **ptr;
  int printedslash;
  int counter=1;
  
  printf("\t%.8g",theunit->factor);
  for(ptr=theunit->numerator;*ptr;ptr++)
      {
      if (ptr>theunit->numerator && **ptr &&
	  !strcmp(*ptr,*(ptr-1))) counter++;
      else
	  {
	  if (counter>1) printf("%s%d",powerstring,counter);
	  if (**ptr) printf(" %s",*ptr);
	  counter=1;
	  }
      }
  if (counter>1) printf("%s%d",powerstring,counter);
  counter=1;
  printedslash=0;
  for(ptr=theunit->denominator;*ptr;ptr++)
      {
      if (ptr>theunit->denominator && **ptr && 
	  !strcmp(*ptr,*(ptr-1))) counter++;
      else
	  {
	  if (counter>1) printf("%s%d",powerstring,counter);
	  if (**ptr)
	      { 
	      if (!printedslash) printf(" /");
	      printedslash=1;
	      printf(" %s",*ptr);
	      }
	  counter=1;
	  }
      }
  if (counter>1) printf("%s%d",powerstring,counter);
  printf("\n");
}


void zeroerror()
{
//  fprintf(stderr,"Unit reduces to zero\n");
  printf ("**** Units unspecified\n");
}   

//
//   Adds the specified string to the unit. 
//   Flip is 0 for adding normally, 1 for adding reciprocal.
// 
//   Returns 0 for successful addition, nonzero on error.
//

int addunit(struct unittype *theunit,const char *toadd,int flip)
{
  char *scratch,*savescr;
  char *item;
  char *divider,*subunit,*slash;
  int doingtop;
  
  savescr=scratch=dupstr(toadd);
  for(slash=scratch+1;*slash;slash++) 
    if (*slash=='-' && 
	(tolower(*(slash-1))!='e' || !strchr(".0123456789",*(slash+1))))
      *slash=' ';
  slash=strchr(scratch,'/');
  if (slash) *slash=0;
  doingtop=1;
  do
      {
      item=strtok(scratch," *\t\n/");
      while(item)
	  {
	  if (strchr("0123456789.",*item))
	      {
	      //
	      // Item is a number.
	      //
	      double num;
	      
	      divider=strchr(item,'|');
	      if (divider)
		  {
		  *divider=0;
		  num=atof(item);
		  if (!num)
		      {
		      //
		      // Allow zero values, but do not convert them.
		      //
//		      zeroerror();
//		      return 1;
		      return 0;
		      }
		  if (doingtop^flip) theunit->factor *= num;
		  else  theunit->factor /=num;
		  num=atof(divider+1);
		  if (!num)
		      {
		      //
		      // Allow zero values, but do not convert them.
		      //
//		      zeroerror();
//		      return 1;
		      return 0;
		      }
		  if (doingtop^flip) theunit->factor /= num;
		  else theunit->factor *= num;
		  }
	      else
		  {
		  num=atof(item);
		  if (!num)
		      {
		      //
		      // Allow zero values, but do not convert them.
		      //
//		      zeroerror();
//		      return 1;
		      return 0;
		      }
		  if (doingtop^flip) theunit->factor *= num;
		  else theunit->factor /= num;
		  }
	      }
	  else
	      {
	      //
	      // Item is not a number.
	      //
	      int repeat=1;
	      if (strchr("23456789",item[strlen(item)-1]))
		  {
		  repeat=item[strlen(item)-1]-'0';
		  item[strlen(item)-1]=0;
		  }
	      for(;repeat;repeat--)
		if (addsubunit(doingtop^flip?theunit->numerator:theunit->denominator,item))
		  return 1;
	      }
	  item=strtok(NULL," *\t/\n");
	  }
      doingtop--;
      if (slash)
	  {
	  scratch=slash+1;
	  }
      else doingtop--;
      } while (doingtop>=0 );
  free(savescr);
  return 0;
}


int compare(const void *item1, const void *item2)
{
  return strcmp(*(char **) item1, * (char **) item2);
}


void sortunit(struct unittype *theunit)
{
  char **ptr;
  int count;
  
  for(count=0,ptr=theunit->numerator;*ptr;ptr++,count++);
  qsort(theunit->numerator, count, sizeof(char *), compare);
  for(count=0,ptr=theunit->denominator;*ptr;ptr++,count++);
  qsort(theunit->denominator, count, sizeof(char *), compare);
}


void cancelunit(struct unittype *theunit)
{
  char **den,**num;
  int comp;
  
  den=theunit->denominator;
  num=theunit->numerator;
  
  while(*num && *den)
    {
      comp=strcmp(*den,*num);
      if (!comp)
	{
	  //      if (*den!=NULLUNIT) free(*den);
	  //	if (*num!=NULLUNIT) free(*num);
	  *den++=NULLUNIT;
	  *num++=NULLUNIT;
	}
      else if (comp<0) den++;
      else num++;
    }
}




//
//   Looks up the definition for the specified unit.  
//   Returns a pointer to the definition or a null pointer
//   if the specified unit does not appear in the units table.
//

static char buffer[100];  /* buffer for lookupunit answers with prefixes */

const char *lookupunit(char *unit)
  {
  int i;
  char *copy;
  
  for(i=0;i<unitcount;i++)
      {
      if (!strcmp(unittable[i].uname,unit)) return unittable[i].uval;
      }

  if (unit[strlen(unit)-1]=='^')
      {
      copy=dupstr(unit);
      copy[strlen(copy)-1]=0;
      for(i=0;i<unitcount;i++)
	  {
	  if (!strcmp(unittable[i].uname,copy))
	      {
	      strcpy(buffer,copy);
	      free(copy);
	      return buffer;
	      }
	  } 
      free(copy);
      }
  if (unit[strlen(unit)-1]=='s')
      {
      copy=dupstr(unit);
      copy[strlen(copy)-1]=0;
      for(i=0;i<unitcount;i++)
	  {
	  if (!strcmp(unittable[i].uname,copy))
	      {
	      strcpy(buffer,copy);
	      free(copy);
	      return buffer;
	      }
	  }
      if (copy[strlen(copy)-1]=='e')
	  {
	  copy[strlen(copy)-1]=0;
	  for(i=0;i<unitcount;i++)
	      {
	      if (!strcmp(unittable[i].uname,copy))
		  {
		  strcpy(buffer,copy);
		  free(copy);
		  return buffer;
		  }
	      }
	  }
      free(copy);
      }
  for(i=0;i<prefixcount;i++)
      {
      if (!strncmp(prefixtable[i].prefixname,unit,
		   strlen(prefixtable[i].prefixname)))
	  {
	  unit += strlen(prefixtable[i].prefixname);
	  if (!strlen(unit) || lookupunit(unit))
	      {
	      strcpy(buffer,prefixtable[i].prefixval);
	      strcat(buffer," ");
	      strcat(buffer,unit);
	      return buffer;
	      }
	  }
      }
  return 0;
  }



//
//   reduces a product of symbolic units to primitive units. 
//   The three low bits are used to return flags:
//
//     bit 0 (1) set on if reductions were performed without error.
//     bit 1 (2) set on if no reductions are performed.
//     bit 2 (4) set on if an unknown unit is discovered.
//


#define ERROR 4

int reduceproduct(struct unittype *theunit, int flip)
  {
  
  const char *toadd;
  char **product;
  int didsomething=2;
  
  if (flip) product=theunit->denominator;
  else product=theunit->numerator;

  for(;*product;product++)
      {
      for(;;)
	  {
	  if (!strlen(*product)) break;
	  toadd=lookupunit(*product);
	  if (!toadd)
	      {
	      char tempstring[PATH_MAX];
	      sprintf(tempstring, "Unknown units: %s", *product);
              printf ("%s\n", tempstring);
//	      printf("unknown unit '%s'\n",*product);
	      return ERROR;
	      }
	  if (strchr(toadd,PRIMITIVECHAR)) break;
	  didsomething=1;
	  if (*product!=NULLUNIT)
	      {
	      free (*product);
	      *product=NULLUNIT;
	      }
	  if (addunit(theunit, toadd, flip)) return ERROR;
	  }
      }
  return didsomething;
  }


// 
//   Reduces numerator and denominator of the specified unit.
//   Returns 0 on success, or 1 on unknown unit error.
//

int reduceunit(struct unittype *theunit)
  {
  int ret;
  ret=1;
  while(ret & 1)
      {
      ret = reduceproduct(theunit,0) | reduceproduct(theunit,1);
      if (ret & 4) return 1;
      }
  return 0;
  }


int compareproducts(char **one,char **two)
  {
  while(*one || *two)
      {
      if (!*one && *two!=NULLUNIT) return 1;
      if (!*two && *one!=NULLUNIT) return 1;
      if (*one==NULLUNIT) one++;
      else if (*two==NULLUNIT) two++;
      else if (strcmp(*one,*two)) return 1;
      else one++,two++;
      }
  return 0;
  }

//
// Return zero if units are compatible, nonzero otherwise
//

int compareunits(struct unittype *first, struct unittype *second)
  {
  return 
    compareproducts(first->numerator,second->numerator) ||
    compareproducts(first->denominator,second->denominator);
  }

// 
//   Reduces numerator and denominator of the specified unit.
//   Returns 0 on success, or 1 on unknown unit error.
//

int completereduce(struct unittype *unit)
  {
  if (reduceunit(unit)) return 1;
  sortunit(unit);
  cancelunit(unit);
  return 0;
  }


void showanswer(struct unittype *have, struct unittype *want)
  {
  if (compareunits(have,want))
      {
      printf("conformability error\n");
      showunit(have);
      showunit(want);
      }
  else
      {
      printf("\t* %.8g\n\t/ %.8g\n",have->factor/want->factor,
	     want->factor/have->factor);
      }
  }


void usage()
  {
  fprintf(stderr,"\nunits [-f unitsfile] [-q] [-v] [from-unit to-unit]\n");
  fprintf(stderr,"\n    -f specify units file\n");
  fprintf(stderr,"    -q supress prompting (quiet)\n");
  fprintf(stderr,"    -v print version number\n");
  exit(3);
  }



//@F///////////////////////////////////////////////////////////////////////////
//
//  Function Name         conversion
//  Description       
//
// This sample subroutine converts a "from string" (consisting of a floating
// point number (-13, 25, 1567.2e6) and possibly a scale factor (micro, nano, 
// pico) and units(meters, henries, seconds) to a to string (consisting
// only of scale factor and units).
//
//  Formal Arguments  
//  Return Value      
//
//F@///////////////////////////////////////////////////////////////////////////

int conversion(char *from_string, char *to_string, 
                                        double &scaled_number)
  {
  char *remove_all_spaces(char *string);
  char *local_from_string = strdup(from_string);
  char *havestr, *wantstr;
  struct unittype have, want;
  double number;
  int status;
  int i, j, unitspt;
  int negative_number=FALSE;

  //
  // Remove any spaces embedded in the from_string to facilitate parsing.
  // 
  remove_all_spaces(local_from_string);
  
  //
  // Read the ASCII units file for each conversion, which allows
  // more conversions to be added 'on the fly'.
  //
  if ( (unitcount==0) || (prefixcount==0) )
      {
      setunits();
      }

  if (local_from_string == NULL)
      {
      fprintf(stderr,"A string to convert from must be specified.");
      }
  else
      {
      //
      // Insist that the units be separated from the actual number and
      // copy verbatim the number into an internal string.  This technique
      // avoids any conversions possibly performed by a scanf.
      //
      havestr = new char[strlen(local_from_string)+2];
      //
      // If a negative sign is present at the beginning of the number, then 
      // strip it since the units converter cannot handle it.  Flag this as 
      // the case, and restore the negative sign later to the converted 
      // number.  
      //
      if(local_from_string[0] != '-')
	  {
          strcpy(havestr, local_from_string);
          }
      else
          {
          negative_number = TRUE;
          strcpy(havestr, &local_from_string[1]);
          }
      //
      // Scan backwards in the string until the first digit is found.
      // Any character at or before this point is assumed to be the physical 
      // number and any character after this point is assumed to be the units 
      // and scale factor.  In this way, '1.0ps', '1.0 ps', '1000.0fs', 
      // '1000.0 fs', '1e3fs', and '1e3 fs' are converted correctly.
      //
      j = strlen(havestr);
      unitspt = j;
      while ( !isdigit(havestr[unitspt]) )
	  {
          unitspt--;
          if(unitspt < 0)     // no physical number was provided.
              {
	      unitspt = j;        
              break; 
              }
          }
      // 
      // Insert a space after the physical number (before the units).
      //
      for(i = j; i > unitspt; --i)
	  {
          havestr[i+1] = havestr[i];
          }
      havestr[i+1] = ' ';
      }

  if (to_string == NULL)
      {
      fprintf(stderr,"A string to convert to must be specified.");
      }
  else
      {
      //
      // Allocate memory to work with this string internally.
      //
      wantstr = new char[strlen(to_string)+1];
      strcpy(wantstr, to_string);
      }
  //
  // Calculate the conversion factor.
  //
  initializeunit(&have);
  addunit(&have,havestr,0);
  if (completereduce(&have) == 0)
      {
      initializeunit(&want);
      addunit(&want,wantstr,0);
      if (completereduce(&want) == 0)
	  {
	  //
	  // For the simple case of a 0.0 value, the number is 0.0 in any 
	  // units.  No conversion is performed and the 0.0 value is returned.
	  //
	  if(1 == sscanf(havestr, "%lf", &number))
              {          
	      if (number == 0.0)
	          {
	          scaled_number = 0.0;
	          status = SUCCESS;
	          }
	      else
	          {
	          //
	          // Return the result.
	          //
	          if (negative_number == FALSE)
		      {
		      scaled_number = have.factor/want.factor;
		      }
	          else
		      {
		      scaled_number = -1.0*have.factor/want.factor;
		      }
	          status = SUCCESS;
	          }
              }
         else
	      {
	      //
	      // An error has occurred during the conversion.
     	      //
	      status = FAIL;
              }
	  }
      else
	  {
	  //
	  // An error has occurred during the conversion.
	  //
	  status = FAIL;
	  }
      }
  else
      {
      //
      // An error has occurred during the conversion.
      //
      status = FAIL;
      }
  //
  // Deallocate the local memory.
  //
  delete [] wantstr;
  delete [] havestr;
  delete [] local_from_string;
  //
  // Return the status to the calling routine.
  //
  return (status);
  }



