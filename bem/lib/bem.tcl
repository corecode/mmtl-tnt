#----------------------------------------------*-TCL-*------------
#
#  mmtl_bem.tcl
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: bem.tcl,v 1.6 2004/05/05 19:31:11 techenti Exp $
#
#-----------------------------------------------------------------
package require Itcl
package require units
package require csdl
package require gui

package provide bem 1.0

namespace eval ::bem:: {

    variable BuildCount 0

    variable Title {}
    variable Csegs
    variable Psegs

    variable _iterateFrame
    variable _maxIterations
    variable _row 1
    variable _col 0
    variable _spFrame
    variable _curls
    variable _impedanceType
    variable _endImpedance

    variable _botWidthStart
    variable _botWidthIncr

    variable _startcl
    variable _endcl
    variable _numcl
    variable _startrt
    variable _endrt
    variable _numrt
    variable _startcs
    variable _endcs
    variable _numcs
    variable _startps
    variable _endps
    variable _numps
    variable _totalLoops
    variable _changesArr

    #--------------------------------------------
    # Allow for sweeping the thickness and permittivity
    # of the DielectricLayer object.
    #--------------------------------------------
    variable _arrDielThckList
    variable _arrDielPrmtList

     #--------------------------------------------
    # Allow for sweeping the width, height, conductivity
    # and pitch of a RectangleConductors object.
    #--------------------------------------------
    variable _arrRectWidthList
    variable _arrCircDiamList
    variable _arrTrapWidthTopList
    variable _arrTrapWidthBotList
    variable _iterateFrame

    #--------------------------------------------
    # Allow for sweeping the diameter, conductivity
    # and pitch of a CircleConductors object.
    #--------------------------------------------
    variable _arrCircDiamList
    variable _arrCircCndctList
    variable _arrCircPtchList
    variable _arrCircXoffList
    variable _arrCircYoffList

    #--------------------------------------------
    # Allow for sweeping the top and bottom width, height, 
    # conductivity and pitch of a TrapezoidConductors object.
    #--------------------------------------------
    variable _arrTrapWidthTopList
    variable _arrTrapWidthBotList
    variable _arrTrapHeightList
    variable _arrTrapCndctList
    variable _arrTrapPtchList
    variable _arrTrapXoffList
    variable _arrTrapYoffList


    variable _arrStartList
    variable _arrEndList
    variable _arrNumList

    variable _doClngLgt
    variable _doRseTme
    variable _doCseg
    variable _doPseg
    variable _sweepFrame

    #-------------------------------------------------------
    # Used in the script generated for a sweep or an iterate
    #-------------------------------------------------------
    namespace export _changesArr

    namespace export bemDeleteAll
    namespace export bemFinishSimulation
    namespace export bemWriteGraphicFile
    namespace export bemRunSimulation
    namespace export bemRunSimulationCS
    namespace export bemViewPlotPotential
    namespace export bemViewGraph
    namespace export bemRunIterateMMTL
    namespace export bemRunSweepMMTL
    namespace export bemParseSweptResults
    namespace export bemParseIterationResults
    namespace export bemPutRectangleConductorsToFile
    namespace export bemPutCircleConductorsToFile
    namespace export bemPutTrapezodConductorsToFile
    namespace export bemPutDielectricLayerToFile
    namespace export bemPutGroundPlaneToFile
    namespace export bemReadFile
    namespace export bemShowResultsFile
    namespace export bemWriteSweptParameterFile
    
}

##########################################################
#
# bemReadFile
#    Write the xsctn file
#
# nodename: file name without the extension
#
##########################################################
proc ::bem::bemReadFile { nodename } {

    ::bem::bemDeleteAll


    #---------------------------------------------------------
    # Set the default units to match the gui-defined units for
    # length.
    #---------------------------------------------------------
    set ::units::default(Length) $::Stackup::defaultLengthUnits
    set ::units::default(Time) ps

    ::csdl::csdlReadTCL $nodename.xsctn

}
 

##########################################################
#
# bemDeleteAll
#    Delete all the objects created by the xsctn file
#
##########################################################
proc ::bem::bemDeleteAll {} {
    
    foreach snme $::Stackup::structureList {
	set lgt [string length $snme]
	set nme [string range $snme 2 $lgt]
	itcl::delete object $nme
    }

 
    set ::Stackup::structureList {}

    set lst [itcl::find objects]
    foreach itm $lst {
	itcl::delete object $itm
    }

}


