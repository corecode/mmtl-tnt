#! /bin/sh
# restart using wish \
exec /sppdg/software/tools/public_domain/${HOST_TYPE}/bin/wish "$0" "$@"

set auto_path [linsert $auto_path 0 /sppdg/software/src/tnt/calcRL/lib]
set auto_path [linsert $auto_path 0 /sppdg/software/src/tnt/bem/lib]
set auto_path [linsert $auto_path 0 /sppdg/software/src/tnt/gui]

package require Itcl
package require -exact BWidget 1.2.1

package require calcRL
package require gui


#  From whence is this script run?  
#  For there shall we find the rest.
set ::scriptDir [file dirname [info script]]
source [file join $::scriptDir calcRL_compare.tcl]


proc runTests { testNode originalNode log {cSegs 10} {pSegs 10} } {

    puts "-----------------------------------------------"

    puts "\nRunning calcRL on $testNode..."

    ::calcRL::calcRL_RunBatch $testNode 0
    calcRL_compare $testNode.out $originalNode.out_save $log
    

}

##################################################################
# Create the main window.
##################################################################
proc createMain { calcRLInitialize } {
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
    

    runTests $testDir/jc.data $orgDir/jc.data $log 20 40
    update
    
    runTests $testDir/test $orgDir/test $log 40 80
    update
    
    runTests $testDir/tst0.data $orgDir/tst0.data $log 45 45
    update

    runTests $testDir/tst1.data $orgDir/tst1.data $log 45 45
    update

    runTests $testDir/tst2.data $orgDir/tst2.data $log 45 45
    update

    runTests $testDir/tst3.data $orgDir/tst3.data $log 45 45
    update

    runTests $testDir/tst4.data $orgDir/tst4.data $log 45 45
    update

    runTests $testDir/tst5.data $orgDir/tst5.data $log 45 45
    update

    runTests $testDir/tst6.data $orgDir/tst6.data $log 45 45
    update

}

##################################################################
##################################################################
proc main { argv } {
    global env

    set calcRLInitialize 1
    

    option add *font {courier 10 bold}
    wm withdraw .
    
    #  Create main window
    createMain $calcRLInitialize
    BWidget::place . 0 0 center


    set testDir $::scriptDir

    set orgDir $::scriptDir

    set log [open $testDir/test_bem.log w]

    runTheTests $log $testDir $orgDir
    
    close $log
}

main $argv










