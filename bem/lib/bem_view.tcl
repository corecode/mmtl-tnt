#! /bin/sh
#----------------------------------------------------------------
#
#  bem_view.tcl
#
#  Displays BEM MMTL results using graphs
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: bem_view.tcl,v 1.3 2004/02/12 22:28:28 techenti Exp $
#
#----------------------------------------------------------------
# restart using wish \
exec wish "$0" "$@"

set auto_path [linsert $auto_path 0 /users/zahn/lib]

package require Itcl
package require BWidget
package require BLT

package require gui
namespace import ::gui::*

package provide bem 1.0

source [file join $env(BEM_LIBRARY) bem_graphs.itcl]

# --------------------------------------------------------------------------
# Starting with Tcl 8.x, the BLT commands are stored in their own 
# namespace called "blt".  The idea is to prevent name clashes with
# Tcl commands and variables from other packages, such as a "table"
# command in two different packages.  
#
# You can access the BLT commands in a couple of ways.  You can either
# prefix all the BLT commands with the namespace qualifier "blt:"
#  
#    blt::graph .g
#    blt::table . .g -resize both
# 
# or you can import all the command into the global namespace.
#
#    namespace import blt::*
#    graph .g
#    table . .g -resize both
#
# --------------------------------------------------------------------------

if { $tcl_version >= 8.0 } {
    namespace import blt::*
    namespace import -force blt::tile::*
}

##################################################################
# Select the results file to display.
##################################################################
proc _getResultsFile {} {
    global infofile

    set typelist {
        {{information files} {.swept_result .iterate_result} }
        {{All Files}        *                   }
    }

    set infofile [tk_getOpenFile -filetypes $typelist \
	    -title "swept_results or iterate_results:" \
	    -initialdir "." ]
    if { [string length $infofile] < 1 } {
	return
    }
    # Read the file.
    _collectData
}

##################################################################
# Create the gui window.
##################################################################
proc createMain { } {
    global   mainframe
    global   outputdir
    global   infofile
    global   _buttoncount
    global   _fxtNameList
    global   _fxtDataList
    global   _bxtNameList
    global   _bxtDataList
    global   _printerName
    global   _colorMode
    global _xaxis
    global _yaxis

    set _xaxis "width/diameter"
    set _yaxis "impedance"
    set _printerName "princess"
    set _colorMode "gray"
    set _fxtNameList ""

    set _fxtDataList ""
    set _bxtNameList ""
    set _bxtDataList ""
    set _bxtNameList ""
    set _buttoncount 1
    set outputdir [pwd]

    # Menu description
    set descmenu {
        "&File" all file 0 {
	    {command "&Print" {} \
		    "Generate a Postscript file and print the graph" {Ctrl g} \
		    -command _printPS }
	    {command "&Exit" {} "Exit the program" {Ctrl e} \
		    -command _exit }
	}
        "&Setup" all setup 0 {
	    {command "Setup Printer" {} "Set printer options" {} \
		    -command _setupPrinter}
	    {command "Toggle Grid Display" {} "Toggle visibility of grid" {} \
		    -command _toggleGrid}
	}
    }

    set mainframe [MainFrame .mainframe \
                       -menu         $descmenu \
                       -textvariable status]
    
    set allf [$mainframe getframe]
    set topf [frame $allf.topf -relief sunken -borderwidth 4]
    pack $topf -expand true -fill both
    
    set cfrme [frame $topf.cfrme -relief sunken -borderwidth 2]
    set f2 [frame $cfrme.f2 -relief sunken -borderwidth 2]

    set b1b [::gui::guiBuildButton $f2 "Input:" \
	    _getResultsFile \
	    "Pick the file containing the swept_results information"]
    set e1b [entry $f2.e1b -width 75 -textvariable infofile]
    pack $b1b $e1b -side left -fill both -expand yes
    pack $f2 -side top -fill x -expand yes


    set bfrme [frame $cfrme.f3]

    set p1 [ComboBox $bfrme.p1 -labelanchor e \
	    -textvariable _xaxis \
	    -values { width/diameter height conductivity pitch x-offset \
	    y-offset } \
	    -label "X-axis:" ]

    set p2 [ComboBox $bfrme.p2 -labelanchor e \
	    -textvariable _yaxis \
	    -values { impedance odd-impedance even-impedance \
	    dielectric-constant velocity delay Rdc \
	    FXT BXT } \
	    -label "Y-axis:" ]

    set bdraw [::gui::guiBuildButton $bfrme "Draw" \
	    "_drawTheGraph 1" \
	    "Draw the graph with selected x-axis and y-axis values" ]

    pack $bdraw $p1 $p2  -side left -fill both -expand yes
    pack $bfrme -side top -expand yes -fill x
    pack $cfrme -side top -expand yes -fill x
    _createGraph $topf


    pack $mainframe -fill both -expand yes

    if { [string length $infofile] > 0 } {
	_collectData
    }
}

