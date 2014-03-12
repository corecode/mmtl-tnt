#----------------------------------------------------------------
#
#  mmtl_initialize.tcl
#
#  Main entry point for the interface program
#  for Mayo SPPDG MMTL and other transmission line
#  simulators.
#
#  This module creates the BWidget ::gui::mainframe framework
#  for the main window, and defines procedures for
#  accessing numerous other integrated simulation programs
#  and utilities.
#
#  The actual GUI is constructed by [createElements]
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved
#  $Id: gui_initialize.tcl,v 1.29 2004/09/10 19:51:10 techenti Exp $
#
#----------------------------------------------------------------

package provide gui 2.0

#  From whence is this script run?
#  For there shall we find the rest. (of the GUI stuff)
set ::gui::scriptDir [file dirname [info script]]

#----------------------------------------------------------------
#
#  ::gui::guiNew
#
#  "New File" operation, called from the menu File->New.
#  This function just deletes all the design information.
#
#----------------------------------------------------------------
proc ::gui::guiNew {} {

    #------------------------------------------------------
    #  Delete all the design information,
    #  and reset GUI defaults.
    #------------------------------------------------------
    ::gui::_deleteAll

    set ::gui::_nodename ""
    set ::gui::_nodedirectory ""
    ::gui::_setNodeName
}


#----------------------------------------------------------------
#
#  ::gui::guiOpenExisting
#
#  "Open File" operation, called from the menu File->Open.
#
#  Prompt for file name (if not given).  Set node name
#  and directory, and read the cross section.
#
#  Arguments:
#      filename (optional)  -- path name with
#                              (optional) .xsctn type
#
#  Results:
#    Sets global variables for node name and node directory.
#    Reads new cross section data.
#
#----------------------------------------------------------------
proc ::gui::guiOpenExisting { {filename ""} } {

    set ::gui::_title ""
    set typelist {
        {{mmtl files}   {.xsctn}     }
        {{All Files}        *             }
    }

    #------------------------------------------------------
    #  If we didn't get a filename from the argument,
    #  then prompt for a file name
    #------------------------------------------------------
    if { $filename == "" } {
	set filename [ tk_getOpenFile  -filetypes $typelist \
		-initialdir $::gui::_nodedirectory]
	#  Evade Windows double-click bug
	if { $::tcl_platform(platform) eq "windows" } {
	    ::gui::drainEventQueue
	}

	# Check that the user didn't cancel from this option.
	if { [string length $filename] < 1 } {
	    return {}
	}
    } else {
	#------------------------------------------------------
	#  We _did_ get a file name.
	#  Allow file type ".xsctn" to be optional
	#------------------------------------------------------
	if { [file extension $filename] ne ".xsctn" } {
	    append $filename ".xsctn"
	}
    }

    #------------------------------------------------------
    #  Define node name and directory
    #------------------------------------------------------
    set ::gui::_nodedirectory [file dirname $filename]
    set ::gui::_nodename [file rootname $filename]


    #------------------------------------------------------
    #  Open and read the file, update the GUI
    #------------------------------------------------------
    ::gui::_read_cross_section_file $filename

}

#----------------------------------------------------------------
#
#  ::gui::_read_cross_section_file
#
#  Read cross section file and update the GUI
#
#  This function does _not_ update the global node
#  name and directory, and is suitable for reading
#  in temporary cross sections, as might be done
#  during iterations or sweeping.
#
#  Arguments:
#     filename
#
#  Results:
#     Old cross section results are deleted
#     New cross section file is read.
#     GUI is updated.
#
#----------------------------------------------------------------
proc ::gui::_read_cross_section_file { filename } {

    #---------------------------------------------------------
    # Delete old data and set defaults
    #---------------------------------------------------------
    ::gui::_deleteAll
    set ::CSEG 10
    set ::DSEG 10

    #---------------------------------------------------
    # Read new cross section and set GUI parameters
    #---------------------------------------------------
    ::csdl::csdlReadTCL $filename
    set ::gui::_num_c_segs $::CSEG
    set ::gui::_num_p_segs $::DSEG
    set ::gui::_title $::_title

    #---------------------------------------------------
    #  Tell the units conversion routines what our new
    #  defeault units should be
    #---------------------------------------------------
    set ::units::default(Length) $::Stackup::defaultLengthUnits
    set ::units::default(Time) ps

    #---------------------------------------------------
    #  Update the GUI
    #---------------------------------------------------
    ::gui::_setNodeName
    ::gui::_canvas_redraw
    ::gui::_canvas_zoom_fit $::gui::_canvas
    foreach obj $::Stackup::structureList {
	set objname [namespace tail $obj]
	$::gui::_tree insert 0 root $objname -text $objname
    }
}



