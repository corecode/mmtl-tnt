//@M///////////////////////////////////////////////////////////////////////////
//
//  Module Name           node_database.h
//
//  Description       
//
//    This C++ include file defines the classes that encapsulate
//    the data and functionality of the MagiCAD POSIX design databases.
//
//  Author            Andrew Staniszewski
//  Creation Date     5-April-1993
//
//  Copyright (C) by Mayo Foundation for Medical Education and Research.  
//  All rights reserved.
//
//
//M@///////////////////////////////////////////////////////////////////////////


#ifndef node_database_h
#define node_database_h

#ifndef magicad_h
#include "magicad.h"
#endif

#include <rw/tpdlist.h>
#include "tpdlistSortingFunctions.h"


// types of access allowed by the database
enum FILE_ACCESS {READ,WRITE,READ_USING_WORK,WRITE_USING_WORK,TEMPORARY};


//@C///////////////////////////////////////////////////////////////////////////
//
//  Class Name          FILE_NAME
//  
//  Description      
//
//    This class is essentially a type-safe file name string
//    with POSIX defined maximum length. 
//
//C@///////////////////////////////////////////////////////////////////////////
class FILE_NAME
{
  char name[FILENAME_MAX];
public:
  FILE_NAME() {name[0] = '\0';}
  FILE_NAME(const char *fname) {strcpy(name,fname);}
  FILE_NAME(const FILE_NAME &old) {strcpy(name,old.name);}
  operator char*() {return name;}
  friend ostream & operator << (ostream &s,const FILE_NAME &data)
  {
    s << endl << data.name << endl;
    return s;
  }
  boolean operator == (const FILE_NAME &inName)
    {
      if (strcmp(name,inName.name) == 0)
	return TRUE;
      else 
	return FALSE;
    }
  boolean operator > (const FILE_NAME &inName)
    {
      if (strcmp(name,inName.name) > 0)
	return TRUE;
      else
	return FALSE;
    }

};

//@F///////////////////////////////////////////////////////////////////////////
//
//  Function Name         file_access  - check for specific access to a file
//                        rename_file  - works even across file systems
//                        copy_file    - general file copy
//
//  Description   
//
//    These general purpose functions are used by the NODE_DATABASE
//    to simplify file management functions.
//    
//  Formal Arguments  
//
//    char *path              full POSIX path specifications to files
//    char *oldfile
//    char *newfile
//    FILE_ACCESS flag        MagiCAD specific file access type
//
//  Return Value    
//
//    magicad_message status = 
//          SUCCESS        operation was successful
//          mcms_nopriv    POSIX "EACCES" or requested access unavailable
//          mcms_openin    can't fopen() oldfile
//          mcms_reading   error reading oldfile
//          mcms_openout   can't fopen() newfile
//          mcms_writing   error writing newfile
//          mcms_invdirfmt POSIX "ENAMETOOLONG" or "ENOTDIR"
//          mcms_accfnf    POSIX "ENOENT"
//          mcms_isdir     is a POSIX directory, not a file
//          mcms_internal  POSIX "EBADF" or other internal error
//
//F@///////////////////////////////////////////////////////////////////////////
  
magicad_message file_access(char *path, FILE_ACCESS flag);

magicad_message rename_file(char *oldfile, char *newfile);

magicad_message copy_file(char *oldfile,char *newfile);


//@C///////////////////////////////////////////////////////////////////////////
//
//  Class Name            NODE_DATABASE_INSTANCE_RECORD
//
//  Description       
//
//    The node database instance record is a helper class to
//    NODE_DATABASE.  Since the MagiCAD database is hierarchical,
//    every node could have a number of instances of other nodes
//    as children.  The "instance record" appears in the database
//    file.
//
//C@///////////////////////////////////////////////////////////////////////////

class NODE_DATABASE_INSTANCE_RECORD
{
public:
  char name[FILENAME_MAX];
  char dir[FILENAME_MAX];
  char path[PATH_MAX];
  int references;
  int version;


