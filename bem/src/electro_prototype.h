#ifndef electro_prototype_h
#define electro_prototype_h

//
//
// Common prototypes from the electro library routines
//
//

//
// Checks to see if the directory and node name were specified on the
// command line and if not prompts for them.
//
// int  argc       argument count from command line of main
// char *argv[]   argument vector from command line of main
// char *dir   directory
// char *node  node name
// boolean new_allowed TRUE == allow a new node name to be returned
//                     does not verify if okay or not okay
//
int electro_get_dir_and_node(int argc, char *argv[], 
                                         char *dir, char *node,
					 boolean new_allowed = FALSE);
//
// Reads the pertinent information from the Icon section of the
// graphic file and places it in a structure.  Also verifies that
// all pins have names and the IOTYPE of bidirectional. If the Icon
// does not exist, a flag is set in the netlist structure, a warning
// is given and gendata continues.
//
// FILE *fp;             graphic input file
// NETLIST_REC *netlist; netlist structure
// FILE *err;            error output file
//
//int electro_read_icon_section(FILE *fp, NETLIST_REC *netlist, FILE *err);
//
// Outputs header information on electro data files.
// Date Time User Program V# dir node.extension     
//
// FILE *fptr;        /* output file on which to print a header
// char *program,     /* name of the program that creates this file
// *ext;         /* the file extension of the output file
// 
//void header_c(FILE *fptr,char * program, char * ext);


//
// This sample subroutine converts a from string (consisting of a floating
// point number and possibly a scale factor and units) to a to string (consisting
// only of units.
//
int conversion(char *from_string, char *to_string, double &scaled_number);
//
//
//  get_latest_mmtl_file()
// 
//  This function will determine the latest dated (if any) of all
//  the possible mmtl results files.
//
//  This function was extracted from code in Jeff P's read_mmtl_network_file.cxx
//  Maybe, this function could be transferred back there sometime, so
//  do not have to support same functionality in 2 different places.
//
// Possible return values of mmtl_file_to_use:
// (calling function needs to allocation the memory for the character string)
//
//   "no_mmtl_file_found"
//   "error_never_assigned_value"
//   "electro_results_2dlf"
//   "electro_results_2dly"
//   "nmmtl"

// Could return magicad message in future
void get_latest_mmtl_file(char *mmtl_file_to_use,
			  int verbose = FALSE);


#endif
