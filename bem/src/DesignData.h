#ifndef DesignData_h
#define DesignData_h

#ifndef assert_h
#include <assert.h>
#endif

#ifndef node_database_h
#include "node_database.h"
#endif

/////////////////////////////////////////////////
// A class to create, modify or hold data
// from the d_database file
////////////////////////////////////////////////
enum AttributeType {default_attributes,em_attributes};

class DesignData
{

private:
  /////////////////////////////////////////////
  // Read Data from the static file list
  // in the MC_COMMON directory
  ///////////////////////////////////////////
  magicad_message readCommonStaticFiles();

  char *fullPath;           // the full path of the design
  char *path;               // the path excluding the design name
  char *name;               // the design alias name
  char *dir;                // the subdirectory defined as the design directory
  char *description;        // the description of the design
  char *creator;            // the creator of the design
  boolean history;          // the history collection flag
  int keep;                 // the number of old revisions to keep

  boolean newDesign;        // flag to indicate a new design
  AttributeType attributes;

public:
  //////////////////////////////////////////////////////////
  // these lists are made public to simplify the interface
  //////////////////////////////////////////////////////////

  RWTPtrDlist<FILE_NAME> commonStaticFiles;
  RWTPtrDlist<FILE_NAME> staticFiles;
  RWTPtrDlist<FILE_NAME> dynamicFiles;  
  RWTPtrDlist<Alias> aliases;
  const char *getFullPath() { return fullPath;}
  const char *getPath() {return path;}
  const char *getSubDirectory() {return dir;}
  const char *getName() {return name;}
  const char *getCreator() {return creator;}
  const char *getDescription() {return description;}
  boolean getHistory() {return history;}
  int getKeep() {return keep;}
  AttributeType getAttributeType() {return attributes;}
  void setAttributeType(AttributeType value) {attributes = value;}
  const char *attributeFileSpec();

  /////////////////////////////////
  // default ctor
  ////////////////////////////////
  DesignData();


  ////////////////////////////////////
  // Existing directory ctor
  // pass in the fullpath of the design
  /////////////////////////////////////
  DesignData(char *alias,char *pathSpecification);
  

  ///////////////////////////////////
  // set the path spec for the design
  //////////////////////////////////
  void setPath(const char *newPath)
    {
      //////////////////////////////////////////////////////
      // the path can only be set when creating a new design
      //////////////////////////////////////////////////////
      assert(newDesign);
      if (path != 0) delete [] path;
      path = new char[strlen(newPath) +1];
      strcpy(path,newPath);

      if (fullPath != 0) delete [] fullPath;
      if (dir != 0)
	{
	  fullPath = new char[strlen(path) + strlen(dir) +2];
	  strcpy(fullPath,path);
	  strcat(fullPath,"/");
	  strcat(fullPath,dir);
	}
    }
  ///////////////////////////////////
  // set the name, and possibly a seperate 
  // subdirectory name, for the design
  ////////////////////////////////////
  void setName(const char *newName,const char *newDir=0)
    {
      //////////////////////////////////////////////////
      // the name and directory can only be set when 
      // createing a new design
      ///////////////////////////////////////////////
      assert(newDesign);
      if (newDir ==0) newDir = newName;

      if (name != 0) delete [] name;
      name = new char[strlen(newName) +1];
      strcpy(name,newName);

      if (dir != 0) delete [] dir;
      dir = new char[strlen(newDir) +1];
      strcpy(dir,newDir);

      if (fullPath != 0) delete [] fullPath;
      if (path != 0)
	{
	  fullPath = new char[strlen(path) + strlen(dir) +2];
	  strcpy(fullPath,path);
	  strcat(fullPath,"/");
	  strcat(fullPath,dir);
	}
    }

  ///////////////////////////////////////////////
  //Set the description of the design
  ///////////////////////////////////////////////
  void setDescription(const char *newDescription)
    {
      if (description != 0) delete [] description;
      description = new char[strlen(newDescription) +1];
      strcpy(description,newDescription);
    }

  /////////////////////////////////////////////////
  // toggle the history flag
  ////////////////////////////////////////////////
  void toggleHistory() {history = (boolean) !history;}

  ////////////////////////////////////////////////////
  // Set the number of old versions to keep
  ////////////////////////////////////////////////////
  void setNumberToKeep(int value) {keep = value;}
  /////////////////////////////////////////////////////
  // save the changes to the design
  ///////////////////////////////////////////////////
  magicad_message save();
};

#endif


