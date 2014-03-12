#----------------------------------------------*-TCL-*------------
#
#  bem_parameters.tcl
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: bem_parameters.tcl,v 1.4 2004/06/03 14:19:29 techenti Exp $
#
#----------------------------------------------*-TCL-*------------


package provide bem 1.0

#--------------------------------------------------------------------------
# Routine to parse the <node>.swept_results file and write the file
# <node>_swept_result.csv.  This file contains the comma-separated data
# - one line of data for each simulation run.
#--------------------------------------------------------------------------

proc ::bem::bemWriteSweptParameterFile { filetype } {

    set doOnce 1

    #-----------------------------------------------------------
    # Get the default units for the geometries.
    #-----------------------------------------------------------
    set defUnits $::Stackup::defaultLengthUnits

    #-----------------------------------------------------------
    # Open <node>.swept_result as the input file.
    #-----------------------------------------------------------
    set filename [format {%s.%s} $::gui::_nodename $filetype]
    if { ! [file exists $filename] } {
	::gui::guiPopupWarning "$filename doesn't exist!"
	return
    }
    set fp [open $filename r]

    #-----------------------------------------------------------
    # Open <node>_swept_result.csv as the output file.
    #-----------------------------------------------------------
    set filename [format {%s_%s.csv} $::gui::_nodename $filetype]
    set outf [open $filename w]

    while { 1 } {
	#-------------------------------------------------------
	# Process the results of a simulation run -- unitl EOF
	#-------------------------------------------------------
	if { [eof $fp] } {
	    close $fp
	    close $outf
	    return $filename
	}
	
	#-------------------------------------------------------
	# Initialize the variables that contain the new-file 
	# information
	#    data   : will contain the comma-separated data values
	#    header1: will contain the comma-separated data identifiers
	#    header2: will contain the comma-separated data units
	#-------------------------------------------------------
	set data ""
	set header1 ""
	set header2 ""
	while { 1 } {
	    set line [gets $fp]
	
	    if { [eof $fp] } {
		close $fp
		close $outf
		return $filename
	    }

	    #---------------------------------------------------
	    # Break out of this loop when read the "Node =" record
	    #---------------------------------------------------
	    if { ([string first "ode =" $line] > -1) || \
		 ([string first "ile =" $line] > -1) } {
		break
	    } 
	   
	    set indx [string first ":" $line]
	    if { $indx < 0 } {
		continue
	    }
	    scan $line {%s %s} itm nme

	    #---------------------------------------------------
	    # Is this a Dielectric?
	    #---------------------------------------------------
	    if { [string compare $nme "DielectricLayer"] == 0 } {
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Thickness
		#-----------------------------------------------
		set indx1 [string first "thickness" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } thck
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Permittivity
		#-----------------------------------------------
		set indx1 [string first "permittivity" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } permit
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Permeability
		#-----------------------------------------------
		set indx1 [string first "permeability" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } permea
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  LossTangent
		#-----------------------------------------------
		set indx1 [string first "lossTangent" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line {%s} lssTngt
		
		append header1 $itm
		append header1 "T,"
		append header2 "$defUnits,"
		append data "$thck,"

		append header1 $itm
		append header1 "Pi,"
		append header2 ","
		append data "$permit,"

		append header1 $itm
		append header1 "Pe,"
		append header2 ","
		append data "$permea,"

		append header1 $itm
		append header1 "LT,"
		append header2 ","
		append data "$lssTngt,"
	    }
	    
	    #---------------------------------------------------
	    # Is this a Rectangular conductor?
	    #---------------------------------------------------
	    if { [string compare $nme "RectangleConductors"] == 0 } {
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Height
		#-----------------------------------------------
		set indx1 [string first "height" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } hght
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Width
		#-----------------------------------------------
		set indx1 [string first "width" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } wdth
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Conductivity
		#-----------------------------------------------
		set indx1 [string first "conductivity" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } cndt
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Number
		#-----------------------------------------------
		set indx1 [string first "number" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line {%d} number
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Pitch
		#-----------------------------------------------
		set indx1 [string first "pitch" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } ptch
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  X-offset
		#-----------------------------------------------
		set indx1 [string first "x-offset" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } xoff
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Y-offset
		#-----------------------------------------------
		set indx1 [string first "y-offset" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } yoff
		append header1 $itm
		append header1 "W,"
		append header2 "$defUnits,"
		append data "$wdth,"

		append header1 $itm
		append header1 "H,"
		append header2 "$defUnits,"
		append data "$hght,"

		append header1 $itm
		append header1 "C,"
		append header2 "siemens/meter,"
		append data "$cndt,"

		append header1 $itm
		append header1 "N,"
		append header2 ","
		append data "$number,"

		append header1 $itm
		append header1 "P,"
		append header2 "$defUnits,"
		append data "$ptch,"

		append header1 $itm
		append header1 "X,"
		append header2 "$defUnits,"
		append data "$xoff,"

		append header1 $itm
		append header1 "Y,"
		append header2 "$defUnits,"
		append data "$yoff,"
	    }
	    
	    #---------------------------------------------------
	    # Is this a Trapezoidal conductor?
	    #---------------------------------------------------
	    if { [string compare $nme "TrapezoidConductors"] == 0 } {
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Height
		#-----------------------------------------------
		set indx1 [string first "height" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } hght
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  TopWidth
		#-----------------------------------------------
		set indx1 [string first "topWidth" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } tpwdth
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  BotWidth
		#-----------------------------------------------
		set indx1 [string first "botWidth" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } btwdth
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Conductivity
		#-----------------------------------------------
		set indx1 [string first "conductivity" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } cndt
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Number
		#-----------------------------------------------
		set indx1 [string first "number" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line {%d} number
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Pitch
		#-----------------------------------------------
		set indx1 [string first "pitch" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } ptch
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  X-offset
		#-----------------------------------------------
		set indx1 [string first "x-offset" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } xoff
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Y-offset
		#-----------------------------------------------
		set indx1 [string first "y-offset" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } yoff


		append header1 $itm
		append header1 "Tw,"
		append header2 "$defUnits,"
		append data "$tpwdth,"

		append header1 $itm
		append header1 "Bw,"
		append header2 "$defUnits,"
		append data "$btwdth,"

		append header1 $itm
		append header1 "H,"
		append header2 "$defUnits,"
		append data "$hght,"

		append header1 $itm
		append header1 "C,"
		append header2 "siemens/meter,"
		append data "$cndt,"

		append header1 $itm
		append header1 "N,"
		append header2 ","
		append data "$number,"

		append header1 $itm
		append header1 "P,"
		append header2 "$defUnits,"
		append data "$ptch,"

		append header1 $itm
		append header1 "X,"
		append header2 "$defUnits,"
		append data "$xoff,"

		append header1 $itm
		append header1 "Y,"
		append header2 "$defUnits,"
		append data "$yoff,"
	    }
	    
	    #---------------------------------------------------
	    # Is this a circular conductor?
	    #---------------------------------------------------
	    if { [string compare $nme "CircleConductors"] == 0 } {
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Diameter
		#-----------------------------------------------
		set indx1 [string first "diameter" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } dmtr
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Conductivity
		#-----------------------------------------------
		set indx1 [string first "conductivity" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } cndt
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Number
		#-----------------------------------------------
		set indx1 [string first "number" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line {%d} number
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Pitch
		#-----------------------------------------------
		set indx1 [string first "pitch" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } ptch
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  X-offset
		#-----------------------------------------------
		set indx1 [string first "x-offset" $line]
		set line [string range $line [expr {$indx1 + 13}] \
			[string length $line]]
		scan $line { %f } xoff
		
		set line [gets $fp]
		#-----------------------------------------------
		#
		#  Y-offset
		#-----------------------------------------------
		set indx1 [string first "y-offset" $line]
		set line [string range $line [expr {$indx1 + 13}] \
		    [string length $line]]
		scan $line { %f } yoff
		append header1 $itm
		append header1 "D,"
		append header2 "$defUnits,"
		append data "$dmtr,"

		append header1 $itm
		append header1 "C,"
		append header2 "siemens/meter,"
		append data "$cndt,"

		append header1 $itm
		append header1 "N,"
		append header2 "$defUnits,"
		append data "$number,"

		append header1 $itm
		append header1 "P,"
		append header2 "$defUnits,"
		append data "$ptch,"

		append header1 $itm
		append header1 "X,"
		append header2 "$defUnits,"
		append data "$xoff,"

		append header1 $itm
		append header1 "Y,"
		append header2 "$defUnits,"
		append data "$yoff,"
	    }
	    
	}

	#-------------------------------------------------------
	# Read until get the "Coupling Length =" record.
	#-------------------------------------------------------
	while { [string first "pling Length" $line] < 0 } {
	    set line [gets $fp]
	    
	}
	
	#-------------------------------------------------------
	# Coupling length
	#-------------------------------------------------------
	scan $line {%*s %*s = %f %s } cplgt unts
	append header1 "CL,"
	append header2 "$unts,"
	append data "$cplgt,"

	#-------------------------------------------------------
	# Risetime
	#-------------------------------------------------------
	set line [gets $fp]
	scan $line {%*s %*s = %f %s } rtme unts
	append header1 "RT,"
	append header2 "$unts,"
	append data "$rtme,"

	#-------------------------------------------------------
	# cseg
	#-------------------------------------------------------
	set line [gets $fp]
	scan $line {%*s %*s %*s %*s = %d} cseg
	append header1 "cseg,"
	append header2 ","
	append data "$cseg,"

	#-------------------------------------------------------
	# dseg
	#-------------------------------------------------------
	set line [gets $fp]
	scan $line {%*s %*s %*s %*s = %d} dseg
	append header1 "dseg,"
	append header2 ","
	append data "$dseg,"


	#-------------------------------------------------------
	# Read until get the "Mutual and Self Electrostatic Induction"
	# record
	#-------------------------------------------------------
	while { [string first "Electrostatic Induc" $line] < 0 } {
	    set line [gets $fp]
	    
	}
	
	#-------------------------------------------------------
	# Skip the two header records
	#-------------------------------------------------------
	set line [gets $fp]
	set line [gets $fp]
	
	while { [string length $line] > 1 } {
	    
	    scan $line {%*s %s %*c %s %*s %f } v1 v2 b
	    set lgt [expr { [string length $v1] - 1 }]
	    set c1 [string range $v1 2 $lgt]
	    set lgt [expr { [string length $v2] - 1 }]
	    set c2 [string range $v2 2 $lgt]
	    append header1 "B$c1$c2,"
	    append header2 "Farads/Meter,"
	    append data "$b,"
	    set line [gets $fp]
	}
	
	#-------------------------------------------------------
	# Skip the headers for the "Mutual and Self Inductance" 
	# records
	#-------------------------------------------------------
	set line [gets $fp]
	set line [gets $fp]
	set line [gets $fp]

	while { ([string length $line] > 1) && \
	    ([string first "try Ratio" $line] < 1) } {
	    
	    scan $line {%*s %s %*c %s %*s %f} v1 v2 b
	    set lgt [expr { [string length $v1] - 1 }]
	    set c1 [string range $v1 2 $lgt]
	    set lgt [expr { [string length $v2] - 1 }]
	    set c2 [string range $v2 2 $lgt]
	    append header1 "L$c1$c2,"
	    append header2 "Henrys/Meter,"
	    append data "$b,"
	    set line [gets $fp]
	}
	
	#-------------------------------------------------------
	# Read until get the "Characteristic Impedance" record
	#-------------------------------------------------------
	while { [string first "ic Imped" $line] < 0 } {
	    set line [gets $fp]
	    
	}
	
	set line [gets $fp]
	while { [string length $line] > 1 } {
	    
	    scan $line {%*s %*s %*s %s %f} v1 b
	    set lgt [expr { [string length $v1] - 2 }]
	    set c1 [string range $v1 2 $lgt]
	    append header1 "I$c1,"
	    append header2 "Ohms,"
	    append data "$b,"
	    set line [gets $fp]
	}

	#-------------------------------------------------------
	# Check if there are Odd/Even impedance values.
	#-------------------------------------------------------
	set line [gets $fp]
	if { [string first "Odd/Even" $line] > 0 } {

	    set line [gets $fp]
	    scan $line {%*s %f} b
	    append header1 "Odd$c1,"
	    append header2 ","
	    append data "$b,"

	    set line [gets $fp]

	    scan $line {%*s %f} b
	    append header1 "Even$c1,"
	    append header2 ","
	    append data "$b,"

	    set line [gets $fp]
	    set line [gets $fp]
	}

	#-------------------------------------------------------
	# Read the "Effective Dielectric Constant" records
	#-------------------------------------------------------
	set line [gets $fp]
	
	while { [string length $line] > 1 } {
	    
	    scan $line {%*s %*s %*s %s %f} v1 b
	    set lgt [expr { [string length $v1] - 2 }]
	    set c1 [string range $v1 2 $lgt]
	    append header1 "DC$c1,"
	    append header2 ","
	    append data "$b,"
	    set line [gets $fp]
	}
	
	#-------------------------------------------------------
	# Read the "Propagation Velocity" records
	#-------------------------------------------------------
	set line [gets $fp]
	set line [gets $fp]
	
	while { [string length $line] > 1 } {
	    
	    scan $line {%*s %*s %*s %s %f} v1 b
	    set lgt [expr { [string length $v1] - 2 }]
	    set c1 [string range $v1 2 $lgt]
	    append header1 "PV$c1,"
	    append header2 "meters/second,"
	    append data "$b,"
	    set line [gets $fp]
	}
	
	#-------------------------------------------------------
	# Check if there are Odd/Even propagation velocity values.
	#-------------------------------------------------------
	set line [gets $fp]
	if { [string first "Odd/Even" $line] > 0 } {

	    set line [gets $fp]
	    scan $line {%*s %f} b
	    append header1 "PVOdd$c1,"
	    append header2 ","
	    append data "$b,"

	    set line [gets $fp]

	    scan $line {%*s %f} b
	    append header1 "PVEven$c1,"
	    append header2 ","
	    append data "$b,"
	    set line [gets $fp]
	    set line [gets $fp]
	}

	#-------------------------------------------------------
	# Read the "Propagation Delay" records
	#-------------------------------------------------------
	set line [gets $fp]
	
	while { [string length $line] > 1 } {
	    
	    scan $line {%*s %*s %*s %s %f} v1 b
	    set lgt [expr { [string length $v1] - 2 }]
	    set c1 [string range $v1 2 $lgt]
	    append header1 "PD$c1,"
	    append header2 "seconds/meter,"
	    append data "$b,"
	    set line [gets $fp]
	}
	
	
	#-------------------------------------------------------
	# Check if there are Odd/Even propagation delay  values.
	#-------------------------------------------------------
	set line [gets $fp]
	if { [string first "Odd/Even" $line] > 0 } {

	    set line [gets $fp]
	    scan $line {%*s %f} b
	    append header1 "PDOdd$c1,"
	    append header2 ","
	    append data "$b,"

	    set line [gets $fp]

	    scan $line {%*s %f} b
	    append header1 "PDEven$c1,"
	    append header2 ","
	    append data "$b,"

	    set line [gets $fp]
	    set line [gets $fp]
	}

	#-------------------------------------------------------
	# Read the "Rdc" records
	#-------------------------------------------------------
	set line [gets $fp]
	set line [gets $fp]
	
	while { [string length $line] > 1 } {
	    
	    scan $line {%*s %s %*c %s %*s %f} v1 v2 b
	    set lgt [expr { [string length $v1] - 1 }]
	    set c1 [string range $v1 2 $lgt]
	    set lgt [expr { [string length $v2] - 1 }]
	    set c2 [string range $v2 2 $lgt]
	    append header1 "Rdc$c1$c2,"
	    append header2 "Ohms/Meter,"
	    append data "$b,"
	    set line [gets $fp]
	}
	
	#-------------------------------------------------------
	# Read the FXT records
	#-------------------------------------------------------
	set line [gets $fp]
	set line [gets $fp]
	set line [gets $fp]

	while { [string length $line] > 1 } {
	    
	    scan $line {%*s %s %*c %s %*s %*s = %f} v1 v2 b
	    set lgt [expr { [string length $v1] - 1 }]
	    set c1 [string range $v1 2 $lgt]
	    set lgt [expr { [string length $v2] - 1 }]
	    set c2 [string range $v2 2 $lgt]
	    append header1 "FXT$c1$c2,"
	    append header2 "dB,"
	    append data "$b,"
	    set line [gets $fp]
	}
	
	#-------------------------------------------------------
	# Read the BXT records
	#-------------------------------------------------------
	set line [gets $fp]
	set line [gets $fp]
	set line [gets $fp]
	
	set frst 1
	while { [string length $line] > 1 } {
	    
	    if { $frst } {
		set frst 0
	    } else {
		append header1 ","
		append header2 ","
		append data ","
	    }

	    scan $line {%*s %s %*c %s %*s %*s = %f} v1 v2 b
	    set lgt [expr { [string length $v1] - 1 }]
	    set c1 [string range $v1 2 $lgt]
	    set lgt [expr { [string length $v2] - 1 }]
	    set c2 [string range $v2 2 $lgt]
	    append header1 "BXT$c1$c2"
	    append header2 "dB"
	    append data $b
	    set line [gets $fp]
	}

	if { $doOnce } {
	    puts $outf $header1
	    puts $outf $header2
	    set doOnce 0
	}
	puts $outf $data
    }

    close $outf
    close $fp
   return $filename
}


