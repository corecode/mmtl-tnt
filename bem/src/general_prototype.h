#ifndef general_prototype_h
#define general_prototype_h

#include <sys/types.h>
#include <sys/stat.h>

class RWCString;
class istream;
//
// compare_times.cxx
//

int compare_times(char *file1, char *file2, int *which_one, int print);



//
// copy_file.cxx
//

int copy_file(char *oldfile, char *newfile);


//
// delete_trailing_spaces.cxx
//

char *delete_trailing_spaces(char *str);


//
// design_alias.cxx
//


//
// design_alias_test.cxx
//


//
// dim2.cxx
//


//
// dim3.cxx
//


//
// error_location.cxx
//


//
// file_access.cxx
//


//
// free2.cxx
//


//
// free3.cxx
//


//
// get_date.cxx
//

int get_date(char *date);


//
// get_design_dir.cxx is defined in magicad.h
//
//int get_design_dir(char *dir, const char *prompt, const char *def_dir,
//                               PFI test,
//			       const char *help_lib,
//			       const char *help_topic, 
//			       int help_flag);

//
// get_hardcopy_name.cxx
//

char *get_hardcopy_name();


//
// get_help.cxx
//

int get_help(const char *topic, const char *library, int flag);
int printhelp(char *, char *);
int printtopix(char *topix[], int, int);
int help1(char *, char *svec[], int, int, int, char *, int);
int takeit(char *, char *, char *, char *);
int matchv(char *, char *vec[], char *mx[]);
int fkoff(char *, char *, char *, char *, char *);
int makewvec(char *, char *rvec[], int);
int xref_matchv(char *, char *vec[], char *mx[]);
int do_xref(char *, char *, char *mx[], char *, int);
int do_one_xref(char *, char *, char *, char *, int);
int printlist(char *vec[], int);


//
// get_input.cxx
//
//int get_input(const char *prompt, const char *default_,char *dest,
//                          const char *hlp_topic,
//                          const char *hlp_libr, int prmpt_flg, PFI test,
//                          int maxlen,
//                          int window, int remove_spaces);

//
// get_input_tests.cxx
//
/* all input data is ok, even a NULL string */
int accept_all(char *data);
/* do not use e,?,b or NULL input with no default */
int disable_all(char *data);
/* disable the exit capability */	
int no_exit(char *data);
/* override the exit capability (e is valid input) */
int ok_exit(char *data);
/* disable the backup character */
int no_backup(char *data);
/* disable the list character (?) */
int no_list(char *data);
/* disable backup and exit others are OK */
int no_backup_no_exit_else_success(char *data);
 /* disable backup and exit others are FAIL*/
int no_backup_no_exit_else_fail(char *data);
/* allow NULL string input without a default */
int ok_null(char *data);
/* used for multi_line_input. NULL terminates */
int ok_null_no_backup(char *data);
/* "hit <return> to continue" */
int confirm(char *data);
/* "hit <return> to continue" */
int confirm2(char *data);
/* 1.06 allow only y/n (Y/N) responses */
int yes_no(char *str);
int gpge_path( char *str );
int gpge_path_ok_null(char *str);
int is_hex( char *string );
int ok_null_is_int( char *string );
int is_dir( char *string );
int is_node(char * string );
// redesigned overlay for get_input.  
int input_output_bidirectional(char * string);



//
// get_logical_name.cxx
//
char *get_logical_name(const char *name,int case_flag);


//
// get_node.cxx
//

int get_node (char *node,
			  const char *prompt,
			  char *dir,
			  int new_allowed,
			  const char *default_node,
			  const char    *help_lib,
			  const char    *help_topic,
			  int   help_flag,
			  PFPFI test_setup);

//
// get_outfile.cxx
//
int get_outfile(char *filename,FILE **fptr,char * tt_header);


//
// gi_test_simple_range.cxx
//

PFI gi_test_simple_range(int set_limit, int lo_limit, int hi_limit);
int gi_test_simple_range_test(char *string);
void gi_test_simple_range_get(int *high, int *low);





//
// glob.cxx
//
//int glob(const char *pattern,int  flags,int (*errfunc)(Char *,int),
//	 glob_t *pglob);
//void globfree(glob_t *pglob);

//
// in_string.cxx
//

int in_string(const char *string,const char *sub_string);

//
// is_ascii_file.cxx
//

int is_ascii_file(const char *file, const char *path = ".", int length = 1000);

//
// is_int.cxx
//

boolean is_int(char *istring);

//
// is_node_string.cxx
//

boolean is_node_string(const char *path);

//
// is_real.cxx
//

int is_real(char *string);

//
// is_wildcard_string.cxx
//

boolean is_wildcard_string(const char *string);