  NODE_DATABASE_INSTANCE_RECORD(char *node_name,
				char *node_dir,
				char *node_path,
				int node_references,
				int node_version)
  : references(node_references), version(node_version)
  {
    strcpy(name,node_name);
    strcpy(dir,node_dir);
    strcpy(path,node_path);
  }
  friend ostream & operator << (ostream & s, 
				const NODE_DATABASE_INSTANCE_RECORD & data)
  {
    s << endl;
    s << "NODE_INSTANCE" << endl;
    s << "Node: " << data.name << endl;
    s << "Design Directory: " << data.dir << endl;
    s << "Design Path: " << data.path << endl;
    s << "Version: " << data.version << endl;
    s << "Number of References: " << data.references << endl;
    s << endl;
    return s;
  }
};


//@C///////////////////////////////////////////////////////////////////////////
//
//  Class Name            Alias   (Design Directory Alias)
//
//  Description       
//      An alias is just a short name for a (typically) long path
//      specification to a design directory.  The MagiCAD databases
//      keep track of a node's children (in a design hierarchy) in 
//      terms of the design aliases, so full path specifications
//      do not appear as part of design data.
//
//
//      The design Alias class is not typically used by itself,
//      but applications will create an instance of AliasList,
//      which includes logic about where the aliases are defined
//      by the MagiCAD system.
//

//C@///////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
//  Each Alias has a type: one of system, user or design directory.
//  User aliases can augment or supersede entries from the system
//  list.  Entries from the design directory database can augment or
//  supersede either user or system entries.
//  
/////////////////////////////////////////////////////////////////
typedef enum AliasType {
    SYSTEM_ALIAS, 
    USER_ALIAS, 
    DESIGN_ALIAS
} ALIAS_TYPE;

/////////////////////////////////////////////////////////////////
//
//  Define the Alias, which includes data and some trivial member
//  functions for creation, destruction, and access.
//
/////////////////////////////////////////////////////////////////

#define SIZE_ALIAS (SIZE_FILE_SPEC)

class Alias
{
  //	Private data
private:
  char	alias[SIZE_ALIAS] ;	// alias name
  char	path[PATH_MAX] ;	// associated path spec
  ALIAS_TYPE	type ;			// type of alias
  
  
    // Public Functions
public:
  
  //	    Constructors for all occasions
  Alias( char *alias, char *path, ALIAS_TYPE type ) ;
  Alias() ;
  
  //////////////////////////////////////////////////////////////
  //	    Access individual data fields
  //
  //		Strings are returned as pointers to a
  //		member-function specific static buffer, so the
  //		caller can just use the pointer and forget it.
  //		(no need to free() the character buffer)
  //////////////////////////////////////////////////////////////
  char *get_alias() ;
  char *get_path() ;
  ALIAS_TYPE get_type() ;	    // returns type of alias
  void setPath(const char *newPath) {strcpy(path,newPath);}
  
  //////////////////////////////////////////////////////////////
  //  The following ostream << operator friend function is required for
  //  the list class functionality.  The freelist definitions are found
  //  in the class definition (in the .cxx file) and the default copy
  //  constructor is sufficient.
  //////////////////////////////////////////////////////////////
  friend ostream & operator << (ostream & s, Alias & data)
    {
      s << data.alias << "  " << data.path ;
      s << endl ;
      return s;
    };

  boolean operator == (const Alias &inName)
    {
      if (strcmp(alias,inName.alias) == 0)
	return TRUE;
      else 
	return FALSE;
    }
  boolean operator > (const Alias &inName)
    {
      if (strcmp(alias,inName.alias) > 0)
	return TRUE;
      else
	return FALSE;
    }


};




//@C///////////////////////////////////////////////////////////////////////////
//
//  Class Name            NODE_DATABASE
//
//  Description       
//
//   This class and its myriad of member access functions provide the
//   access to the MagiCAD node and directory databases.
//
//C@///////////////////////////////////////////////////////////////////////////

class NODE_DATABASE
{
private:
// work data fields
  char name[FILENAME_MAX];
  char design_alias[FILENAME_MAX] ;
  char path[PATH_MAX];
  char username[32];
  char library[PATH_MAX];
  char floorplan[FILENAME_MAX];
  boolean update;
  boolean new_node;
  boolean writeOk;
  static RWTPtrDlist<FILE_NAME> common_static_files;  // default 

// data saved in the database
  char description[SIZE_COMMENT];
  char creator[SIZE_COMMENT];
  int version;
  int keep;
  boolean history;