#----------------------------------------------------------------
#
#  ::gui::guiSave
#
#  Save the cross section file to the previously
#  defined file name.  If the file name doesn't exist
#  (because this is a new cross section), then call guiSaveAs.
#
#----------------------------------------------------------------
proc ::gui::guiSave {} {

    #-------------------------------------
    #  If we don't have a file name, then
    #  call guiSaveAs, which chooses a
    #  file name, then calls us again.
    #-------------------------------------
    if { $::gui::_nodename eq "" } {
	::gui::guiSaveAs
	return
    }

    set filename ${::gui::_nodename}.xsctn

    #-------------------------------------
    #  Some attributes need to be changed
    #  from GUI-specific formatting to
    #  regular CSDL.
    #-------------------------------------
    ::gui::_replaceNamedValues
    ::csdl::csdlWriteTCL $filename $::gui::_title \
	$::gui::_num_c_segs  $::gui::_num_p_segs

    return
}

#----------------------------------------------------------------
#
#  ::gui::guiSaveAs
#
#  Prompt for a file name and save the cross section.
#
#  Its almost an infinte loop ... but we can call guiSave
#  to do the actual file save.  (As long as we make sure
#  that the filename is defined.)
#
#----------------------------------------------------------------
proc ::gui::guiSaveAs {} {

    #------------------------------------------------------
    # Prompt for file name
    #------------------------------------------------------
    set typelist {
        {{cross-section files} {.xsctn} }
        {{All Files}               *    }
    }

    set filename [ tk_getSaveFile  -filetypes $typelist \
		       -initialdir $::gui::_nodedirectory ]
    #  Evade Windows double-click bug
    if { $::tcl_platform(platform) eq "windows" } {
	::gui::drainEventQueue
    }

    #------------------------------------------------------
    # Check that the user didn't cancel from this option.
    #------------------------------------------------------
    if { [string length $filename] < 1 } {
	return {}
    }

    #------------------------------------------------------
    # Allow file type ".xsctn" to be optional
    #------------------------------------------------------
    if { [file extension $filename] ne "*.xsctn" } {
	append $filename ".xsctn"
    }

    #------------------------------------------------------
    #  Save directory and file names, and
    #  display the new filename
    #------------------------------------------------------
    set ::gui::_nodedirectory [file dirname $filename]
    set ::gui::_nodename [file rootname $filename]
    ::gui::_setNodeName


    #---------------------------------------------------
    #  Save the file
    #---------------------------------------------------
    ::gui::guiSave
}



