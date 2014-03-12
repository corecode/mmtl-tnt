#----------------------------------------------------------------
#
#  mmtl.tcl
#
#  Main entry point for building the TNT
#  interface as part of the Xmission application.
#  Builds the GUI with a more horizontal layout,
#  within the specified frame.
#
#  The actual GUI is constructed by [createElements]
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved
#  $Id: gui_xmission.tcl,v 1.8 2004/07/20 14:51:55 techenti Exp $
#
#----------------------------------------------------------------
package require Itcl

package provide gui 2.0



##################################################################
#
# Create the main window.
#
##################################################################
proc ::gui::_mmtlCreateMain { parent argv} {

    set ::gui::mainframe [frame $parent.mainframe ]

    #-----------------------------------------------------------
    # Set flag that this will not be the mmtl gui interface.
    #-----------------------------------------------------------
    set mmtlInitialize 0

    #--------------------------------------------------
    # Set flag to not create the mmtl gui interface.
    #--------------------------------------------------
    set ::gui::f1 [::gui::guiCreateElements $::gui::mainframe $mmtlInitialize]

    pack $::gui::mainframe -expand false

    #-----------------------------------------------------------
    #  If there was a filename supplied on the command line,
    #  then open that file
    #-----------------------------------------------------------
    if { [llength $argv] > 0 } {
	set filename [lindex $argv 0]
	set ::gui::_nodename $filename
	::gui::guiOpenExisting $filename
    }
}


##################################################################
#
# _runAndSend
#
# Run the BEM simulation and send the retreive the data for 
#  xmission
#
##################################################################
proc ::gui::_runAndSend {} {

    #-------------------------------------------------------------
    # Run the BEM simulation.
    #-------------------------------------------------------------
    set rtrn [::gui::guiSaveAndRun]

##    No longer needed for xmission
##    RetrieveBEMResults $::gui::_nodename
    return $rtrn
}