  // This function copies files from one directory to another
  magicad_message copy_files(char *from_dir,char *to_dir);

  // This function opens and reads the common static file list
  // from MC_CAD_COMMON
  magicad_message load_common_statics();

  // performs the  necessary actions to initialize
  // a new node, the optional description will be used as the description
  // field in the new node. If it is not given the user is prompted for it
  magicad_message create_new_node(char *desc=0);

  // Delete a node or node sub directory.
  // The default is to delete the root node directory,
  // or you can specify a relative path from the root node directory.
  magicad_message delete_directory(char *relative_path=0);



public:
  // constructor
  // this will use the value of MC_NODE and MC_DESIGN to open the database
  // if node and path name are not given.  If new_allowed is TRUE, 
  // the database will allow a new node to be created.  If 
  // full_path_spec is true, then the path argument represents
  // a file system path spec to the node, instead of a design
  // directory alias name.  (only used when instantiating new nodes).
  // NOTE: new nodes are temporary until at least one save is 
  // performed. If a save is not performed the new node is deleted
  // when the database is deleted
  NODE_DATABASE(char *node = 0, char *path = 0,
		boolean new_allowed = FALSE, boolean full_path_spec = FALSE,
		char *new_node_description=0);


  // Copy constructor
  // this constructor is meant for use in copying TEMPORARY objects
  // to real objects in container classes. NOTE: use of this constructor
  // assumes that the original will not be used again before it is
  // destroyed
  NODE_DATABASE(const NODE_DATABASE &old);

  // destructor
  // This will complain if the database was altered but not saved.
  // It is also responsible for making certain new nodes are deleted
  // if they were not saved. The USER is RESPONSIBLE for making 
  // certain the destructor is NOT called until all files in the node
  // have been de-accessed, BUT IS called before the program terminates
  ~NODE_DATABASE();


// This function will register an access request for a cad file associated 
// with this node. NOTE: files accessed for write ALWAYS require a comment
// that MUST be at least 5 characters long 
// Comments are ignored for other types of access
  magicad_message access(char *fullpath,char *fname,
			 FILE_ACCESS type_of_access=READ,char *comment=NULL);

// This function will register an access request for a cad file associated 
// with this node, and returns an opened file ptr.
// NOTE: files accessed for write ALWAYS require a comment
// and comments are ignored for other types of access.
  magicad_message access_n_open(FILE **file_ptr,char *fname,
				FILE_ACCESS type_of_access=READ,
				char *comment=0);

// This function is meant close a file and to release the lock on a file 
// accessed thru access_n_open. As with access, comments are only
// used when the access_type was for write. unlike access,
// the comment is always optional
  magicad_message close_n_deaccess(FILE *file_ptr,char *fname,
				   FILE_ACCESS access_type,
				   char *comment=0);

// This will copy the current node to another location
// NOTE: not yet implemented
  magicad_message copy(char *node,char *path);

// This function is meant to release the lock on a file 
// accessed thru access. As with access, comments are only
// used when the access_type was for write. unlike access,
// the comment is always optional
  magicad_message deaccess(char *fname,FILE_ACCESS access_type,
			   char *comment=0);

// Function to determine if this is a new node
  boolean is_new_node() {return new_node;}

// This function returns the node name, design directory, and   
//  and full path spec to the design directory.
  void location(char *node_name, char *design=0,char *fullpath=0)
  {
    if (node_name != 0)
      strcpy(node_name,name);
    if (design != 0)
      strcpy(design,design_alias);
    if (fullpath != 0)
      {
	strcpy(fullpath,path);
	strcat(fullpath,"/");
	strcat(fullpath,name);
      }
  }

// This function allows the program to terminate without
// updating the database
// It assumes that a new node should have any associated files deleted
// Note, the database still exists (even for new nodes) and may
// still be used, saved etc, after a quit
  void quit();

// This function saves the changes made to the database
// If the new version flag is set, a comment can be supplied
// or the user is prompted for it
  //
  // Note: if save returns a BACKUP status,
  // this indicates that file privileges etc
  // would cause the save to fail,
  // but that 

  magicad_message save(boolean make_new_version = TRUE, 
		       char *new_version_comment = 0);


};



#endif