#----------------------------------------------------------------
#
#  ::gui::writeHspiceW
#
#  Write HSPICE W-element model.
#
#----------------------------------------------------------------
proc ::gui::writeHspiceW {} {


    #--------------------------------------------------------------
    #  Create the Welement object, and ask it for the
    #  W-element string
    #--------------------------------------------------------------
    set w [Welement #auto]
    set wmodel [$w welementString ${::gui::_nodename}.result]
    itcl::delete object $w

    #--------------------------------------------------------------
    #  Write the W-element file and display it to the user
    #--------------------------------------------------------------
    set fp [open ${::gui::_nodename}.hspice-w.rlgc w]
    puts $fp $wmodel
    close $fp

    ::gui::guiPopupResultsFile "hspice-w.rlgc"

}

##################################################################
##################################################################
proc ::gui::guiViewPlotP {} {
    global env

    ::bem::bemViewPlotPotential $::gui::_nodename

}

##################################################################
##################################################################
proc ::gui::guiViewTheGraph { type } {

    ::bem::bemViewGraph ${::gui::_nodename}.$type

}


proc ::gui::_setDisplayData {} {

    #-------------------------------------------------------------
    # The user has the choice of setting the display data to the
    # data from the last save, data prior to the simulations, or
    # leaving set to the values at the end of the last simulation.
    #-------------------------------------------------------------
    set retrn [tk_dialog .tkd "Question" \
	    "Set the display data to?" \
	    question {} \
	    "Data from previous save" "Data prior to simulations" \
	    "Data after simulations"]
    
    switch -- $retrn {
	0 {
	    puts "Set the display values to the data from the last save."
	    _read_cross_section_file ${::gui::_nodename}.xsctn
	}
	1 {
	    puts "Set the display values to the data prior to\
		    the simulations."
	    _read_cross_section_file ${::gui::_nodename}.xsctn.temp
	}
	2 {

	    if { ! [catch { puts "Update the values for\
		    $selectedObject" }] } {
		#------------------------------------------------
		# Put the currently defined values into the 
		# appropriate form.
		#------------------------------------------------
		_fillGlobalValues $selectedObject
	    }
	}
    }
    ::gui::_canvas_redraw
}


#----------------------------------------------------------------
#
#  ::gui::saveAndRunBem
#
#  Save the cross section file.
#  Open a control dialog with a log window
#  Run 'bem' simulator, directing output
#  to the log window.
#  Display BEM results in another dialog.
#
#----------------------------------------------------------------
proc ::gui::saveAndRunBem {} {

    #  Make sure that the dialogs exist
    if { ! [winfo exists $::gui::dialog(bemRun,dialog)] } {
	::gui::_createBemRunDialog
    }
    if { ! [winfo exists $::gui::dialog(bemLog,dialog)] } {
	::gui::_createBemLogDialog
    }

    #  Open a "run" window, let them change settings,
    #  and check to see if they hit the "run" button.
    set result [$::gui::dialog(bemRun,dialog) draw]

    if { $result == 0 } {

	#  Save the cross section
	#  (might have new coupling length or rise time)
	::gui::guiSave

	#  Run mmtl as a subprocess.
	set cmd [auto_execok bem]
	set filename $::gui::_nodename
	set cseg $::gui::_num_c_segs
	set dseg $::gui::_num_p_segs
	set pipe [open "|$cmd \"$filename\" $cseg $dseg" w+]

	#  Connect stdout and stderror to the text widget
	fconfigure $pipe -buffering none -blocking 0
	set txt $::gui::dialog(bemLog,text)
	fileevent $pipe readable [list ::gui::_copyDataToText $txt $pipe]

	#  Set busy cursor, clear the text widget, and draw dialog
	$txt configure -cursor watch
	$txt delete 1.0 end
	set result [$::gui::dialog(bemLog,dialog) draw]

	#  Just in case the simulator is still running
	#  when we get back from the dialog...
	catch {close $pipe}
	set defcursor [lindex [$txt configure -cursor] 3]
	$txt configure -cursor $defcursor

	#  Did they click "View Results?"
	if { $result == 0 } {
	    ::gui::guiPopupResultsFile "result"
	}
    }

}


#----------------------------------------------------------------
#
#  ::gui::_copyDataToText
#
#  Copy data from a file channel into a text widget.
#  Called on a 'readable' fileevent, this will copy
#  whatever data is available on a file channel
#  to the end of the text widget.
#
#  If we detect EOF, then we set the text widget cursor
#  to the default, on the assumption that somebody might
#  have changed it to a watch.
#
#----------------------------------------------------------------
proc ::gui::_copyDataToText {textwidget fp} {
    #  Read the channel
    set status [catch {set line [read $fp]} result]
    if { $status != 0 } {
	# error on the channel
	error "Error reading $fp:  $result"
    } else {
	#  append data to text widget
	$textwidget insert end $line
#	$textwidget insert end \n
	$textwidget see end
    }
    #  check for end of run
    if { [eof $fp] } {
	close $fp
	set defcursor [lindex [$textwidget configure -cursor] 3]
	$textwidget configure -cursor $defcursor
    }
}

#----------------------------------------------------------------
#
#  ::gui::writeSweptCsvFile
#
#  Writes a csv (character separated values) file from
#  the swept (or iterated) results file.
#
#----------------------------------------------------------------
proc ::gui::writeSweptCsvFile { filetype } {

    #  Write csv file
    #  (sorry, no choices...)
    set filename [::bem::bemWriteSweptParameterFile $filetype]

    #  Display file for user
    if { $filename ne "" } {
	::gui::guiPopupFile $filename
    }
}

#
#----------------------------------------------------------------
#
#  ::gui::runCalcRL
#
#  Run the experimental wavelet-based RL calculator.
#
#----------------------------------------------------------------
proc ::gui::runCalcRL {} {


    #  Make sure that the dialogs exist
    if { ! [winfo exists $::gui::dialog(calcRLRun,dialog)] } {
	::gui::_createCalcRLRunDialog
    }
    if { ! [winfo exists $::gui::dialog(calcRLLog,dialog)] } {
	::gui::_createCalcRLLogDialog
    }

    #  Open a "run" window, let them change settings,
    #  and check to see if they hit the "run" button.
    set result [$::gui::dialog(calcRLRun,dialog) draw]

    if { $result == 0 } {

	#------------------------------------------------
	#  Save the cross section and generate the
	#  calcRL input file
	#------------------------------------------------
	::gui::guiSave
	::calcRL::genInputFile $::gui::_nodename $::Stackup::frequency

	#  Figure out command and filename
	set cmd [auto_execok calcRL]
	set filename ${::gui::_nodename}.ri

	#  Translate frequencies to Hz
	foreach freq $::gui::calcRLfrequencies {
	    lappend frequencies [::units::convert $freq "Hz"]
	}

	#------------------------------------------------
	#  We need to run calcRL multiple times for
	#  multiple frequencies.  We want to have
	#  asynchronous execution, but we can only tie
	#  one process pipeline to the log text widget.
	#  (Multiple [open "|calcRL..."] calls would
	#  interleave their output.)
	#
	#  Since /bin/sh might not be available, we will
	#  write a tcl script with the proper number of
	#  exec commands.
	#
	#  Unfortunately, we have to modify the input
	#  file each time we execute the calcRL program.
	#  But all the info for creating a new data
	#  file is in this interpreter.  So our script
	#  becomes a bunch of inline data and exec commands.
	#------------------------------------------------

	#  Start building script
	set script {
	    # /bin/tclsh
	    #  Dynamically Created Script for running calcRL
	    #
	    #  We need to repeately re-write the input
	    #  file with new frequency values.
	    proc writeInputFile {filename data} {
		set fp [open $filename "w"]
		puts $fp $data
		close $fp
	    }
	}

	#  No "append" for the first frequency, so
	#  we get a clean set of output files
	set freq [lindex $frequencies 0]
	::calcRL::genInputFile $::gui::_nodename $freq
	set fp [open ${filename}.in "r"]
	set data [read $fp]
	close $fp
	append script "writeInputFile \"$filename.in\" \{$data\}\n"
	append script "catch {exec $cmd \"$filename\" \"\"} result\n"
	append script "puts \$result\n"

	#  Add data and commands for following frequencies
	foreach freq [lrange $frequencies 1 end] {
	    ::calcRL::genInputFile $::gui::_nodename $freq
	    set fp [open ${filename}.in "r"]
	    set data [read $fp]
	    close $fp
	    append script "writeInputFile \"$filename.in\" \{$data\}\n"
	    append script "catch {exec $cmd \"$filename\" append} result\n"
	    append script "puts \$result\n"
	}
	append script "exit\n"

	#  Write the script to a file
	#  (oooh, if only tclsh took command line scripts...)
	set scriptfilename ${filename}.script
	set fp [open $scriptfilename "w"]
	puts $fp $script
	close $fp


	#  ... finally ... we can execute the script
	set tclsh [auto_execok tclsh]
	if { $tclsh eq "" } {
	    set tclsh [list [info nameofexecutable]]
	}
	set pipe [open "|$tclsh \"$scriptfilename\"" "r+"]

	#  Connect stdout and stderror to the text widget
	fconfigure $pipe -buffering none -blocking 0
	set txt $::gui::dialog(calcRLLog,text)
	fileevent $pipe readable [list ::gui::_copyDataToText $txt $pipe]

	#  Set busy cursor, clear the text widget, and draw dialog
	$txt configure -cursor watch
	$txt delete 1.0 end
	set result [$::gui::dialog(calcRLLog,dialog) draw]

	#  Just in case the simulator is still running
	#  when we get back from the dialog...
	catch {close $pipe}
	set defcursor [lindex [$txt configure -cursor] 3]
	$txt configure -cursor $defcursor

	#  Did they click "View Results?"
	if { $result == 0 } {
	    ::gui::guiPopupResultsFile "ri.out"
	}
    }

}


#----------------------------------------------------------------
#
#  ::gui::runCalcCap
#
#  Run the experimental wavelet-based RL calculator.
#
#----------------------------------------------------------------
proc ::gui::runCalcCap {} {


    #  Make sure that the dialogs exist
    if { ! [winfo exists $::gui::dialog(calcCapLog,dialog)] } {
	::gui::_createCalcCapLogDialog
    }

    #------------------------------------------------
    #  Save the cross section and generate the
    #  calcCap input file
    #------------------------------------------------
    ::gui::guiSave
    ::calcCAP::genInputFile $::gui::_nodename

    #  Figure out command and filename, and run it
    set cmd [auto_execok calcCAP]
    set filename ${::gui::_nodename}.cap
    set pipe [open "|$cmd \"$filename\"" "r+"]

    #  Connect stdout and stderror to the text widget
    fconfigure $pipe -buffering none -blocking 0
    set txt $::gui::dialog(calcCapLog,text)
    fileevent $pipe readable [list ::gui::_copyDataToText $txt $pipe]

    #  Set busy cursor, clear the text widget, and draw dialog
    $txt configure -cursor watch
    $txt delete 1.0 end
    set result [$::gui::dialog(calcCapLog,dialog) draw]

    #  Just in case the simulator is still running
    #  when we get back from the dialog...
    catch {close $pipe}
    set defcursor [lindex [$txt configure -cursor] 3]
    $txt configure -cursor $defcursor

    #  Did they click "View Results?"
    if { $result == 0 } {
	::gui::guiPopupResultsFile "cap.out"
    }
}


#----------------------------------------------------------------
#
#  ::gui::guiPopupResultsFile
#
#  Pop up a dialog with one of the simulation results
#  files.  This is really a convenience function for
#  many of the GUI operations, which operate on
#  files with the same base name as the cross section.
#
#  Appends the file type to the open file root
#  name ($::gui::_nodename), and call ::gui::guiPopupFile.
#
#----------------------------------------------------------------
proc ::gui::guiPopupResultsFile { type } {

    ::gui::guiPopupFile ${::gui::_nodename}.$type

}


proc ::gui::_replaceNamedValues {} {

    #---------------------------------------------------------
    # Set the default units to match the gui-defined units for
    # length.
    #---------------------------------------------------------
    set ::units::default(Length) $::Stackup::defaultLengthUnits
    set ::units::default(Time) ps


    foreach item $::Stackup::structureList {
        set nme [string range $item 2 \
                [expr {[string length $item] - 1} ]]
        #-------------------------------------------------
        # Find out the component-type of $nme.
        #-------------------------------------------------
        if { [$nme isa GroundPlane] } {
            continue
        }
        if { [$nme isa DielectricLayer] || \
              [$nme isa RectangleDielectric] } {
            set type 1
        }
        if { [$nme isa RectangleConductors] || \
             [$nme isa CircleConductors] || \
             [$nme isa TrapezoidConductors] } {
            set type 2
        }

        #-------------------------------------------------
        # Loop through the attributes of this item. 
        #-------------------------------------------------
        foreach def [$item configure] {
            set flg [lindex $def 0]
            set vle [lindex $def 2]
            #---------------------------------------------
            # Skip the attribute is the value is set to the default.
            #---------------------------------------------
            if { [string compare $vle [lindex $def 1]] == 0 } {
                continue
            }

            #---------------------------------------------
            # If attribute is conductivity, check if the value
            # is the name of a material.  If it is, replace the
            # name with the numerical value.
            #---------------------------------------------
            if { ($type == 2) && \
                    (! [string compare $flg "-conductivity"]) } {
                set nme [lindex $vle 0]
                if { ! [ catch { set val $::gui::_cValList($nme) } \
                        result ] } {
                    set val [lindex $vle 1]
                    if { [string length $val] < 1 } {
                        set val $::gui::_cValList($nme)
                    }
                    $item configure -conductivity $val
                }
            }
            #---------------------------------------------
            # If attribute is permittivity, check if the value
            # is the name of a material.  If it is, replace the
            # name with the numerical value.
            #---------------------------------------------
            if { ($type == 1) && \
                    (! [string compare $flg "-permittivity"]) } {
                set nme [lindex $vle 0]
                if { ![ catch { set val $::gui::_pValList($nme) } \
                        result ] } {
                    set val [lindex $vle 1]
                    if { [string length $val] < 1 } {
                        set val $::gui::_pValList($nme)
                    }
                    $item configure -permittivity $val
                }
            }
            #---------------------------------------------
            # If attribute is loss tangent, check if the value
            # is the name of a material.  If it is, replace the
            # name with the numerical value.
            #---------------------------------------------
            if { ($type == 1) && \
                    (! [string compare $flg "-lossTangent"]) } {
                set nme [lindex $vle 0]
                if { ! [ catch { set val $::gui::_ltValList($nme) } \
                        result ] } {
                    set val [lindex $vle 1]
                    if { [string length $val] < 1 } {
                        set val $::gui::_ltValList($nme)
                    }
                    $item configure -lossTangent $val
                }
            }
        }
    }
}


##################################################################
#
#
##################################################################
proc ::gui::guiRunIterateMMTL {} {

    if { [string length $::gui::_nodename] < 1 } {
	guiOpenExisting
    }

    ::bem::bemRunIterateMMTL $::gui::_nodename $::gui::_num_c_segs \
    $::gui::_num_p_segs 1 $::gui::f1 $::gui::f1

}


##################################################################
##################################################################
proc ::gui::guiRunSweepMMTL {} {

   if { [string length $::gui::_nodename] < 1 } {
	guiOpenExisting
    }

    ::bem::bemRunSweepMMTL $::gui::_nodename $::gui::_num_c_segs \
	    $::gui::_num_p_segs 1 $::gui::f1 $::gui::f1

}



##################################################################
##################################################################
proc ::gui::guiRunFee2dDisplay {} {

    ::fem::femRunFee2dDisplay [file join $::scriptDir \
				   ${::gui::_nodename}.fee2d]
}

 
##################################################################
##################################################################
proc ::gui::guiSaveFee2d {} {

    #---------------------------------------------------------
    # Set the default units to match the gui-defined units for
    # length.
    #---------------------------------------------------------
    set ::units::default(Length) $::Stackup::defaultLengthUnits
    set ::units::default(Time) ps

    _replaceNamedValues

    if { [string length $::gui::_nodename] < 1 } {
	::gui::guiSaveAs
    }
    ::fem::femSave  [file join $::scriptDir $::gui::_nodename]
}

##################################################################
##################################################################
proc ::gui::guiRunFee2dRun {} {

    ::fem::femRunFee2d ${::gui::_nodename}.fee2d

}

##################################################################
##################################################################
proc ::gui::guiRunFee2dPlotTranParameters {} {

    ::fem::femPlotTranParameters [file join $::scriptDir \
				      ${::gui::_nodename}.fee2d]
 
}

##################################################################
##################################################################
proc ::gui::guiRunFee2dPlotEigenParameters {} {

    ::fem::femPlotEigenParameters [file join $::scriptDir \
				       ${::gui::_nodename}.fee2d]
 
}

##########################################################
# Write the input file for the program the calculates 
# resistance/inductance.
##########################################################
proc ::gui::guiGenRLinputFiles {} {

    ::calcRL::calcRL_GenInputFiles $::gui::_nodename
}


proc ::gui::guiRunRLbatch { flg } {

    ::calcRL::calcRL_RunBatch $::gui::_nodename $flg

    ::gui::guiPopupResultsFile "ri.out"

}

proc ::gui::guiRunRLgraphical {} {

    ::calcRL::calcRL_RunGraphical $::gui::_nodename

}


##########################################################
# Write the input file for the program the calculates 
# capacitance.
##########################################################
proc ::gui::guiGenCAPinputFiles {} {

    ::calcCAP::calcCAP_GenInputFiles $::gui::_nodename
}


proc ::gui::guiRunCAPbatch {} {

    ::calcCAP::calcCAP_RunBatch $::gui::_nodename

    ::gui::guiPopupResultsFile "cap.out"
}

proc ::gui::guiRunCAPgraphic {} {

    ::calcCAP::calcCAP_RunGraphical $::gui::_nodename

}




