##################################################################
# Generate hardcopy. 
##################################################################
proc _printPS {} {
    global infofile
    global grph1
    global _printerName
    global _colorMode
    
    set nme [file tail $infofile]
    $grph1 postscript output $nme.ps -landscape yes \
	    -colormode $_colorMode \
	    -maxpect yes -decorations true
    set cmd {lp -d$_printerName $nme.ps}
    eval exec $cmd
}

##################################################################
# Exit. 
##################################################################
proc _exit {} {
    exit
}

##################################################################
# Build a conductor object. 
##################################################################
proc _buildConductor { num conductor type wdth diam } {

    for { set i 0 } { $i < $num } { incr i } {
	set nme "$conductor$i"

	#
	# Create a graphs object for each conductor in the
	# set.
	#
	Graphs $nme -name $conductor -type $type
	eval [$nme configure -number $num]

	#
	# Check if need to save the width or diameter.
	#
	if { [string compare $type "width"] == \
		0 } {
	    $nme addWidth $wdth
	} else {
	    $nme addWidth $diam
	}
    }
}

##################################################################
# Read the swept_result or the iterate_result file.
##################################################################
proc _collectData {} {
    global outputdir
    global infofile
    global graph_title
    global _fxtNameList
    global _fxtDataList
    global _fxtXList
    global _bxtNameList
    global _bxtDataList
    global _bxtXList
    global _oddImpList
    global _evenImpList
    #
    # Delete any objects created for a previous graph.
    #
    foreach itm [itcl::find objects] {
	itcl::delete object $itm
    }
    _deleteVectors


    #
    # Initialization.
    #
    set _fxtNameList ""
    set _fxtDataList ""
    set _fxtXList ""
    set _bxtNameList ""
    set _bxtDataList ""
    set _bxtNameList ""
    set _bxtXList ""
    set _oddImpList ""
    set _evenImpList ""
    set gotFXT 0
    set gotBXT 0
    set countProcessed 0
    set firstSet 1
    set strg [file extension $infofile]
    set graph_title [string range $strg 1 [string length $strg]]
    set xp -1
    set diam 0
    set wdth 0


    #
    # Open the results file and collect the data needed.
    #
    set fp [open  $infofile r]
    while {1} {
	set lne [gets $fp]

	if { [eof $fp] } {
	    break
	}

	if { [string length $lne] == 0 } {
	    continue
	}

	# 
	# Check when have read the first design definition.
	#
	if { ([string first MMTL $lne] > 1) } {
	    set firstSet 0
	}
	
	
	#--------------------------------------------------------
	# Is this a RectangleConductors, TrapezoidConductors or a
	# CircleConductors?
	#--------------------------------------------------------
	if { ([string first RectangleConductors $lne] > 1) ||\
		([string first CircleConductors $lne] > 1) ||\
		([string first TrapezoidConductors $lne] > 1) } {
	    set nme [lindex $lne 0]
	    set conductor [string range $nme 0 [expr { [string length $nme] \
		    - 2 }]]

	    #
	    # Loop until collected needed attribute values.
	    #
	    while { 1 } { 
		set lne [gets $fp]
		#
		# 'diameter' attribute?
		#
		if { [string first iameter $lne] > 0 } {
		set indx [string first ":" $lne]
		    if { $indx > 1 } {
			set strg [string range $lne [expr { $indx + 1 }] \
				[string length $lne]]
			scan $strg "%g" diam
			set type "diameter"
			set chr C
		    }
		}
		#
		# 'width' attribute?
		#
		if { [string first idth $lne] > 0 } {
		    set indx [string first ":" $lne]
		    if { $indx > 1 } {
			set strg [string range $lne [expr { $indx + 1 }] \
				[string length $lne]]
			scan $strg "%g" wdth
			set type "width"
			if { [string first Width $lne] > 0 } {
			    set chr T
			} else {
			    set chr R
			}
		    }
		}
		#
		# 'height' attribute?
		#
		if { [string first eight $lne] > 0 } {
		    set indx [string first ":" $lne]
		    if { $indx > 1 } {
			set strg [string range $lne [expr { $indx + 1 }] \
				[string length $lne]]
			scan $strg "%g" hght
		    }
		}
		#
		# 'conductivity' attribute?
		#
		if { [string first onductiv $lne] > 0 } {
		    set indx [string first ":" $lne]
		    if { $indx > 1 } {
			set strg [string range $lne [expr { $indx + 1 }] \
				[string length $lne]]
			scan $strg "%g" conductivity
		    }
		}
		#
		# 'number' attribute?
		#
		if { [string first umber $lne] > 0 } {
		    set indx [string first ":" $lne]
		    if { $indx > 1 } {
			set strg [string range $lne [expr { $indx + 1 }] \
				[string length $lne]]
			scan $strg "%d" num
			if { $firstSet } {
			    set conductor [format {%s%s} $conductor $chr]
			    _buildConductor $num $conductor $type \
				    $wdth $diam
			    incr countProcessed
			} else {
			    set conductor [format {%s%s} $conductor $chr]
			    for { set i 0 } { $i < $num } { incr i } {
				if { [string compare $type "width"] == \
					0 } {
				    $conductor$i addHeight $hght
				    $conductor$i addWidth $wdth
				} else {
				    $conductor$i addWidth $diam
				    set wdth $diam
				}
				$conductor$i addConductivity $conductivity
			    }
			}
		    }
		}
		#
		# 'ptich' attribute?
		#
		if { [string first itch $lne] > 0 } {
		    set indx [string first ":" $lne]
		    if { $indx > 1 } {
			set strg [string range $lne [expr { $indx + 1 }] \
				[string length $lne]]
			scan $strg "%g" pitch
		    }
		
		    for { set i 0 } { $i < $num } { incr i } {
			$conductor$i addPitch $pitch
		    }
		}
		#
		# 'x-offset' attribute?
		#
		if { [string first offset $lne] > 0 } {
		    set indx [string first ":" $lne]
		    if { $indx > 1 } {
			set strg [string range $lne [expr { $indx + 1 }] \
				[string length $lne]]
			scan $strg "%g" xoff
		    }
		
		    for { set i 0 } { $i < $num } { incr i } {
			$conductor$i addXoff $xoff
		    }

		    #
		    # 'y-offset' attribute
		    #
		    set lne [gets $fp]
		    set indx [string first ":" $lne]
		    if { $indx > 1 } {
			set strg [string range $lne [expr { $indx + 1 }] \
				[string length $lne]]
			scan $strg "%g" yoff
		    }
		
		    for { set i 0 } { $i < $num } { incr i } {
			$conductor$i addYoff $yoff
		    }

		    #
		    # Break out of this loop since no more attribute
		    # values need to be saved.
		    #
		    break
		}
	    }
	} 
	
	#
	################ parse for the impedance values #################
	#
	if { ([string first Impedance $lne] > 1) } {

	    incr xp
	    set lne [gets $fp]
	    while { ( [string first "Odd/Even" $lne] < 1 ) &&
	            ( [string first "Dielectric Constant" $lne] < 1 ) } {
		set indx [string first "::" $lne]
		if { $indx  > 1 } {
		    set indx2 [string first "=" $lne]
		    set conductor [string range $lne [expr { $indx + 2 }] \
			    [expr { $indx2 - 1}]]
		    set strg [string range $lne $indx2 [string length $lne]]
		    scan $strg "%*c %g" imp
		    if {[lsearch -exact [itcl::find objects] $conductor] > -1} {
			$conductor addImpedance $imp
			$conductor addX $xp
		    }
		}
		set lne [gets $fp]
	    }
	    if {  [string first "Odd/Even" $lne] > 0 } {
		set lne [gets $fp]
		scan $lne {%*s %f} imp
		lappend _oddImpList $imp
		set lne [gets $fp]
		scan $lne {%*s %f} imp

		lappend _evenImpList $imp
		set lne [gets $fp]
		set lne [gets $fp]
	    }
	    #
	    ######## parse for the delectric constant values #############
	    #
	    set lne [gets $fp]
	    while { [string first "ation Velocity" $lne] < 1 } {
		set indx [string first "::" $lne]
		if { $indx  > 1 } {
		    set indx2 [string first "=" $lne]
		    set conductor [string range $lne [expr { $indx + 2 }] \
			    [expr { $indx2 - 1}]]
		    set strg [string range $lne $indx2 [string length $lne]]
		    scan $strg "%*c %g" dielC
		    if {[lsearch -exact [itcl::find objects] $conductor] > -1} {
			$conductor addDielectricConstant $dielC
		    }
		}
		set lne [gets $fp]
	    }

	    #
	    ######## parse for the velocity values #############
	    #
	    set lne [gets $fp]
	    while { [string first "ation Delay" $lne] < 1 } {
		set indx [string first "::" $lne]
		if { $indx  > 1 } {
		    set indx2 [string first "=" $lne]
		    set conductor [string range $lne [expr { $indx + 2 }] \
			    [expr { $indx2 - 1}]]
		    set strg [string range $lne $indx2 [string length $lne]]
		    scan $strg "%*c %g" velocity
		    if {[lsearch -exact [itcl::find objects] $conductor] > -1} {
			$conductor addVelocity $velocity
		    }
		}
		set lne [gets $fp]
	    }

	    #
	    ######## parse for the delay values #############
	    #
	    set lne [gets $fp]
	    while { [string first "dc:" $lne] < 1 } {
		set indx [string first "::" $lne]
		if { $indx  > 1 } {
		    set indx2 [string first "=" $lne]
		    set conductor [string range $lne [expr { $indx + 2 }] \
			    [expr { $indx2 - 1}]]
		    set strg [string range $lne $indx2 [string length $lne]]
		    scan $strg "%*c %g" delay
		    if {[lsearch -exact [itcl::find objects] $conductor] > -1} {
			$conductor addDelay $delay
		    }
		}
		set lne [gets $fp]
	    }

	    #
	    ######## parse for the rdc values #############
	    #
	    while { 1 } {
		set lne [gets $fp]
		if { [string first "(Active Sign" $lne] > 0 } {
		    break
		}
	    }
	    set lne [gets $fp]
	    while { [string first "(Forward" $lne] < 1 } {
		set indx [string first "::" $lne]
		if { $indx  > 1 } {
		    set indx2 [string first ", " $lne]
		    set conductor [string range $lne [expr { $indx + 2 }] \
			    [expr { $indx2 - 2}]]
		    set strg [string range $lne $indx2 [string length $lne]]
		    set indx [string first "::" $strg]
		    if { $indx  < 1 } {
			puts "Trouble"
			return
		    }
		    set indx2 [string first " )" $strg]
		    set cnd2 [string range $strg [expr { $indx + 2 }] \
			    [expr { $indx2 - 1}]]
		    if { [string compare $conductor $cnd2] != 0 } {
			set lne [gets $fp]
			continue
		    }
		    set strg [string range $strg $indx2 [string length $strg]]
		    set indx2 [string first "=" $strg]
		    set strg [string range $strg $indx2 [string length $strg]]
		    scan $strg "%*c %g" rdc
		    if {[lsearch -exact [itcl::find objects] $conductor] > -1} {
			$conductor addRdc $rdc
		    }
		}
		set lne [gets $fp]
	    }

	    #
	    ######## parse for the FXT values #############
	    #
	    set fxtCount 0
	    set lne [gets $fp]
	    while { [string first "(Backward" $lne] < 1 } {
		set indx [string first "::" $lne]
		if { $indx  > 1 } {
		    set indx2 [string first ", " $lne]
		    set conductor [string range $lne [expr { $indx + 2 }] \
			    [expr { $indx2 - 2}]]
		    set strg [string range $lne $indx2 [string length $lne]]
		    set indx [string first "::" $strg]
		    if { $indx  < 1 } {
			puts "Trouble"
			return
		    }
		    set indx2 [string first ")=" $strg]
		    set cnd2 [string range $strg [expr { $indx + 2 }] \
			    [expr { $indx2 - 1}]]

		    global FXT$fxtCount

		    if { ! $gotFXT } {
			lappend _fxtNameList $conductor$cnd2
			eval [set FXT$fxtCount ""]
			lappend _fxtDataList FXT$fxtCount
		    }
		    set strg [string range $strg [expr { $indx2 + 2 }] \
			    [string length $strg]]
		    if { [string first "infinite" $strg] > 0 } {
			set fxt 0.0
		    } else {
			set indx2 [string first "=" $strg]
			set strg [string range $strg $indx2 \
				[string length $strg]]
			scan $strg "%*c %g" fxt
		    }
		    eval { lappend FXT$fxtCount $fxt }
		    incr fxtCount
		}
		set lne [gets $fp]
	    }
	    lappend _fxtXList $wdth
	    set gotFXT 1

	    #
	    ######## parse for the BXT values #############
	    #
	    set bxtCount 0
	    set lne [gets $fp]
	    while { [string first "Cross talk" $lne] < 1 } {
		set indx [string first "::" $lne]
		if { $indx  > 1 } {
		    set indx2 [string first ", " $lne]
		    set conductor [string range $lne [expr { $indx + 2 }] \
			    [expr { $indx2 - 2}]]
		    set strg [string range $lne $indx2 [string length $lne]]
		    set indx [string first "::" $strg]
		    if { $indx  < 1 } {
			puts "Trouble"
			return
		    }
		    set indx2 [string first ")=" $strg]
		    set cnd2 [string range $strg [expr { $indx + 2 }] \
			    [expr { $indx2 - 1}]]

		    global BXT$bxtCount
		    
		    if { ! $gotBXT } {
			lappend _bxtNameList "$conductor$cnd2"
			eval [set BXT$bxtCount ""]
			lappend _bxtDataList BXT$bxtCount
		    }
		    set strg [string range $strg [expr { $indx2 + 2 }] \
			    [string length $strg]]
		    set indx2 [string first "=" $strg]
		    set strg [string range $strg $indx2 [string length $strg]]
		    scan $strg "%*c %g" bxt
		    eval { lappend BXT$bxtCount $bxt }
		    incr bxtCount
		}
		set lne [gets $fp]
	    }
	    lappend _bxtXList $wdth
	    set gotBXT 1

	}
    }
    close $fp

    _drawTheGraph -1


    return
}

