#!/bin/sh
#---------------------------------------------------------------------
#
#  console_demo.tcl
#
#  Demonstrate console package in a little application.
#
#---------------------------------------------------------------------

#
#  Restart using wish \
exec wish "$0" "$@"


#---------------------------------------------------------------------
#  Create BWIDGET GUI to demonstrate
#  Most of this code is non-functional.
#---------------------------------------------------------------------

package require Tk
package require BWidget

# Menu description
set descmenu {
    "&File" all file 0 {
	{command "&New" {} "Create a new file" {Ctrl n}}
	{command "&Open" {} "Open existing file" {Ctrl o}}
	{command "&Save" {} "Save" {Ctrl s}}
	{command "Sa&ve_as" {} "Save file with new name" {}}
	{command "&Exit" {} "Quit" {Ctrl x}}
    }
    "View" all file 0 {
	{checkbutton "Console" {} "Show/hide command console" {} \
	     -variable globalvar}
	{command "Show Console" {} "Show command console" {} -command console::show}
	{command "Hide Console" {} "Hide command console" {} -command console::hide}
    }
    "&Help" all help 0 {
	{command "About" {} "" {}}
    }
}

#  Main window just contains a text widget
set f [MainFrame .mainframe -menu $descmenu]
pack [text [$f getframe].text]
pack $f -fill both -expand yes


#---------------------------------------------------------------------
#  Load the console package from the same directory as
#  this demo script.
#  Note that the "console" package name is lower case.
#---------------------------------------------------------------------
set auto_path [concat [file dirname [info script]] $auto_path]
package require console

#---------------------------------------------------------------------
#  Create the console, and attach it to our global variable
#---------------------------------------------------------------------
console::create -title "Command Console" -variable globalvar

# ... and we're done...