##########################################################
#
# bemWriteGraphicFile
#    Write the graphic file needed by the bem simulator
#
# nodename: file name without the extension
#
##########################################################
proc ::bem::bemWriteGraphicFile { nodename } {

    #---------------------------------------------------------
    # Set the default units to match the gui-defined units for
    # length.
    #---------------------------------------------------------
    set ::units::default(Length) $::Stackup::defaultLengthUnits
    set ::units::default(Time) ps

    #------------------------------------------------------------
    # Delete the previous object (if exists)
    #------------------------------------------------------------
    catch { itcl::delete object gpgeObj } result

    #------------------------------------------------------------
    # Create the gpge object
    #------------------------------------------------------------
    gpge gpgeObj
    

    set filename $nodename.graphic
    #------------------------------------------------------------
    # Create the output for the graphic file.
    #------------------------------------------------------------
    set grph [gpgeObj graphic $filename]

    #------------------------------------------------------------
    # Open the graphic file and write the information to the file.
    #------------------------------------------------------------
    set fp [open $filename w]
    puts $fp $grph
    close $fp

    puts "The write of $filename is complete!"
    return 0
}

##################################################################
#
# bemRunSimulation
#
#    nodename  : file name without the extension
#    num_c_segs: number of contour segments
#    num_p_segs: number of plane/dielectric segments
#
##################################################################
proc ::bem::bemRunSimulationCS { nodename {num_c_segs 10} {num_p_segs 10} } {
    global env

    #---------------------------------------------------
    # This application sends some of it's output to stderr
    # so the exec will return with an error or 1.  The
    # catch solves this problem.
    #---------------------------------------------------
    set exeName [auto_execok bem]

    set rtn [::bem::bemFinishSimulation $exeName $nodename \
	    $num_c_segs $num_p_segs]
    return $rtn
}

##################################################################
#
# bemRunSimulation
#
#    nodename  : file name without the extension
#    num_c_segs: number of contour segments
#    num_p_segs: number of plane/dielectric segments
#
##################################################################
proc ::bem::bemRunSimulation { nodename {num_c_segs 10} {num_p_segs 10} } {
    global env

    #---------------------------------------------------
    # This application sends some of it's output to stderr
    # so the exec will return with an error or 1.  The
    # catch solves this problem.
    #---------------------------------------------------
    set exeName [file join $env(BEM_LIBRARY) nmmtl]

    set rtn [::bem::bemFinishSimulation $exeName $nodename \
	    $num_c_segs $num_p_segs]
    return $rtn
}


##################################################################
#
# bemFinishSimulation
#
##################################################################
proc ::bem::bemFinishSimulation { exeName nodename num_c_segs num_p_segs } {

    set cmmnd {$exeName $nodename $num_c_segs $num_p_segs}
    puts "----------------------------------------------------"
    puts "Command: $exeName $nodename $num_c_segs $num_p_segs"
    puts "----------------------------------------------------"
    set returnStatus 0
    set status [catch {  eval exec $cmmnd} result ]
    puts "status: $status  result: $result"
    if {  $status || ([string first "MMTL is done" $result] == 0) } {
	puts stderr "The simulation (Node: $nodename  CSEG: $num_c_segs\
		PSEG: $num_p_segs) has failed!"
	puts stderr $result
	set returnStatus 1
    } else {
	puts "The simulation of $nodename  (CSEG: $num_c_segs\
		PSEG: $num_p_segs) is done!"
    }
    set fp [open $nodename.result_log w]

    ::gui::_logDesignAttributes $fp
    puts $fp $result
    close $fp
    return $returnStatus
}

##################################################################
#
# bemViewPlotPotential
#
# nodename: file name without the extension
#
##################################################################
proc ::bem::bemViewPlotPotential { nodename } {
    global env


    set cmdName [file join $env(BEM_LIBRARY) staticFieldPlot.exe]
    set cmmnd {$cmdName $nodename.result_field_plot_data}
    
    set returnStatus 0
    if { [catch {  eval exec $cmmnd} result] } {
	puts "Finished display of $nodename.result_field_plot_data\
		with staticFieldPlot."
    } else {
	puts "Display of $nodename.result_field_plot_data with\
		staticFieldPlot has failed!"
	set returnStatus 1
    }
    return $returnStatus
}

##################################################################
##################################################################
proc ::bem::bemShowResultsFile { filename } {

    if { ! [file exists $filename] } {
	puts stderr "$filename doesn't exist!"
	return 1
    } 

    set fp [open $filename r]
    set result [read $fp]
    close $fp

    puts "------------------ $filename --------------------------"
    puts $result

    return $result
}

##################################################################
#
# bemViewGraph
#
# filename: file name of swept or iterated results data
#
##################################################################
proc ::bem::bemViewGraph { filename } {
    global env

    set cmdName [file join $env(BEM_LIBRARY) bem_view.tcl]

    set cmmnd "$cmdName $filename"

    set returnStatus 0
    if { [ catch {  eval exec $cmmnd} result] } {
	puts "Finished display of $filename with\
		$env(BEM_LIBRARY)/bem_view.tcl"
    } else {
	puts "Display of $filename with\
		$env(BEM_LIBRARY)/bem_view.tcl has failed.!"
	set returnStatus 1
    }
    return $returnStatus
}