##################################################################
# Delete the existing vectors.
##################################################################
proc _deleteVectors {} {
    global grph1

    if { [llength [$grph1 element names]] > 0 } {
	set cnt 0
	foreach itm [$grph1 element names] {

	    #-------------------------------------------------
	    # Destroy the vectors for the elements of the graph.
	    #-------------------------------------------------
	    set xv [$grph1 element cget $itm -xdata]
	    set yv [$grph1 element cget $itm -ydata]
	    vector destroy $xv
	    vector destroy $yv

	    #-------------------------------------------------
	    # Delete the elements from the graph.
	    #-------------------------------------------------
	    $grph1 element delete $itm
	}
    }
}

proc _getColor { clrIndx } {
    global numColors
    global colorList

    set indx [expr { $clrIndx - (($clrIndx / $numColors) * $numColors) }]
    return [lindex $colorList $indx]
}


##################################################################
# Draw the graph.
##################################################################
proc _drawTheGraph { flg } {
    global grph1
    global infofile
    global graph_title
    global _fxtNameList
    global _fxtDataList
    global _fxtXList
    global _bxtNameList
    global _bxtDataList
    global _bxtXList
    global _xaxis
    global _yaxis
    global xLst
    global _oddImpList
    global _evenImpList
    
    if { $flg > -1 } {
	_deleteVectors
    } else {
	set flg 0
    }

    
    $grph1 configure  -title [file tail $infofile]
    $grph1 axis configure x -title $_xaxis
    $grph1 axis configure y -title $_yaxis

    
    set cnt 0
    set clrCnt 0

    set lst [itcl::find objects]
    foreach itm $lst {
	
	switch -- $_xaxis {
	    width/diameter {
		set xLst [$itm cget -widthList]
	    }
	    height {
		set xLst [$itm cget -heightList]
	    }
	    conductivity {
		set xLst [$itm cget -conductivityList]
	    }
	    pitch {
		set xLst [$itm cget -pitchList]
	    }
	    x-offset {
		set xLst [$itm cget -xOffList]
	    }
	    y-offset {
		set xLst [$itm cget -yOffList]
	    }
	}
    
	###############################################################
	# Draw the FXT data to the 2D graph.
	###############################################################
	if { [string compare $_yaxis "FXT"] == 0 } {
	    
	    foreach obj $_fxtDataList {
		global FXT$cnt
		
		vector yvec$cnt
		vector xvec$cnt
		
		eval [xvec$cnt set $xLst]
		set itmList FXT$cnt
		set strg [format "yvec%d set \$FXT%d" $cnt $cnt]
		eval $strg
		$grph1 element create [lindex $_fxtNameList $cnt] \
			-color [_getColor $clrCnt] \
			-symbol circle -pixels 5 -linewidth 2 \
			-xdata xvec$cnt -ydata yvec$cnt
		
		incr cnt
		incr clrCnt
	    }
	    return
	}
	
	###############################################################
	# Draw the BXT data to the 2D graph.
	###############################################################
	if { [string compare $_yaxis "BXT"] == 0 } {
	    
	    foreach itm $_bxtDataList {
		global BXT$cnt
		
		vector yvec$cnt
		vector xvec$cnt
		
		eval [xvec$cnt set $xLst]
		set itmList BXT$cnt
		set strg [format "yvec%d set \$BXT%d" $cnt $cnt]
		eval $strg
		
		$grph1 element create [lindex $_bxtNameList $cnt] \
			-color [_getColor $clrCnt] \
			-symbol circle -pixels 5 -linewidth 2 \
			-xdata xvec$cnt -ydata yvec$cnt
		
		incr cnt
		incr clrCnt
	    }
	    return
	}
	
	
	switch -- $_yaxis {
	    impedance {
		vector yvec$cnt
		vector xvec$cnt
		
		eval [xvec$cnt set $xLst]
		set yLst [$itm cget -impList]
		eval [yvec$cnt set $yLst]
		$grph1 axis configure y -title $_yaxis
		$grph1 element create $itm -color [_getColor $clrCnt] \
			-symbol circle -pixels 5 -linewidth 2 \
			-xdata xvec$cnt -ydata yvec$cnt
		
	    }
	    odd-impedance {
		vector yvec$cnt
		vector xvec$cnt
		
		eval [xvec$cnt set $xLst]
		set yLst $_oddImpList
		if { [llength $yLst] < 1 } {
		    tk_messageBox -type ok \
			 -message "The results file contains no\
			 odd-impedance values." -icon warning
		    return
		}	    
		eval [yvec$cnt set $yLst]
		$grph1 axis configure y -title $_yaxis
		$grph1 element create $itm -color [_getColor $clrCnt] \
			-symbol circle -pixels 5 -linewidth 2 \
			-xdata xvec$cnt -ydata yvec$cnt
		
	    }
	    even-impedance {
		vector yvec$cnt
		vector xvec$cnt
		
		eval [xvec$cnt set $xLst]
		set yLst $_evenImpList
		if { [llength $yLst] < 1 } {
		    tk_messageBox -type ok \
			 -message "The results file contains no\
			 even-impedance values." -icon warning
		    return
		}	    
		eval [yvec$cnt set $yLst]
		$grph1 axis configure y -title $_yaxis
		$grph1 element create $itm -color [_getColor $clrCnt] \
			-symbol circle -pixels 5 -linewidth 2 \
			-xdata xvec$cnt -ydata yvec$cnt
		
	    }
	    velocity {
		vector yvec$cnt
		vector xvec$cnt
		
		eval [xvec$cnt set $xLst]
		set yLst [$itm cget -velocityList]
		eval [yvec$cnt set $yLst]
		$grph1 axis configure y -title $_yaxis
		$grph1 element create $itm -color [_getColor $clrCnt] \
			-symbol circle -pixels 5 -linewidth 2 \
			-xdata xvec$cnt -ydata yvec$cnt
		
	    }
	    delay {
		vector yvec$cnt
		vector xvec$cnt
		
		eval [xvec$cnt set $xLst]
		set yLst [$itm cget -delayList]
		eval [yvec$cnt set $yLst]
		$grph1 axis configure y -title $_yaxis
		$grph1 element create $itm -color [_getColor $clrCnt] \
			-symbol circle -pixels 5 -linewidth 2 \
			-xdata xvec$cnt -ydata yvec$cnt
		
	    }
	    dielectric-constant {
		vector yvec$cnt
		vector xvec$cnt
		
		eval [xvec$cnt set $xLst]
		set dielCLst [$itm cget -dielCList]
		eval [yvec$cnt set $dielCLst]
		$grph1 axis configure y -title $_yaxis
		$grph1 element create $itm -color [_getColor $clrCnt] \
			-symbol circle -pixels 5 -linewidth 2 \
			-xdata xvec$cnt -ydata yvec$cnt
		
	    }
	    Rdc {
		vector yvec$cnt
		vector xvec$cnt
		
		eval [xvec$cnt set $xLst]
		set rdcLst [$itm cget -rdcList]
		eval [yvec$cnt set $rdcLst]
		$grph1 axis configure y -title $_yaxis
		$grph1 element create $itm -color [_getColor $clrCnt] \
			-symbol circle -pixels 5 -linewidth 2 \
			-xdata xvec$cnt -ydata yvec$cnt
		
	    }
	}
	incr cnt
	incr clrCnt
    }

    return
}


