#! /bin/sh
# restart using wish \
exec /sppdg/software/tools/public_domain/${HOST_TYPE}/bin/wish "$0" "$@"

set auto_path [linsert $auto_path 0 /sppdg/software/lib]

package require Itcl
package require -exact BWidget 1.2.1

package require bem
namespace import ::bem::*


#  From whence is this script run?  
#  For there shall we find the rest.
set ::scriptDir [file dirname [info script]]
source [file join $::scriptDir mmtl_compare.tcl]


proc runTests { testNode originalNode log {cSegs 10} {pSegs 10} } {

    puts "-----------------------------------------------"

#    bemDeleteAll
#    puts "\n\nProcessing the graphic file..."
#    puts $log "\n\nProcessing the graphic file..."
#    source $testNode.xsctn
#    bemWriteGraphicFile $testNode
#    bemRunSimulation $testNode $cSegs $pSegs
#    mmtl_compare $testNode.result $originalNode.result $log

    bemDeleteAll
    puts "\nProcessing the cross-section file..."
    puts $log "\nProcessing the cross-section file..."
    source $testNode.xsctn

##    set newNode [format {%sB} $testNode]
##    ::csdl::csdlWriteTCL $newNode.xsctn \
##	    "Duplicate of $newNode.xsctn" $cSegs $pSegs
##    puts "\nComparing xsctn files $testNode.xsctn - $newNode.xsctn"
##    mmtl_compare $testNode.xsctn $newNode.xsctn $log

    bemRunSimulationCS $testNode $cSegs $pSegs
    mmtl_compare $testNode.result $originalNode.result_save $log
    

}

##################################################################
# Create the main window.
##################################################################
proc createMain { mmtlInitialize } {
    global mainframe
    global status

    # Menu description
    set descmenu {
        "&File" all file 0 {
            {command "&Quit" {} "Quit" {Ctrl q} -command _quit}
	}
    }

    set mainframe [MainFrame .mainframe \
                       -menu         $descmenu \
                       -textvariable status]
    

    ::gui::guiCreateConsole
    ::gui::guiConsole 1


    pack $mainframe -fill both -expand yes

    wm protocol . WM_DELETE_WINDOW {_exit}

}

proc runTheTests { log testDir orgDir } {
    global env
    

    runTests $testDir/9-7-00 $orgDir/9-7-00 $log 20 40
    update

    runTests $testDir/abcs1 $orgDir/abcs1 $log 20 40
    update
    
    runTests $testDir/w10t2.5 $orgDir/w10t2.5 $log 40 80
    update
    
    runTests $testDir/coplanar $orgDir/coplanar $log 45 45
    update

    runTests $testDir/trap_test $orgDir/trap_test $log 45 45
    update

    runTests $testDir/test1 $orgDir/test1 $log 45 45
    update

    runTests $testDir/generic $orgDir/generic $log 90 90
    update


}

##################################################################
##################################################################
proc main { argv } {
    global env

    set mmtlInitialize 1
    

    option add *font {courier 10 bold}
    wm withdraw .
    
    #  Create main window
    createMain $mmtlInitialize
    BWidget::place . 0 0 center


    set testDir $::scriptDir

    set orgDir $::scriptDir

    set log [open $testDir/test_bem.log w]

    runTheTests $log $testDir $orgDir
    
    close $log
}

main $argv