//
// list_node_funct.cxx
//

int list_node_funct(int number_of_nodes,const char **database_list);

//
// list_test.cxx
//


//
// magicad_alloc.cxx
//


//
// match.cxx
//

int match(char *wildcardstring,char *normalstring, boolean case_sensitive=FALSE);


//
// newer_timestamp.cxx
//
int newer_timestamp(struct stat *timestamp1, struct stat *timestamp2);


//
// node_database.cxx
//


//
// number_hash.cxx
//
int number_hash(int, int, int, int *, void **, int, 
			    int (*)(int, int) = 0);

//
// substitute.cxx
//
int substitute(char *line, char *search, char *replace, 
			   int case_sensitive = FALSE, int match_limit = 0);


//
// output.cxx
//

int cad_output (FILE *fptr,       /* optional output file */
			    int window_name,  /* The window name for GRAPHIC output */
			    int confirm_flag, /* flag for confirming in GRAPHIC  */
			    const char *fmt,  /* output format spec for printf */
			    ...               /* arguments for sprintf formatting */
			    );

int cad_output (const char *fmt,  /* output format spec for printf */
			    ...               /* arguments for sprintf formatting */
			    );

static 
int output_data (FILE *fptr,       /* optional output file  */
			     int window_name,  /* The window name for GRAPHIC output */
			     int confirm_flag, /* flag for confirming in GRAPHIC */
			     const char *output_string /*output format spec for printf*/
			     );

//
// plotters.cxx
//


//
// printers.cxx
//


//
// range_float.cxx
//

int range_float(char *value);


//
// range_int.cxx
//

int range_int(char *value);

//
// range_string.cxx
//

int range_string(char *value);

//
// remove_dir.cxx
//
int remove_dir(char *full_path);


//
// remove_leading_spaces.cxx
//

char *remove_leading_spaces( char *string );

//
// remove_outer_spaces.cxx
//

char *remove_outer_spaces(char *string );


//
// rename_file.cxx
//

int rename_file(char *oldfile,char *newfile);

//
// replace_newlines.cxx
//

void replace_newlines(char *str);

//
// search_dir.cxx
//

int search_dir(char *dir);

//
// special_input.cxx
//

boolean special_input(const char *str, char key);

//
// string_hash.cxx
//
int string_hash(int  mode,       /* defines operation to be perf */
			    char *key1,      /* first key to hash on         */
			    char *key2,      /* second key to hash on        */
			    void **attribute,/* val to associate with keys: int or ptr*/
			    void **table,   /* Address of the hash table    */
			    int  table_size, /* size of hash table (in bytes)*/
			    /* optional user-defined hash function*/
			    int  (*hash_func)(char *s1, char *s2));

//
// string_lower.cxx
//

char *string_lower (char *string);

//
// string_upper.cxx
//

char *string_upper (char *string);

//
// test_dir.cxx
//

int test_dir(char *dir);

//
// test_get_outfile.cxx
//


//
// test_help.cxx
//


//
// test_logical.cxx
//

boolean test_logical(const char *name );

//
// test_node.cxx
//

PFI test_node(char *directory, int new_allowed_flag);

//
// test_page.cxx
//

int test_page(char *opt);

//
// test_printers.cxx*
//


//
// text_list_window.cxx
//


//
// timestamp.cxx
//

int timestamp(char *file, char *access, struct stat *timbuf, int print);

//
// truncate_string.cxx
//

int truncate_string (char *string);


//
// Checks to see if the directory and node name were specified on the
// command line and if not prompts for them.
//
// int  argc       argument count from command line of main
// char *argv[]   argument vector from command line of main
// char *dir   directory (must be allocated in calling routine)
// char *node  node name (must be allocated in calling routine)
// boolean new_allowed TRUE == allow a new node name to be returned
//                     does not verify if okay or not okay
//
int get_design_dir_and_node(int argc, char *argv[], 
					char *dir, char *node,
					boolean new_allowed = FALSE);

//
// Converts the input argument to engineering notation.  The default
//           format is "%8.3f" and user-specified formats should be similar.
//
// ENVIRONMENT  char *toengineering(float number, char *units, char *format);
//              char *toengineering(double number, char *units, char *format);
//              char *toengineering(int number, char *units, char *format);
//
// INPUTS  float, double, or int
//         Optional units which are prefixed by the engineering 'size'
//         Optional character pointer to a printf style format string
//
// OUTPUTS Character pointer (NULL terminated) ready for printing
//
char *toengineering(double number, char *units = NULL, char *format = NULL);

long countPrecedingLines(istream &s);

int executeSerialProcess(const RWCString &executable,
				     const char *const argv[],
				     const char *const envp[]);

#endif
