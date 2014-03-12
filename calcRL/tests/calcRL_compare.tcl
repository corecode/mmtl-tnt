
proc calcRL_compare { file1 file2 log } {

    #--------------------------------------------------------------
    # Open the first first file for the camparison.
    #--------------------------------------------------------------
    if { [catch { set f1 [open $file1 r] } result ] } {
	puts stderr "Could not open $file1"
	puts $log "Could not open $file1"
	return 1
    }

    #--------------------------------------------------------------
    # Get the data from the file.
    #--------------------------------------------------------------
    set data1 [ read $f1]
    close $f1

    #--------------------------------------------------------------
    # Open the second file for the comparison.
    #--------------------------------------------------------------
    if { [catch { set f2 [open $file2 r] } result ] } {
	puts stderr "Could not open $file2"
	puts $log "Could not open $file2"
	return 1
    }

    #--------------------------------------------------------------
    # Get the data from the file.
    #--------------------------------------------------------------
    set data2 [read $f2]
    close $f2

    set isCS 0
    set isResult 0
    #-------------------------------------------------------
    # Check if this is a result file.
    #-------------------------------------------------------
    if { [ string first "out" $file1] > 1 } {
	
	set indx [string first "equency" $data1]
	if { $indx < 1 } {
	    puts stderr "The comparison failed!"
	    puts $log "The comparison failed!"
	    return 1
	}
	set data1 [string range $data1 [expr {$indx + 9}] \
		[string length $data1]]

	set indx [string first "requency" $data2]
	if { $indx < 1 } {
	    puts stderr "The comparison failed!"
	    puts $log "The comparison failed!"
	    return 1
	}
	set data2 [string range $data2 [expr {$indx + 9}] \
		[string length $data2]]
	
    } 
    
    #----------------------------------------------------------------------
    # Initialize.
    #----------------------------------------------------------------------
    set maxDiff 0
    set maxNum1 0
    set maxNum2 0
    set indx 0
    set total1 0
    set total2 0
    set cnt1 0
    foreach itm $data1 {
	#-------------------------------------------------------------------
	# Only process the items that are numbers.
	#-------------------------------------------------------------------
	if { [scan $itm {%f} num1 ] == 1 } {
	    puts "$itm  .. [lindex $data2 $indx]"
	    update
	    #---------------------------------------------------------------
	    # Check that this index in the second data-set is also a number.
	    #---------------------------------------------------------------
	    if { [scan [lindex $data2 $indx] {%f} num2] < 1 } {
		break
	    }

	    if { $num1 != 0 } {
		set diff [expr { abs ( ($num1 - $num2) / $num1 ) * 100.0 }]
		set total1 [expr { $total1 + $diff }]
		incr cnt1
		#------------------------------------------------------------
		# Save the maximum diff from the comparisons.
		#------------------------------------------------------------
		if { $diff > $maxDiff } {
		    set maxDiff $diff
		    set maxNum1 $num1
		    set maxNum2 $num2
		}
	    }
	}
	incr indx
    }

    set total1 [expr { $total1 / $cnt1 }]
    puts stderr "-------------------------------------------------------------"
    puts stderr "Comparing:\t$file1\n          \t$file2"
    set strg [format "              Max Diff: %7.3f%s  (%s : %s)" $maxDiff "\%" \
	    $maxNum1 $maxNum2]
    puts stderr $strg

    if { ! $isCS } {
	set strg [format "          Average Diff: %7.3f%s" $total1 "\%"]
	puts stderr $strg
	if { $isResult } {
	    set total2 [expr { $total2 / $cnt2 }]
	    set strg [format "(I - Rdc) Average Diff: %7.3f%s" $total2 "\%"]
	    puts stderr $strg
	    set strg [format "    Total Average Diff: %7.3f%s" \
		    [expr { $total1 + $total2 }] "\%"]
	    puts stderr $strg
	}
    }

    puts $log "-------------------------------------------------------------"
    puts $log "Comparing:\t$file1\n          \t$file2"
    set strg [format "             Max Diff: %5.3f%s  (%s : %s)" $maxDiff "\%" \
	    $maxNum1 $maxNum2]
    puts $log $strg

    if { ! $isCS } {
	set strg [format "          Average Diff: %7.3f%s" $total1 "\%"]
	puts $log $strg
    }
    
    return 0
}