proc _toggleGrid {} {
    global   grph1

    $grph1 grid toggle
}

##################################################################
# Create the graph.
##################################################################
proc _createGraph { parent } {
    global   grph1

    set fgrph [ frame $parent.fgrph -relief raised -borderwidth 2 ]

     # create a new graph.  
    set grph1 [graph $fgrph.g1 -plotbackground white ]

#    $grph1 configure -width 900
    $grph1 legend configure -font {courier 8}

    # get the current display list.
    pack $grph1 -side top -expand yes -fill both

    pack $fgrph -side top -expand yes -fill both
}


##########################################################
# Set up a printer.
##########################################################
proc _setupPrinter {} {
    global _printerName
    global _colorMode
    global _savePrinterName
    global _saveColorMode
    global _prntFrame
    global _ecount

    set _ecount 0

    set _savePrinterName $_printerName
    set _saveColorMode $_colorMode

    if {! [winfo exists .prntFrame]} {

	set _prntFrame [frame .prntFrame \
		-relief sunken -borderwidth 5]
	set seglbl [Label $_prntFrame.seglbl \
		-justify center -text "Printer Options"]
	grid $seglbl -sticky new
	
	set c1 [::gui::guiBuildLabelEntry $_prntFrame _printerName \
		"Printer:"  ]
	grid $c1 -sticky new

	set p1 [ComboBox $_prntFrame.sb -labelanchor e \
	    -textvariable _colorMode \
	    -values { color grey mono } \
	    -label "Color Mode:" ]
	grid $p1 -sticky new
	LabelFrame::align $c1 $p1
	
	set but1 [::gui::guiBuildButton $_prntFrame "OK" \
		"_finishPrinter 1" "Print"]
	set but2 [::gui::guiBuildButton $_prntFrame "Cancel" \
		"_finishPrinter 0" "Cancel the Print"]
	grid $but1 $but2 -sticky nw
    }
    place $_prntFrame -x 75 -y 300
}

##########################################################
# Finish setting up a printer.
##########################################################
proc _finishPrinter { opt } {
    global _printerName
    global _colorMode
    global _savePrinterName
    global _saveColorMode
    global _prntFrame

    place forget $_prntFrame

    if { $opt } {
	puts "Setup Printer:  $_printerName  $_colorMode"
	return
    }
    
    set _printerName $_savePrinterName
    set _colorMode $_saveColorMode

}

##################################################################
# main.
##################################################################
proc main { argc argv } {
    global auto_path
    global infofile
    global colorList
    global numColors
    global dashesList

    set numColors 8
    set colorList { navy blue green cyan magenta red orange yellow }

    lappend auto_path ..
    package require BWidget

    option add *font {courier 10 bold}
    wm withdraw .
    wm title . "Cross-section Generator"

    if { $argc == 1 } {
	set infofile [lindex $argv 0]
    }

    createMain
    BWidget::place . 0 0 center
    DynamicHelp::configure -bg black
    DynamicHelp::configure -fg white


    wm deiconify .
    raise .
    focus -force .
}


main $argc $argv
wm geom . [wm geom .]







