#! /bin/sh
#----------------------------------------------*-TCL-*------------
#
#  calcRL_exe.tcl
#
#  Main program script for calcRL.
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: calcRL_exe.tcl,v 1.4 2004/02/12 22:20:18 techenti Exp $
#
#-----------------------------------------------------------------
# restart using wish \
exec wish "$0" "$@"

global curPosition

global openFile 

global fileHead;# the number of each kind of conductor and dielectric

global dieleData recData cirData triData ellData polyData gndData
# global variables for saving all parameters of each kind
global palFlag
global corFlag
global newFlag
global yesMove

global recCounter cirCounter dieleCounter ellCounter triCounter polyCounter \
       gndCounter
global recTags cirTags dieleTags ellTags triTags polyTags gndTags
global recId cirId dieleId ellId triId polyId gndId

global maxXcoord maxYcoord
global paletteXsize paletteYsize Xscale Yscale
global pbottomleftX pbottomleftY

global Nh Nit J
global Nwx Nwy Nws
global Np Eps matr
global freq sigma
global pData
global pcal

set fileHead { 0 0 0 0 0 0 0 }; # dieleNum recNum cirNum triNum ellNum polyNum gndNum
set palFlag -1
set corFlag -1; # initially set to new mode
set newFlag -1
set yesMove -1
set curPosition -1

set recCounter 0
set cirCounter 0
set dieleCounter 0
set ellCounter 0
set triCounter 0
set polyCounter 0
set gndCounter 0

set recId ""
set cirId ""
set dieleId ""
set ellId ""
set triId ""
set polyId ""
set gndId ""

set recData ""
set cirData ""
set dieleData ""
set ellData ""
set triData ""
set polyData ""
set gndData ""

set recTags ""
set cirTags ""
set dieleTags ""
set ellTags ""
set triTags ""
set polyTags ""
set gndTags ""
      
set pData ""
set pcal  ""

set maxXcoord 879.00000
set maxYcoord 586.00000

set paletteXsize 0
set paletteYsize 0

set pbottomleftX 0
set pbottomleftY 0

set Xscale 0
set Yscale 0

set Nh 1024
set Nit 40
set J 6
 
set Nwx 8
set Nwy 10
set Nws 5

set Np 0.0
set Eps 1.0e-5
set matr 1

set freq 1.0e6
set sigma 5.6e7

#########################################################################
#     NAME:    global_init
#     PURPOSE: define the global variables which decide the state of
#              display mode
#
#########################################################################

proc global_init { } {

    global palettebkcolor


#    global board
#    catch {unset board}
#    array set board {}

    createMenu

}

#########################################################################
#     NAME: createMenu
#     PURPOSE: create the main menu
#
#########################################################################

proc createMenu { } {
    frame .mbar -relief raised -bd 2
#    frame .draw -relief raised -bd 2
    frame .dummy1 -relief raised -width 20c -height 0.05c -background blue  
    frame .dummy -width 24c -height 0.5c -background black

#############################################################################################

    global myAppFileName

    canvas .c -width 24c -height 16c -background black \
	-xscrollcommand ".shoriz set" \
	-yscrollcommand ".svert set"

    scrollbar .svert  -orient v -command ".c yview"
    scrollbar .shoriz -orient h -command ".c xview" 

#    grid .c      -row 0 -column 0 -columnspan 3 -sticky news
#    grid .svert  -row 0 -column 3 -columnspan 1 -sticky ns
#    grid .shoriz -row 1 -column 0 -columnspan 3 -sticky ew

    frame .butt

    button .butt.rect -text "Rectangle" -command mkRect -background lightblue -foreground black
    button .butt.cir -text "Circle" -command mkCir -background lightblue -foreground black
    button .butt.poly -text "Trapezoid" -command mkPoly -background lightblue -foreground black 
    button .butt.gnd -text "Ground Plate" -command mkGnd -background lightblue -foreground black

    button .butt.move -text "Move" -command moveUnit -background lightyellow -foreground black
    button .butt.delete -text "Delete" -command delUnit -background lightyellow -foreground black

#    button .butt1.zoomin -text "Zoom In" -command "zoomFactor .c 1.25"
#    button .butt1.zoomout -text "Zoom Out" -command "zoomFactor .c 0.8"
#    grid .butt.move .butt.delete ;#.butt1.zoomin .butt1.zoomout

    label .dummy.status -text " Status Bar " -background orange
    label .dummy.statusCon -text "You are welcome to use this Program!" -background black 
###    button .dummy.eplicon -bitmap @epl.bit -background black -foreground white -command eplIntro
    button .dummy.eplicon -text epl -background black -foreground white -command eplIntro
##############################################################################################
 
    pack .mbar .c .butt .dummy1 .dummy -side top -fill x

#    pack .draw.svert .draw.c  -side right -fill y
#    grid .butt.rect .butt.cir .butt.poly .butt.gnd
#    pack .butt1.move .butt1.delete -side left
    pack .butt.rect .butt.cir .butt.poly .butt.gnd -side left
    pack .butt.move .butt.delete -side right
    pack .dummy.eplicon .dummy.status .dummy.statusCon -side left
    

    menubutton .mbar.file -text File  \
	-menu .mbar.file.menu
    menubutton .mbar.edit -text Edit  \
	-menu .mbar.edit.menu
    menubutton .mbar.setup -text Setup  \
	-menu .mbar.setup.menu
    menubutton .mbar.handcreate -text HandCreate  \
	-menu .mbar.handcreate.menu
    menubutton .mbar.dropcreate -text DropCreate  \
	-menu .mbar.dropcreate.menu
    menubutton .mbar.calculate -text Calculate  \
	-menu .mbar.calculate.menu
    menubutton .mbar.help -text About  \
	-menu .mbar.help.menu
    pack .mbar.file .mbar.edit .mbar.setup \
	.mbar.handcreate .mbar.dropcreate .mbar.calculate -side left
    pack .mbar.help -side right

    menu .mbar.file.menu
    .mbar.file.menu add command -label "New"  \
	-command myAppFileNew
    .mbar.file.menu add command -label "Open"  \
	-command myAppFileOpen
    .mbar.file.menu add command -label "Save"  \
	-command {myAppFileSave $myAppFileName}
    .mbar.file.menu add command -label "Save As"  \
	-command myAppFileSaveAs
    .mbar.file.menu add command -label "Close"  \
	-command myAppFileClose
    .mbar.file.menu add separator
    .mbar.file.menu add cascade -label "Print"  \
	-menu .mbar.file.menu.print
    .mbar.file.menu add separator
    .mbar.file.menu add command -label "Exit"  \
	-command myAppExit

    menu .mbar.file.menu.print
    .mbar.file.menu.print add command -label "Geometrical Parameters" \
	    -command printPara
    .mbar.file.menu.print add command -label "Calculation Parameters" \
	    -command printcal
    .mbar.file.menu.print add command -label "Print Canvas" \
	    -command printGraph

    menu .mbar.edit.menu
    .mbar.edit.menu add command -label "Move" \
	-command moveUnit
    .mbar.edit.menu add command -label "Delete" \
	-command delUnit
    .mbar.edit.menu add separator
    .mbar.edit.menu add command -label "Geometry" \
	-command editPara

    menu .mbar.setup.menu
    .mbar.setup.menu add command -label "Option" \
	-command \
	{ Coption .palettePara { Palette Parameters } 0 }
    .mbar.setup.menu add command -label "Parameters"  \
	-command \
	{ calpara .calPara { Computational Parameters } }

    menu .mbar.handcreate.menu
    .mbar.handcreate.menu add command -label "Rectangle" \
	-command { 
	    set corFlag -1
##	    puts $corFlag
	    dispRect .rectPara { Rectangular Conductor Parameters } {}
	}
    .mbar.handcreate.menu add command -label "Circle" \
	-command { 
	    set corFlag -1
##	    puts $corFlag
	    dispCir .cirPara { Circular Conductor Parameters } {} 
	}
    .mbar.handcreate.menu add command -label "Trapezoid" \
	-command {
	    set corFlag -1
##	    puts $corFlag
	    getApexnum
	}
    .mbar.handcreate.menu add command -label "Ground" \
	-command {
	    set corFlag -1
##	    puts $corFlag
	    dispGnd .gndPara { Ground Plane Parameters } {}
	}

    menu .mbar.dropcreate.menu
    .mbar.dropcreate.menu add command -label "Rectangle" \
	-command mkRect
    .mbar.dropcreate.menu add command -label "Circle" \
	-command mkCir
    .mbar.dropcreate.menu add command -label "Trapezoid" \
	-command mkPoly
    .mbar.dropcreate.menu add command -label "Ground" \
	-command mkGnd

    menu .mbar.calculate.menu
    .mbar.calculate.menu add command -label "Check Data" \
	-command { startcal1 1 }
    .mbar.calculate.menu add command -label "Start Calculation" \
	-command { startcal1 2 }
    .mbar.calculate.menu add command \
	-label "Start Frequency-List Calculation" \
	-command { buildFreqList }
    .mbar.calculate.menu add command \
	-label "Start Calculation - Append Output" \
	-command { startcal1 3 }
    .mbar.calculate.menu add command -label "Check Results"  \
	-command { checkResult }

    menu .mbar.help.menu
    .mbar.help.menu add command -label "About..." \
	-command about

    tk_menuBar .mbar .mbar.file .mbar.edit .mbar.view .mbar.setup \
    .mbar.handcreate .mbar.dropcreate .mbar.calculate .mbar.help
    focus .mbar
}

#########################################################################
#     NAME: editPara
#     PURPOSE: edit the  parameters of single objects in the palette
#
#########################################################################

proc editPara { } {

    global dieleData recData cirData triData ellData polyData gndData
    global recTags cirTags dieleTags ellTags triTags polyTags gndTags
    global recId cirId dieleId ellId triId polyId gndId

    global palFlag

    global newFlag

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    }
    
    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    }

    bind .c <Motion> {
	set realX [expr {  %x/$Xscale+$pbottomleftX }]
	set realY [expr {  ($maxYcoord-%y)/$Yscale+$pbottomleftY }]
	.dummy.statusCon configure -text "x=$realX, y=$realY"
    }
    
    bind .c <Button-1> {

	set curPoint [.c find withtag current]
##	puts $curPoint
	
	if { [lsearch -exact $recId $curPoint] != -1 } {
	    setTag $curPoint $recId $recTags $recData
	    set corFlag 0
##	    puts $corFlag
##	    puts "current position: $curPosition" 
	    set title "Correct Rectangle No. " 
	    set title1 [lindex $recData [expr {  $curPosition+1 }]]
	    set title $title$title1
	    dispRect .rectPara $title $curPosition
	} elseif { [lsearch -exact $cirId $curPoint] != -1 } {
	    setTag $curPoint $cirId $cirTags $cirData
	    set corFlag 0;# set the correcting mode
##	    puts $corFlag
	    set title "Correct Circle No. " 
	    set title1 [lindex $cirData [expr {  $curPosition+1 }]]
	    set title $title$title1
	    dispCir .cirPara $title $curPosition
	} elseif { [lsearch -exact $dieleId $curPoint] != -1 } {
	    setTag $curPoint $dieleId $dieleTags $dieleData
	    set corFlag 0;# set the correcting mode
##	    puts $corFlag
	    set title "Correct Dielectric No. " 
	    set title1 [lindex $dieleData [expr {  $curPosition+1 }]]
	    set title $title$title1
	    dispDiele .dielePara $title $curPosition
	} elseif { [lsearch -exact $triId $curPoint] != -1 } {
	    setTag $curPoint $triId $triTags $triData
	    set corFlag 0;# set the correcting mode
##	    puts $corFlag
	    set title "Correct Triangle No. " 
	    set title1 [lindex $triData [expr {  $curPosition+1 }]]
	    set title $title$title1
	    dispTri .triPara $title $curPosition
	} elseif { [lsearch -exact $ellId $curPoint] != -1 } {
	    setTag $curPoint $ellId $ellTags $ellData
	    set corFlag 0;# set the correcting mode
##	    puts $corFlag
	    set title "Correct Ellipse No. " 
	    set title1 [lindex $ellData [expr {  $curPosition+1 }]]
	    set title $title$title1
	    dispEll .ellPara $title $curPosition
	} elseif { [lsearch -exact $polyId $curPoint] != -1 } {
	    setTag $curPoint $polyId $polyTags $polyData
	    set corFlag 0;# set the correcting mode
##	    puts $corFlag
	    set title "Correct Trapezoid No. " 
	    set title1 [lindex $polyData [expr {  $curPosition+1 }]]
	    set title $title$title1
	    dispPoly .polyPara $title \
		[lindex $polyData [expr {  $curPosition+2 }]] $curPosition
	} elseif { [lsearch -exact $gndId $curPoint] != -1 } {
	    setTag $curPoint $gndId $gndTags $gndData
	    set corFlag 0;# set the correcting mode
##	    puts $corFlag
	    set title "Correct Ground No. " 
	    set title1 [lindex $gndData [expr {  $curPosition+1 }]]
	    set title $title$title1
	    dispGnd .gndPara $title $curPosition
	}  
    }
}



################################################################################################
#           Here below is to draw different shapes
#           
################################################################################################

proc mkRect { } {

    global firstnodex firstnodey
    global secondnodex secondnodey
    global tempnodex tempnodey
    global buttonflag

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY

    global palFlag
    global newFlag
    global myAppChangedFlag

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 
	
    set buttonflag 0
    .c config -cursor { cross red white }

    bind .c <Motion> {

##    puts "(%x %y)"

    set realX [expr {  %x/$Xscale+$pbottomleftX }]
    set realY [expr {  ($maxYcoord-%y)/$Yscale+$pbottomleftY }]
##    puts "begin1"
##    puts $pbottomleftY
##    puts $Yscale
##    puts $maxYcoord
##    puts $paletteYsize
##    puts $paletteXsize
    
##    puts "end1"
    .dummy.statusCon configure -text "x=$realX, y=$realY"
    
    if { $buttonflag == 1 } {
    
    .c delete interrect
    .c create rectangle $tempnodex $tempnodey %x %y -outline blue \
	-tags interrect
    }
}

bind .c <Button-1> {

    if { $buttonflag == 1 } {
	.c delete interrect
	incr recCounter

	set rect [.c create rectangle $tempnodex $tempnodey %x %y \
		      -outline black -fill yellow -tags recTags$recCounter ]

	lappend recId $rect
	lappend recTags recTags$recCounter

##	puts $recTags

	set firstnodex($rect) $tempnodex
	set firstnodey($rect) $tempnodey
	set secondnodex($rect) %x
	set secondnodey($rect) %y
##	puts $rect
##	puts $firstnodex($rect)
##	puts $firstnodey($rect)
##	puts $secondnodex($rect)
##	puts $secondnodey($rect)
	set buttonflag 0

	set fileHead [lreplace $fileHead 1 1 [expr {  [lindex $fileHead 1]+1 }]]
##	puts $fileHead

	lappend recData "REC" 
	lappend recData $rect
	lappend recData recTags$recCounter
	lappend recData $recCounter

	if { $firstnodex($rect) <= $secondnodex($rect) } {
	    set bottomleftX [expr {  $firstnodex($rect)/$Xscale+$pbottomleftX }]
	} else {
	    set bottomleftX [expr {  $secondnodex($rect)/$Xscale+$pbottomleftX }]
	}
	
	if { $firstnodey($rect) >= $secondnodey($rect) } {
	    set bottomleftY [expr {  ($maxYcoord-$firstnodey($rect))/$Yscale+$pbottomleftY }]
	} else {
	    set bottomleftY [expr {  ($maxYcoord-$secondnodey($rect))/$Yscale+$pbottomleftY }]
	}

	lappend recData $bottomleftX
	lappend recData $bottomleftY

	lappend recData [expr {  abs($firstnodex($rect)-$secondnodex($rect))/$Xscale }]
	lappend recData [expr {  abs($firstnodey($rect)-$secondnodey($rect))/$Yscale }]
	
	lappend recData 0

	set myAppChangedFlag 1

##	puts $recData

	#.c config -cursor { pointer black white }
    } else {
	set tempnodex %x
	set tempnodey %y
	set buttonflag 1
	
    }
}

bind .c <Button-3> {
    .c delete interrect
    set buttonflag 0
    #.c config -cursor { pointer black white }
}

return

}    


proc mkCir { } {
    global firstnodex firstnodey
    global secondnodex secondnodey
    global tempnodex tempnodey
    global buttonflag

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global palFlag newFlag

    global myAppChangedFlag

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    .c config -cursor { cross red white }
    set buttonflag 0

    bind .c <Motion> {
	
##	puts "(%x %y)"
	set realX [expr {  %x/$Xscale+$pbottomleftX }]
	set realY [expr {  ($maxYcoord-%y)/$Yscale+$pbottomleftY }]
	.dummy.statusCon configure -text "x=$realX, y=$realY"
	
	if { $buttonflag == 1 } {
	    
	    .c delete interrect
	    set radius [expr {  hypot(($tempnodex-%x),($tempnodey-%y)) }]
##	    puts $radius
	    #set radius [expr {  hypot([expr {  $tempnodex-%x }],[expr {  $tempnodey-%y }]) }]
	    .c create oval [expr {  $tempnodex-$radius }] [expr {  $tempnodey-$radius }] \
		[expr {  $tempnodex+$radius }] [expr {  $tempnodey+$radius }] -outline blue -tags interrect
	}
    }

bind .c <Button-1> {

    if { $buttonflag == 1 } {
	.c delete interrect
	set radius [expr {  hypot(($tempnodex-%x),($tempnodey-%y)) }]
##	puts $radius
	incr cirCounter
	set cir [.c create oval [expr {  $tempnodex-$radius }] [expr {  $tempnodey-$radius }]  \
		     [expr {  $tempnodex+$radius }] [expr {  $tempnodey+$radius }] \
		     -outline black -fill red -tags cirTags$cirCounter]
	lappend cirId $cir
	lappend cirTags cirTags$cirCounter

	#lappend allTags cirTags$cirCounter
	set firstnodex($cir) $tempnodex
	set firstnodey($cir) $tempnodey
	set secondnodex($cir) %x
	set secondnodey($cir) %y
##	puts $cir
##	puts $firstnodex($cir)
##	puts $firstnodey($cir)
##	puts $secondnodex($cir)
##	puts $secondnodey($cir)
	set buttonflag 0

	set fileHead [lreplace $fileHead 2 2 [expr {  [lindex $fileHead 2]+1 }]]
##	puts $fileHead

	lappend cirData "CIR" 
	lappend cirData $cir
#	lappend cirData cirTags$temptag
	lappend cirData cirTags$cirCounter
	lappend cirData $cirCounter

	lappend cirData [expr {  $firstnodex($cir)/$Xscale+$pbottomleftX }]
	lappend cirData [expr {  ($maxYcoord-$firstnodey($cir))/$Yscale+$pbottomleftY }]

	lappend cirData [expr {  hypot(($firstnodex($cir)-$secondnodex($cir))/$Xscale, \
		($firstnodey($cir)-$secondnodey($cir))/$Yscale) }]
	
	lappend cirData 0

	set myAppChangedFlag 1

##	puts $cirData


	#.c config -cursor { pointer black white }
    } else {
	set tempnodex %x
	set tempnodey %y
	set buttonflag 1
	
    }
}

bind .c <Button-3> {

    .c delete interrect
    set buttonflag 0
    #.c config -cursor { pointer black white }
}

return

}

proc mkDiele { } {
    global firstnodex firstnodey
    global secondnodex secondnodey
    global tempnodex tempnodey
    global buttonflag

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global palFlag newFlag
    global myAppChangedFlag

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    .c config -cursor { cross red white }
    set buttonflag 0

    bind .c <Motion> {

##	puts "(%x %y)"
	set realX [expr {  %x/$Xscale+$pbottomleftX }]
	set realY [expr {  ($maxYcoord-%y)/$Yscale+$pbottomleftY }]
	.dummy.statusCon configure -text "x=$realX, y=$realY"
    
	if { $buttonflag == 1 } {
    
	    .c delete interrect
	    .c create rectangle $tempnodex $tempnodey %x %y -outline blue \
		-tags interrect
	}
    }
    
    bind .c <Button-1> {
	
	if { $buttonflag == 1 } {
	    .c delete interrect
	    incr dieleCounter
	    set diele [.c create rectangle $tempnodex $tempnodey %x %y \
			   -outline black -fill black -tags dieleTags$dieleCounter]
	    lappend dieleId $diele
	    lappend dieleTags dieleTags$dieleCounter
	    
	    .c lower $diele
	    set firstnodex($diele) $tempnodex
	    set firstnodey($diele) $tempnodey
	    set secondnodex($diele) %x
	    set secondnodey($diele) %y
##	    puts $diele
##	    puts $firstnodex($diele)
##	    puts $firstnodey($diele)
##	    puts $secondnodex($diele)
##	    puts $secondnodey($diele)
	    set buttonflag 0
	    
	    set fileHead [lreplace $fileHead 0 0 [expr {  [lindex $fileHead 0]+1 }]]
##	    puts $fileHead

	    lappend dieleData "DIELE" 
	    lappend dieleData $diele
	    lappend dieleData dieleTags$dieleCounter
	    
	    lappend dieleData $dieleCounter
	    
	    if { $firstnodex($diele) <= $secondnodex($diele) } {
		set bottomleftX [expr {  $firstnodex($diele)/$Xscale+$pbottomleftX }]
	    } else {
		set bottomleftX [expr {  $secondnodex($diele)/$Xscale+$pbottomleftX }]
	    }
	    
	    if { $firstnodey($diele) >= $secondnodey($diele) } {
		set bottomleftY [expr {  ($maxYcoord-$firstnodey($diele))/$Yscale+$pbottomleftY }]
	    } else {
		set bottomleftY [expr {  ($maxYcoord-$secondnodey($diele))/$Yscale+$pbottomleftY }]
	}
	    
	    lappend dieleData $bottomleftX
	    lappend dieleData $bottomleftY

	    lappend dieleData [expr {  abs($firstnodex($diele)-$secondnodex($diele))/$Xscale }]
	    lappend dieleData [expr {  abs($firstnodey($diele)-$secondnodey($diele))/$Yscale }]
	
	    lappend dieleData 0 ;#permit
	    lappend dieleData 0 ;#permea

	    set myAppChangedFlag 1

##	    puts $dieleData

	    
	    #.c config -cursor { pointer black white }
	} else {
	    set tempnodex %x
	    set tempnodey %y
	    set buttonflag 1
	
	}
    }

    bind .c <Button-3> {
	
	.c delete interrect
	set buttonflag 0
	#.c config -cursor { pointer black white }
    }

    return
    
}    

proc mkGnd { } {
    global firstnodex firstnodey
    global secondnodex secondnodey
    global tempnodex tempnodey
    global buttonflag

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global palFlag newFlag
    global myAppChangedFlag

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 


    .c config -cursor { cross red white }
    set buttonflag 0
    
    bind .c <Motion> {
	
##	puts "(%x %y)"
	set realX [expr {  %x/$Xscale+$pbottomleftX }]
	set realY [expr {  ($maxYcoord-%y)/$Yscale+$pbottomleftY }]
	.dummy.statusCon configure -text "x=$realX, y=$realY"
	
	if { $buttonflag == 1 } {
	    
	    .c delete interrect
	    .c create rectangle $tempnodex $tempnodey %x %y -outline blue \
		-tags interrect
	}
    }
    
    bind .c <Button-1> {
	
	if { $buttonflag == 1 } {
	    .c delete interrect
	    incr gndCounter
	    set gnd [.c create rectangle $tempnodex $tempnodey %x %y \
		     -outline black -fill green -tags gndTags$gndCounter]
	    lappend gndId $gnd
	    
	    lappend gndTags gndTags$gndCounter
	    set firstnodex($gnd) $tempnodex
	    set firstnodey($gnd) $tempnodey
	    set secondnodex($gnd) %x
	    set secondnodey($gnd) %y
##	    puts $gnd
##	    puts $firstnodex($gnd)
##	    puts $firstnodey($gnd)
##	    puts $secondnodex($gnd)
##	    puts $secondnodey($gnd)
	    set buttonflag 0
	    
	    set fileHead [lreplace $fileHead 6 6 [expr {  [lindex $fileHead 6]+1 }]]
##	    puts $fileHead
	    
	    lappend gndData "GND" 
	    lappend gndData $gnd
	    
	    lappend gndData gndTags$gndCounter
	    lappend gndData $gndCounter

	    if { $firstnodex($gnd) <= $secondnodex($gnd) } {
		set bottomleftX [expr {  $firstnodex($gnd)/$Xscale+$pbottomleftX }]
	    } else {
		set bottomleftX [expr {  $secondnodex($gnd)/$Xscale+$pbottomleftX }]
	    }
	    
	    if { $firstnodey($gnd) >= $secondnodey($gnd) } {
		set bottomleftY [expr {  ($maxYcoord-$firstnodey($gnd))/$Yscale+$pbottomleftY }]
	    } else {
		set bottomleftY [expr {  ($maxYcoord-$secondnodey($gnd))/$Yscale+$pbottomleftY }]
	    }

	    lappend gndData $bottomleftX
	    lappend gndData $bottomleftY
	    
	    lappend gndData [expr {  abs($firstnodex($gnd)-$secondnodex($gnd))/$Xscale }]
	    lappend gndData [expr {  abs($firstnodey($gnd)-$secondnodey($gnd))/$Yscale }]
	    
	    lappend gndData 0
	    
	    set myAppChangedFlag 1

##	    puts $gndData
	    
	    #.c config -cursor { pointer black white }
	} else {
	    set tempnodex %x
	    set tempnodey %y
	    set buttonflag 1
	    
	}
    }
    
    bind .c <Button-3> {
	
	.c delete interrect
	set buttonflag 0
	#.c config -cursor { pointer black white }
    }
    
    return
    
}    

proc mkTri { } {

    global firstnodex firstnodey
    global secondnodex secondnodey
    global thirdnodex thirdnodey
    global tempnodex tempnodey
    global tempnodex2 tempnodey2
    global buttonflag

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global palFlag newFlag
    global myAppChangedFlag

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    .c config -cursor { cross red white }
    set buttonflag 0

    bind .c <Motion> {

##	puts "(%x %y)"
	set realX [expr {  %x/$Xscale+$pbottomleftX }]
	set realY [expr {  ($maxYcoord-%y)/$Yscale+$pbottomleftY }]
	.dummy.statusCon configure -text "x=$realX, y=$realY"
    
	if { $buttonflag == 1 } {
	    
	    .c delete interrect
	    .c create line $tempnodex $tempnodey %x %y \
		-tags interrect
	} elseif { $buttonflag == 2 } {
	    .c delete interrect
	    .c create  polygon $tempnodex $tempnodey $tempnodex2 $tempnodey2 %x %y \
		-outline blue -tags interrect	
	}
    }
    
    bind .c <Button-1> {
	
	if { $buttonflag == 2 } {
	    .c delete interrect
	    incr triCounter
	    set tri [.c create polygon $tempnodex $tempnodey \
			 $tempnodex2 $tempnodey2 %x %y \
			 -outline black -fill white -tags triTags$triCounter]
	    lappend triId $tri
	    
	    lappend triTags triTags$triCounter
	    set firstnodex($tri) $tempnodex
	    set firstnodey($tri) $tempnodey
	    set secondnodex($tri) $tempnodex2
	    set secondnodey($tri) $tempnodey2
	    set thirdnodex($tri) %x
	    set thirdnodey($tri) %y
##	    puts $tri
##	    puts $firstnodex($tri)
##	    puts $firstnodey($tri)
##	    puts $secondnodex($tri)
##	    puts $secondnodey($tri)
##	    puts $thirdnodex($tri)
##	    puts $thirdnodey($tri)
	    set buttonflag 0
	    
	    set fileHead [lreplace $fileHead 3 3 [expr {  [lindex $fileHead 3]+1 }]]
##	    puts $fileHead
	    
	    lappend triData "TRI" 
	    lappend triData $tri
	    #	lappend triData triTags$temptag
	    lappend triData triTags$triCounter
	    lappend triData $triCounter
	    
	    lappend triData [expr {  $firstnodex($tri)/$Xscale+$pbottomleftX }]
	    lappend triData [expr {  ($maxYcoord-$firstnodey($tri))/$Yscale+$pbottomleftY }]
	    
	    lappend triData [expr {  $secondnodex($tri)/$Xscale+$pbottomleftX }]
	    lappend triData [expr {  ($maxYcoord-$secondnodey($tri))/$Yscale+$pbottomleftY }]
	    
	    lappend triData [expr {  $thirdnodex($tri)/$Xscale+$pbottomleftX }]
	    lappend triData [expr {  ($maxYcoord-$thirdnodey($tri))/$Yscale+$pbottomleftY }]

	    lappend triData 0

	    set myAppChangedFlag 1

##	    puts $triData
	    
	    #.c config -cursor { pointer black white }
	} elseif { $buttonflag == 1 } { 
	    .c delete interrect
	    .c create line $tempnodex $tempnodey %x %y \
		-tags interrect
	    set tempnodex2 %x
	    set tempnodey2 %y
	    set buttonflag 2
	} else {
	    set tempnodex %x
	    set tempnodey %y
	    set buttonflag 1
	    
	}
    }
    
    bind .c <Button-3> {
	
	.c delete interrect
	set buttonflag 0
	#.c config -cursor { pointer black white }
    }
    
    return
    
}    


proc mkEll { } {
    global firstnodex firstnodey
    global secondnodex secondnodey
    global tempnodex tempnodey
    global buttonflag

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global palFlag newFlag
    global myAppChangedFlag

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    .c config -cursor { cross red white }
    set buttonflag 0
    
    bind .c <Motion> {
	
##	puts "(%x %y)"
	set realX [expr {  %x/$Xscale+$pbottomleftX }]
	set realY [expr {  ($maxYcoord-%y)/$Yscale+$pbottomleftY }]
	.dummy.statusCon configure -text "x=$realX, y=$realY"
    
	if { $buttonflag == 1 } {
	    .c delete interrect
	    set majoraxis [expr {  sqrt(2)*($tempnodex-%x) }]
	    set minoraxis [expr {  sqrt(2)*($tempnodey-%y) }]
	    .c create oval [expr {  $tempnodex-$majoraxis }] [expr {  $tempnodey-$minoraxis }] \
		[expr {  $tempnodex+$majoraxis }] [expr {  $tempnodey+$minoraxis }] \
		-outline blue -tags interrect
	}
    }
    
    bind .c <Button-1> {
	
	if { $buttonflag == 1 } {
	    .c delete interrect
	    set majoraxis [expr {  sqrt(2)*($tempnodex-%x) }]
	    set minoraxis [expr {  sqrt(2)*($tempnodey-%y) }]
	    incr ellCounter
	    set ell [.c create oval [expr {  $tempnodex-$majoraxis }] [expr {  $tempnodey-$minoraxis }] \
			 [expr {  $tempnodex+$majoraxis }] [expr {  $tempnodey+$minoraxis }] \
			 -outline black -fill white -tags ellTags$ellCounter]
	    lappend ellId $ell
	    #	set temptag $ell
	    #	lappend ellTags ellTags$temptag
	    lappend ellTags ellTags$ellCounter
	    set firstnodex($ell) $tempnodex
	    set firstnodey($ell) $tempnodey
	    set secondnodex($ell) %x
	    set secondnodey($ell) %y
##	    puts $ell
##	    puts $firstnodex($ell)
##	    puts $firstnodey($ell)
##	    puts $secondnodex($ell)
##	    puts $secondnodey($ell)
	    set buttonflag 0
	    
	    set fileHead [lreplace $fileHead 4 4 [expr {  [lindex $fileHead 4]+1 }]]
##	    puts $fileHead
	    
	    lappend ellData "ELL" 
	    lappend ellData $ell
	    #	lappend ellData ellTags$temptag
	    lappend ellData ellTags$ellCounter
	    lappend ellData $ellCounter
	    
	    lappend ellData [expr {  $firstnodex($ell)/$Xscale+$pbottomleftX }]
	    lappend ellData [expr {  ($maxYcoord-$firstnodey($ell))/$Yscale+$pbottomleftY }]
	    
	    lappend ellData [expr {  abs($majoraxis)/$Xscale }]
	    lappend ellData [expr {  abs($minoraxis)/$Yscale }]
	    
	    lappend ellData 0

	    set myAppChangedFlag 1

##	    puts $ellData
	    
	    
	    #.c config -cursor { pointer black white }
	} else {
	    set tempnodex %x
	    set tempnodey %y
	    set buttonflag 1
	    
	}
    }
    
    bind .c <Button-3> {
	.c delete interrect
	set buttonflag 0
	#.c config -cursor { pointer black white }
    }
    
    return
    
}    

proc mkPoly { } {
    global apexnum
    global xcoord ycoord xycoord
    global apexorder
    global tempnodex tempnodey
    global tempnodex2 tempnodey2
    global buttonflag

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global palFlag newFlag
    global myAppChangedFlag

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    getApexnumdrop

    set buttonflag 0
    set apexorder 0
    .c config -cursor { cross red white }
   
    bind .c <Motion> {
##	puts "(%x %y)"
	set realX [expr {  %x/$Xscale+$pbottomleftX }]
	set realY [expr {  ($maxYcoord-%y)/$Yscale+$pbottomleftY }]
	.dummy.statusCon configure -text "x=$realX, y=$realY"
    
	if { $apexorder > 0 && $apexorder < $apexnum } {
	    .c delete curside
	    .c create line [lindex $xcoord [expr {  $apexorder-1 }]] \
		[lindex $ycoord [expr {  $apexorder-1 }]] %x %y -tags curside 
	}
    }
    bind .c <Button-1> {
	if { $apexorder == 0 } {
	    set xcoord %x
	    set ycoord %y
	    set xycoord %x
	    lappend xycoord %y
	    incr apexorder 
	} elseif { $apexorder > 0 && $apexorder < [expr {  $apexnum-1 }] } {
	    lappend xcoord %x
	    lappend ycoord %y
	    lappend xycoord %x
	    lappend xycoord %y
	    incr apexorder
	    .c delete interrect
	    set counter 0
	    while { $counter < [expr {  $apexorder-1 }] } {
##		puts $counter
##		puts $apexorder
##		puts [lindex $xcoord $counter]
##		puts [lindex $ycoord $counter]
##		puts [lindex $xcoord [expr {  $counter+1 }]]
##		puts [lindex $ycoord [expr {  $counter+1 }]]
##		puts $xcoord
##		puts $ycoord
##		puts $xycoord
		
		.c create line [lindex $xcoord $counter] [lindex $ycoord $counter] \
		    [lindex $xcoord [expr {  $counter+1 }]] [lindex $ycoord [expr {  $counter+1 }]] \
		    -tags interrect 
		incr counter
	    }
	
	} elseif { $apexorder == [expr {  $apexnum-1 }] } {
	    lappend xcoord %x
	    lappend ycoord %y
	    lappend xycoord %x
	    lappend xycoord %y
	    .c delete interrect
	    .c delete curside
	    incr polyCounter
	    set poly [.c create polygon $xycoord -outline black -fill \
			  blue -tags polyTags$polyCounter]
	    lappend polyId $poly
#	    set temptag $poly
#	    lappend polyTags polyTags$temptag
	    lappend polyTags polyTags$polyCounter
	    set buttonflag 0
            set apexorder 0

	    set fileHead [lreplace $fileHead 5 5 [expr {  [lindex $fileHead 5]+1 }]]
##	    puts $fileHead

	    lappend polyData "POLY" 
	    lappend polyData $poly
#	    lappend polyData polyTags$temptag
	    lappend polyData polyTags$polyCounter
	    lappend polyData $polyCounter

	    lappend polyData [expr {  [llength $xycoord]/2 }]

##	    puts [llength $xycoord]

	    for {set i 0} {$i < [expr {  [llength $xycoord]/2 }]} {incr i} {
		lappend polyData [expr {  [lindex $xycoord [expr {  2*$i }]]/$Xscale+$pbottomleftX }]
		lappend polyData [expr {  ($maxYcoord-[lindex $xycoord [expr {  2*$i+1 }]])/$Yscale \
				      +$pbottomleftY }]
	    }

	    lappend polyData 0

	    set myAppChangedFlag 1

##	    puts $polyData
        }	      
    }

    bind .c <Button-3> {
	.c delete interrect
	.c delete curside
	set buttonflag 0
	set apexorder 0
	#.c config -cursor { pointer black white }
    }

    return
}


#########################################################################################
#             Move Unit
#
#########################################################################################

proc moveUnit { } {

    global dieleData recData cirData triData ellData polyData gndData
    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global newFlag
    global myAppChangedFlag


    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    bind .c <Button-1> {	
	if { [lsearch -exact $recId $curPoint] != -1 
	     || [lsearch -exact $cirId $curPoint] != -1 
	     || [lsearch -exact $dieleId $curPoint] != -1
	     || [lsearch -exact $ellId $curPoint] != -1
	     || [lsearch -exact $triId $curPoint] != -1
	     || [lsearch -exact $polyId $curPoint] != -1
	     || [lsearch -exact $gndId $curPoint] != -1 } {
	    
	    set curX %x
	    set curY %y
	    set oriX %x
	    set oriY %y

	} else {}
    }
    
    bind .c <B1-Motion> {

	set curPoint [.c find withtag current]
##	puts $curPoint
	
	if { [lsearch -exact $recId $curPoint] != -1 
	     || [lsearch -exact $cirId $curPoint] != -1 
	     || [lsearch -exact $dieleId $curPoint] != -1
	     || [lsearch -exact $ellId $curPoint] != -1
	     || [lsearch -exact $triId $curPoint] != -1
	     || [lsearch -exact $polyId $curPoint] != -1
	     || [lsearch -exact $gndId $curPoint] != -1 } {

##	    puts "(%x %y)"
	    set realX [expr {  %x/$Xscale+$pbottomleftX }]
	    set realY [expr {  ($maxYcoord-%y)/$Yscale+$pbottomleftY }]
	    set deltaX [expr {  (%x-$oriX)/$Xscale }]
	    set deltaY [expr {  ($oriY-%y)/$Yscale }]
	    .dummy.statusCon configure -text "x=$realX, y=$realY, dx=$deltaX, dy=$deltaY"
	    	  
##	    puts $recId
##	    puts [lsearch -exact $recId $curPoint]
	    
	    if { [lsearch -exact $recId $curPoint] != -1 } {
		
##		puts [lsearch -exact $recId $curPoint]
##		puts [lindex $recTags [lsearch -exact $recId $curPoint]]
		
		.c move [lindex $recTags [lsearch -exact $recId $curPoint]] \
		    [expr {  %x-$curX }] [expr {  %y-$curY }]
		
		.c raise [lindex $recTags [lsearch -exact $recId $curPoint]]; #ensure on top of diele
		
		# the data of position should be changed and saved after movement
		set curX %x
		set curY %y
		
		bind .c <ButtonRelease-1> {
		    setTag $curPoint $recId $recTags $recData
		    set xnew [expr {  (%x-$oriX)/$Xscale }]
		    set ynew [expr {  (%y-$oriY)/$Yscale }]
		    set recData [lreplace $recData [expr {  $curPosition+2 }] [expr {  $curPosition+2 }] \
				     [expr {  [lindex $recData [expr {  $curPosition+2 }]]+$xnew }]]
		    set recData [lreplace $recData [expr {  $curPosition+3 }] [expr {  $curPosition+3 }] \
				     [expr {  [lindex $recData [expr {  $curPosition+3 }]]-$ynew }]]
		    set myAppChangedFlag 1
		    bind .c <Button-1> { }
		    bind .c <B1-Motion> { }
		    bind .c <ButtonRelease-1> { }	
		}
		
	    } elseif { [lsearch -exact $cirId $curPoint] != -1 } {
		
##		puts [lsearch -exact $cirId $curPoint]
##		puts [lindex $cirTags [lsearch -exact $cirId $curPoint]]
		
		.c move [lindex $cirTags [lsearch -exact $cirId $curPoint]] \
			[expr {  %x-$curX }] [expr {  %y-$curY }]
		
		.c raise [lindex $cirTags [lsearch -exact $cirId $curPoint]]
		
		set curX %x
		set curY %y
		
		bind .c <ButtonRelease-1> {
		    setTag $curPoint $cirId $cirTags $cirData
		    set xnew [expr {  (%x-$oriX)/$Xscale }]
		    set ynew [expr {  (%y-$oriY)/$Yscale }]
		    set cirData [lreplace $cirData [expr {  $curPosition+2 }] [expr {  $curPosition+2 }] \
			[expr {  [lindex $cirData [expr {  $curPosition+2 }]]+$xnew }]]
		    set cirData [lreplace $cirData [expr {  $curPosition+3 }] [expr {  $curPosition+3 }] \
				     [expr {  [lindex $cirData [expr {  $curPosition+3 }]]-$ynew }]]
		    set myAppChangedFlag 1
		    bind .c <Button-1> { }
		    bind .c <B1-Motion> { }
		    bind .c <ButtonRelease-1> { }
		}
		
	    } elseif { [lsearch -exact $dieleId $curPoint] != -1 } {
	    
##		puts [lsearch -exact $dieleId $curPoint]
##		puts [lindex $dieleTags [lsearch -exact $dieleId $curPoint]]
		
		.c move [lindex $dieleTags [lsearch -exact $dieleId $curPoint]] \
		[expr {  %x-$curX }] [expr {  %y-$curY }]
		
		.c lower [lindex $dieleTags [lsearch -exact $dieleId $curPoint]]
		
		set curX %x
		set curY %y

		bind .c <ButtonRelease-1> {
		    setTag $curPoint $dieleId $dieleTags $dieleData
		set xnew [expr {  (%x-$oriX)/$Xscale }]
		    set ynew [expr {  (%y-$oriY)/$Yscale }]
		    set dieleData [lreplace $dieleData [expr {  $curPosition+2 }] [expr {  $curPosition+2 }] \
				       [expr {  [lindex $dieleData [expr {  $curPosition+2 }]]+$xnew }]]
		set dieleData [lreplace $dieleData [expr {  $curPosition+3 }] [expr {  $curPosition+3 }] \
				   [expr {  [lindex $dieleData [expr {  $curPosition+3 }]]-$ynew }]]
		    set myAppChangedFlag 1
		    bind .c <Button-1> { }
		    bind .c <B1-Motion> { }
		    bind .c <ButtonRelease-1> { }
		}
		
	    } elseif { [lsearch -exact $ellId $curPoint] != -1 } {
		
##		puts [lsearch -exact $ellId $curPoint]
##		puts [lindex $ellTags [lsearch -exact $ellId $curPoint]]

		.c move [lindex $ellTags [lsearch -exact $ellId $curPoint]] \
		    [expr {  %x-$curX }] [expr {  %y-$curY }]
		
		.c raise [lindex $ellTags [lsearch -exact $ellId $curPoint]]
		
		set curX %x
		set curY %y 

		bind .c <ButtonRelease-1> {
		    setTag $curPoint $ellId $ellTags $ellData
		    set xnew [expr {  (%x-$oriX)/$Xscale }]
		    set ynew [expr {  (%y-$oriY)/$Yscale }]
		    set ellData [lreplace $ellData [expr {  $curPosition+2 }] [expr {  $curPosition+2 }] \
				     [expr {  [lindex $ellData [expr {  $curPosition+2 }]]+$xnew }]]
		    set ellData [lreplace $ellData [expr {  $curPosition+3 }] [expr {  $curPosition+3 }] \
				     [expr {  [lindex $ellData [expr {  $curPosition+3 }]]-$ynew }]]
		    set myAppChangedFlag 1
		    bind .c <Button-1> { }
		    bind .c <B1-Motion> { }
		    bind .c <ButtonRelease-1> { }
		}

	    } elseif { [lsearch -exact $triId $curPoint] != -1 } {
		
##		puts [lsearch -exact $triId $curPoint]
##		puts [lindex $triTags [lsearch -exact $triId $curPoint]]
		
		.c move [lindex $triTags [lsearch -exact $triId $curPoint]] \
		    [expr {  %x-$curX }] [expr {  %y-$curY }]
		
		.c raise [lindex $triTags [lsearch -exact $triId $curPoint]]
		
		set curX %x
		set curY %y
		
		bind .c <ButtonRelease-1> {
		    setTag $curPoint $triId $triTags $triData
		    set xnew [expr {  (%x-$oriX)/$Xscale }]
		    set ynew [expr {  (%y-$oriY)/$Yscale }]
		    set triData [lreplace $triData [expr {  $curPosition+2 }] [expr {  $curPosition+2 }] \
				     [expr {  [lindex $triData [expr {  $curPosition+2 }]]+$xnew }]]
		    set triData [lreplace $triData [expr {  $curPosition+3 }] [expr {  $curPosition+3 }] \
				     [expr {  [lindex $triData [expr {  $curPosition+3 }]]-$ynew }]]
		    set triData [lreplace $triData [expr {  $curPosition+4 }] [expr {  $curPosition+4}] \
				     [expr {  [lindex $triData [expr {  $curPosition+4 }]]+$xnew }]]
		    set triData [lreplace $triData [expr {  $curPosition+5 }] [expr {  $curPosition+5 }] \
			[expr {  [lindex $triData [expr {  $curPosition+5 }]]-$ynew }]]
		    set triData [lreplace $triData [expr {  $curPosition+6 }] [expr {  $curPosition+6 }] \
				     [expr {  [lindex $triData [expr {  $curPosition+6 }]]+$xnew }]]
		    set triData [lreplace $triData [expr {  $curPosition+7 }] [expr {  $curPosition+7 }] \
				     [expr {  [lindex $triData [expr {  $curPosition+7 }]]-$ynew }]]
		    set myAppChangedFlag 1
		    bind .c <Button-1> { }
		    bind .c <B1-Motion> { }
		    bind .c <ButtonRelease-1> { }
		}
		
	    } elseif { [lsearch -exact $polyId $curPoint] != -1 } {
		
##		puts [lsearch -exact $polyId $curPoint]
##		puts [lindex $polyTags [lsearch -exact $polyId $curPoint]]

		.c move [lindex $polyTags [lsearch -exact $polyId $curPoint]] \
		    [expr {  %x-$curX }] [expr {  %y-$curY }]

		.c raise [lindex $polyTags [lsearch -exact $polyId $curPoint]]
		
		set curX %x
		set curY %y
		
		bind .c <ButtonRelease-1> {
		    
		    setTag $curPoint $polyId $polyTags $polyData
		    set apex [lindex $polyData [expr {  $curPosition+2 }]]
		    set xnew [expr {  (%x-$oriX)/$Xscale }]
		    set ynew [expr {  (%y-$oriY)/$Yscale }]
		    
		    for {set polycounter 1} {$polycounter<=$apex} {incr polycounter} {
			set tempx [expr {  $curPosition+2*$polycounter+1 }]
			set tempy [expr {  $curPosition+2*$polycounter+2 }]
			set polyData [lreplace $polyData $tempx $tempx \
			[expr {  [lindex $polyData $tempx]+$xnew }]]
			set polyData [lreplace $polyData $tempy $tempy \
					  [expr {  [lindex $polyData $tempy]-$ynew }]]
			set myAppChangedFlag 1
			bind .c <Button-1> { }
			bind .c <B1-Motion> { }
			bind .c <ButtonRelease-1> { }
		    }
		}
		
	    } elseif { [lsearch -exact $gndId $curPoint] != -1 } {
		
##		puts [lsearch -exact $gndId $curPoint]
##		puts [lindex $gndTags [lsearch -exact $gndId $curPoint]]
		
		.c move [lindex $gndTags [lsearch -exact $gndId $curPoint]] \
		    [expr {  %x-$curX }] [expr {  %y-$curY }]
		
		.c raise [lindex $gndTags [lsearch -exact $gndId $curPoint]]
		
		set curX %x
		set curY %y
		
		bind .c <ButtonRelease-1> {
		    setTag $curPoint $gndId $gndTags $gndData
		    set xnew [expr {  (%x-$oriX)/$Xscale }]
		    set ynew [expr {  (%y-$oriY)/$Yscale }]
		    set gndData [lreplace $gndData [expr {  $curPosition+2 }] [expr {  $curPosition+2 }] \
				     [expr {  [lindex $gndData [expr {  $curPosition+2 }]]+$xnew }]]
		    set gndData [lreplace $gndData [expr {  $curPosition+3 }] [expr {  $curPosition+3 }] \
			[expr {  [lindex $gndData [expr {  $curPosition+3 }]]-$ynew }]]
		    set myAppChangedFlag 1
		    bind .c <Button-1> { }
		    bind .c <B1-Motion> { }
		    bind .c <ButtonRelease-1> { }
		}
	    } else {
		bind .c <Button-1> { }
		bind .c <B1-Motion> { }
		bind .c <ButtonRelease-1> { }
	    }	
	} else {
	    bind .c <B1-Motion> { }
	    bind .c <ButtonRelease-1> { }
	}
    }
    
}


#########################################################################################
#             Delete Unit
#       correct fileHead xxxxData xxxxCounter
#########################################################################################
proc delUnit { } {

    global myAppChangedFlag

    global newFlag

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    bind .c <Button-1> {

	set curPoint [.c find withtag current]
##	puts $curPoint
	if { [lsearch -exact $recId $curPoint] != -1 } {

##	    puts [lsearch -exact $recId $curPoint]
##	    puts [lindex $recTags [lsearch -exact $recId $curPoint]]
	    .c delete [lindex $recTags [lsearch -exact $recId $curPoint]]

	    set recData [lreplace $recData [expr {  [lsearch -exact $recId $curPoint]*9 }] \
		[expr {  [lsearch -exact $recId $curPoint]*9+8 }]]

	    set fileHead [lreplace $fileHead 1 1 [expr {  [lindex $fileHead 1]-1 }]]
##	    puts $fileHead

	    if {[lsearch -exact $recId $curPoint] < [lindex $fileHead 1]} { 

	    for {set counter [lsearch -exact $recId $curPoint]} \
		{$counter<[lindex $fileHead 1]} {incr counter} {
		    set olddata [expr {  $counter*9+3 }]
##		    puts $olddata 
		    set recData [lreplace $recData $olddata $olddata \
				     [expr {  [lindex $recData $olddata]-1 }]]
		    set tempdata [lindex $recData $olddata]
		    set recData [lreplace $recData [expr {  $olddata-1 }] [expr {  $olddata-1 }] \
				     recTags$tempdata]
##		    puts $recData
		    
##		    puts recTags[expr {  $counter+2 }]

		    .c addtag recTags[expr {  $counter+1 }] withtag recTags[expr {  $counter+2 }] 
		    .c dtag recTags[expr {  $counter+2 }]
		    
#Must correct tags for create new shapes
		}
	    }

	    set recCounter [expr {  $recCounter-1 }]
##	    puts $recCounter
	    
#	    set recTags [lreplace $recTags [lsearch -exact $recId $curPoint] \
		[lsearch -exact $recId $curPoint]]
	    set recTags [lreplace $recTags $recCounter $recCounter]
	#delete the last element.

	
	    set recId [lreplace $recId [lsearch -exact $recId $curPoint] \
		       [lsearch -exact $recId $curPoint]]
	
	#            set recId [lreplace $recId $recCounter $recCounter]
##	    puts $recTags
##	    puts $recId

	    set myAppChangedFlag 1

#recTags needs to be corrected also
	    
	} elseif { [lsearch -exact $cirId $curPoint] != -1 } {

##	    puts [lsearch -exact $cirId $curPoint]
##	    puts [lindex $cirTags [lsearch -exact $cirId $curPoint]]
	    .c delete [lindex $cirTags [lsearch -exact $cirId $curPoint]]
#
	    set cirData [lreplace $cirData [expr {  [lsearch -exact $cirId $curPoint]*8 }] \
		[expr {  [lsearch -exact $cirId $curPoint]*8+7 }]]

	    set fileHead [lreplace $fileHead 2 2 [expr {  [lindex $fileHead 2]-1 }]]
##	    puts $fileHead

	    if {[lsearch -exact $cirId $curPoint] < [lindex $fileHead 2]} { 

	    for {set counter [lsearch -exact $cirId $curPoint]} \
		{$counter<[lindex $fileHead 2]} {incr counter} {

		    set olddata [expr {  $counter*8+3 }]
##		    puts $olddata 
		    set cirData [lreplace $cirData $olddata $olddata \
				     [expr {  [lindex $cirData $olddata]-1 }]]
		    set tempdata [lindex $cirData $olddata]
		    set cirData [lreplace $cirData [expr {  $olddata-1 }] [expr {  $olddata-1 }] \
				     cirTags$tempdata]
##		    puts $cirData

##		    puts cirTags[expr {  $counter+2 }]

		    .c addtag cirTags[expr {  $counter+1 }] withtag cirTags[expr {  $counter+2 }] 
		    .c dtag cirTags[expr {  $counter+2 }]
		    
#Must correct tags for create new shapes
		}
	    }

	    #.c addtag newTag recTags2

	    set cirCounter [expr {  $cirCounter-1 }]
##	    puts $cirCounter
	    
	    set cirTags [lreplace $cirTags $cirCounter $cirCounter]
#delete the last element.

	    
	    set cirId [lreplace $cirId [lsearch -exact $cirId $curPoint] \
 		[lsearch -exact $cirId $curPoint]]

#            set cirId [lreplace $cirId $cirCounter $cirCounter]
##	    puts $cirTags
##	    puts $cirId

            set myAppChangedFlag 1

	}  elseif { [lsearch -exact $dieleId $curPoint] != -1 } {

##	    puts [lsearch -exact $dieleId $curPoint]
##	    puts [lindex $dieleTags [lsearch -exact $dieleId $curPoint]]
	    .c delete [lindex $dieleTags [lsearch -exact $dieleId $curPoint]]

            set dieleData [lreplace $dieleData [expr {  [lsearch -exact $dieleId $curPoint]*10 }] \
		[expr {  [lsearch -exact $dieleId $curPoint]*10+9 }]]

	    set fileHead [lreplace $fileHead 0 0 [expr {  [lindex $fileHead 0]-1 }]]
##	    puts $fileHead

	    if {[lsearch -exact $dieleId $curPoint] < [lindex $fileHead 0]} { 

	    for {set counter [lsearch -exact $dieleId $curPoint]} \
		{$counter<[lindex $fileHead 0]} {incr counter} {

		    set olddata [expr {  $counter*10+3 }]
##		    puts $olddata 
		    set dieleData [lreplace $dieleData $olddata $olddata \
				     [expr {  [lindex $dieleData $olddata]-1 }]]
		    set tempdata [lindex $dieleData $olddata]
		    set dieleData [lreplace $dieleData [expr {  $olddata-1 }] [expr {  $olddata-1 }] \
				     dieleTags$tempdata]
##		    puts $dieleData

##		    puts dieleTags[expr {  $counter+2 }]

		    .c addtag dieleTags[expr {  $counter+1 }] withtag dieleTags[expr {  $counter+2 }] 
		    .c dtag dieleTags[expr {  $counter+2 }]
		    
#Must correct tags for create new shapes
		}
	    }

	    #.c addtag newTag recTags2

	    set dieleCounter [expr {  $dieleCounter-1 }]
##	    puts $dieleCounter
	    
	    set dieleTags [lreplace $dieleTags $dieleCounter $dieleCounter]
#delete the last element.

	    
	    set dieleId [lreplace $dieleId [lsearch -exact $dieleId $curPoint] \
 		[lsearch -exact $dieleId $curPoint]]

##	    puts $dieleTags
##	    puts $dieleId

            set myAppChangedFlag 1

	}  elseif { [lsearch -exact $ellId $curPoint] != -1 } {

##	    puts [lsearch -exact $ellId $curPoint]
##	    puts [lindex $ellTags [lsearch -exact $ellId $curPoint]]
	    .c delete [lindex $ellTags [lsearch -exact $ellId $curPoint]]

            set ellData [lreplace $ellData [expr {  [lsearch -exact $ellId $curPoint]*9 }] \
		[expr {  [lsearch -exact $ellId $curPoint]*9+8 }]]

	    set fileHead [lreplace $fileHead 4 4 [expr {  [lindex $fileHead 4]-1 }]]
##	    puts $fileHead

	    if {[lsearch -exact $ellId $curPoint] < [lindex $fileHead 4]} { 

	    for {set counter [lsearch -exact $ellId $curPoint]} \
		{$counter<[lindex $fileHead 4]} {incr counter} {

		    set olddata [expr {  $counter*9+3 }]
##		    puts $olddata 
		    set ellData [lreplace $ellData $olddata $olddata \
				     [expr {  [lindex $ellData $olddata]-1 }]]
		    set tempdata [lindex $ellData $olddata]
		    set ellData [lreplace $ellData [expr {  $olddata-1 }] [expr {  $olddata-1 }] \
				     ellTags$tempdata]
##		    puts $ellData

##		    puts ellTags[expr {  $counter+2 }]

		    .c addtag ellTags[expr {  $counter+1 }] withtag ellTags[expr {  $counter+2 }] 
		    .c dtag ellTags[expr {  $counter+2 }]
		    
#Must correct tags for create new shapes
		}
	    }

	    #.c addtag newTag recTags2

	    set ellCounter [expr {  $ellCounter-1 }]
##	    puts $ellCounter
	    
	    set ellTags [lreplace $ellTags $ellCounter $ellCounter]
#delete the last element.

	    
	    set ellId [lreplace $ellId [lsearch -exact $ellId $curPoint] \
		[lsearch -exact $ellId $curPoint]]

##	    puts $ellTags
##	    puts $ellId
            set myAppChangedFlag 1

	}  elseif { [lsearch -exact $triId $curPoint] != -1 } {

##	    puts [lsearch -exact $triId $curPoint]
##	    puts [lindex $triTags [lsearch -exact $triId $curPoint]]
	    .c delete [lindex $triTags [lsearch -exact $triId $curPoint]]

            set triData [lreplace $triData [expr {  [lsearch -exact $triId $curPoint]*11 }] \
		[expr {  [lsearch -exact $triId $curPoint]*11+10 }]]

	    set fileHead [lreplace $fileHead 3 3 [expr {  [lindex $fileHead 3]-1 }]]
##	    puts $fileHead

	    if {[lsearch -exact $triId $curPoint] < [lindex $fileHead 3]} { 

	    for {set counter [lsearch -exact $triId $curPoint]} \
		{$counter<[lindex $fileHead 3]} {incr counter} {

		    set olddata [expr {  $counter*11+3 }]
##		    puts $olddata 
		    set triData [lreplace $triData $olddata $olddata \
				     [expr {  [lindex $triData $olddata]-1 }]]
		    set tempdata [lindex $triData $olddata]
		    set triData [lreplace $triData [expr {  $olddata-1 }] [expr {  $olddata-1 }] \
				     triTags$tempdata]
##		    puts $triData

##		    puts triTags[expr {  $counter+2 }]

		    .c addtag triTags[expr {  $counter+1 }] withtag triTags[expr {  $counter+2 }]
		    .c dtag triTags[expr {  $counter+2 }]
		    
#Must correct tags for create new shapes
		}
	    }

	    #.c addtag newTag recTags2

	    set triCounter [expr {  $triCounter-1 }]
##	    puts $triCounter
	    
	    set triTags [lreplace $triTags $triCounter $triCounter]
#delete the last element.

	    
	    set triId [lreplace $triId [lsearch -exact $triId $curPoint] \
		[lsearch -exact $triId $curPoint]]

##	    puts $triTags
##	    puts $triId
            set myAppChangedFlag 1

	}  elseif { [lsearch -exact $polyId $curPoint] != -1 } {

##	    puts [lsearch -exact $polyId $curPoint]
##	    puts [lindex $polyTags [lsearch -exact $polyId $curPoint]]
	    .c delete [lindex $polyTags [lsearch -exact $polyId $curPoint]]

            setTag $curPoint $polyId $polyTags $polyData

            set idSite $curPosition
            set curPosition1 $curPosition
##            puts $idSite
##            puts [expr {  $idSite+3+[lindex $polyData [expr {  $idSite+2 }]]*2 }]

            set polyData [lreplace $polyData [expr {  $idSite-2 }] [expr {  $idSite+3+ \
		[lindex $polyData [expr {  $idSite+2 }]]*2 }]]

##            puts $polyData

	    set fileHead [lreplace $fileHead 5 5 [expr {  [lindex $fileHead 5]-1 }]]
##	    puts $fileHead

	    if {[lsearch -exact $polyId $curPoint] < [lindex $fileHead 5]} { 

	    for {set counter [lsearch -exact $polyId $curPoint]} \
		{$counter<[lindex $fileHead 5]} {incr counter} {

		    set olddata [expr {  $curPosition1+1 }]

		    set polyData [lreplace $polyData $olddata $olddata \
			[expr {  [lindex $polyData $olddata]-1 }]]
		    
		    set tempdata [lindex $polyData $olddata]
		    set polyData [lreplace $polyData [expr {  $olddata-1 }] [expr {  $olddata-1 }] \
				     polyTags$tempdata]

		    .c addtag polyTags[expr {  $counter+1 }] withtag polyTags[expr {  $counter+2 }] 
		    .c dtag polyTags[expr {  $counter+2 }]

		    set curPosition1 [expr {  $curPosition1+6+[lindex $polyData \
			[expr {  $curPosition1+2 }]]*2 }]
		}
	    }

	    set polyCounter [expr {  $polyCounter-1 }]
##	    puts $polyCounter
            
	    
	    set polyTags [lreplace $polyTags $polyCounter $polyCounter]
#########
	    set polyId [lreplace $polyId [lsearch -exact $polyId $curPoint] \
		[lsearch -exact $polyId $curPoint]]

#            set polyId [lreplace $polyId $polyCounter $polyCounter]

##            puts $polyData

##	    puts $polyTags
##	    puts $polyId
            set myAppChangedFlag 1

	}  elseif { [lsearch -exact $gndId $curPoint] != -1 } {

##	    puts [lsearch -exact $gndId $curPoint]
##	    puts [lindex $gndTags [lsearch -exact $gndId $curPoint]]
	    .c delete [lindex $gndTags [lsearch -exact $gndId $curPoint]]
	
	    set gndData [lreplace $gndData [expr {  [lsearch -exact $gndId $curPoint]*9 }] \
		[expr {  [lsearch -exact $gndId $curPoint]*9+8 }]]

	    set fileHead [lreplace $fileHead 6 6 [expr {  [lindex $fileHead 6]-1 }]]
##	    puts $fileHead

	    if {[lsearch -exact $gndId $curPoint] < [lindex $fileHead 6]} { 

	    for {set counter [lsearch -exact $gndId $curPoint]} \
		{$counter<[lindex $fileHead 6]} {incr counter} {

		    set olddata [expr {  $counter*9+3 }]
##		    puts $olddata 
		    set gndData [lreplace $gndData $olddata $olddata \
				     [expr {  [lindex $gndData $olddata]-1 }]]
		    set tempdata [lindex $gndData $olddata]
		    set gndData [lreplace $gndData [expr {  $olddata-1 }] [expr {  $olddata-1 }] \
				     gndTags$tempdata]
##		    puts $gndData

##		    puts gndTags[expr {  $counter+2 }]

		    .c addtag gndTags[expr {  $counter+1 }] withtag gndTags[expr {  $counter+2 }] 
		    .c dtag gndTags[expr {  $counter+2 }]
		    
		}
	    }

	    set gndCounter [expr {  $gndCounter-1 }]
##	    puts $gndCounter
	    
	    set gndTags [lreplace $gndTags $gndCounter $gndCounter]

	    set gndId [lreplace $gndId [lsearch -exact $gndId $curPoint] \
		[lsearch -exact $gndId $curPoint]]

#            set gndId [lreplace $gndId $gndCounter $gndCounter]

##	    puts $gndTags
##	    puts $gndId
            set myAppChangedFlag 1
	}
    }
}

################################################################################################
#                       hand create graphs
################################################################################################

proc dispRect { w title startpoint } {

    global palFlag corFlag newFlag
    global recData
    global rectx recty rectwidth rectheight rectcond
    global startpoint1
    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    if { $corFlag == 0 } {
	set rectx [lindex $recData [expr {  $startpoint+2 }]]
	set recty [lindex $recData [expr {  $startpoint+3 }]]
	set rectwidth [lindex $recData [expr {  $startpoint+4 }]]
	set rectheight [lindex $recData [expr {  $startpoint+5 }]]
	set rectcond [lindex $recData [expr {  $startpoint+6 }]]
    }

    grid [label $w.top.lab1 -text "LowerLeft X Coord"] \
	[entry $w.top.ent1 -textvariable rectx] -sticky e
    grid [label $w.top.lab2 -text "LowerLeft Y Coord"] \
	[entry $w.top.ent2 -textvariable recty] -sticky e
    grid [label $w.top.lab3 -text "Width"] \
	[entry $w.top.ent3 -textvariable rectwidth] -sticky e
    grid [label $w.top.lab4 -text "Height"] \
	[entry $w.top.ent4 -textvariable rectheight] -sticky e
    grid [checkbutton $w.top.but1 -text current -variable rectcond -anchor w] -sticky e
    
    set startpoint1 $startpoint

    button $w.bot.buttonok -text "OK" -command { 
	if { $corFlag == -1 } {
	    rectOk $rectx $recty $rectwidth $rectheight $rectcond {}
	    destroy .rectPara
	} else { 
	    rectOk $rectx $recty $rectwidth $rectheight $rectcond $startpoint1
	    destroy .rectPara
	}
    } 
    button $w.bot.buttoncancel -text "CANCEL" -command { destroy .rectPara }
    pack $w.bot.buttonok $w.bot.buttoncancel -side left -fill both
}

proc rectOk { rectx recty rectw recth rectc startpoint } {
    global  myAppChangedFlag
    global recCounter recId recTags
    global Xscale Yscale
    global maxYcoord
    global pbottomleftX pbottomleftY

    global fileHead recData

    global corFlag

##    puts "corFlag"
##    puts $corFlag

    if { $corFlag ==0 } {
##	puts [lindex $recData [expr {  $startpoint }]]
	.c coords [lindex $recData [expr {  $startpoint }]] \
	    [expr {  ($rectx-$pbottomleftX)*$Xscale }]  \
	    [expr {  $maxYcoord-($recty-$pbottomleftY)*$Yscale }] \
	    [expr {  ($rectx+$rectw-$pbottomleftX)*$Xscale }] \
	    [expr {  $maxYcoord-($recty+$recth-$pbottomleftY)*$Yscale }]

	set recData [lreplace $recData [expr {  $startpoint+2 }] [expr {  $startpoint+2 }] \
		$rectx]
	set recData [lreplace $recData [expr {  $startpoint+3 }] [expr {  $startpoint+3 }] \
		$recty]
	set recData [lreplace $recData [expr {  $startpoint+4 }] [expr {  $startpoint+4 }] \
		$rectw]
	set recData [lreplace $recData [expr {  $startpoint+5 }] [expr {  $startpoint+5 }] \
		$recth]
	set recData [lreplace $recData [expr {  $startpoint+6 }] [expr {  $startpoint+6 }] \
		$rectc]
##	puts $recData
    } else {
	incr recCounter
##	puts [expr {  $rectx*$Xscale }]
##	puts [expr {  ($maxYcoord-$recty)*$Yscale }]
##	puts [expr {  ($rectx+$rectw)*$Xscale }]
##	puts [expr {  $maxYcoord-($recty-$recth)*$Yscale }]

##	puts "ggaga"
##	puts $rectc

	set rect [.c create rectangle [expr {  ($rectx-$pbottomleftX)*$Xscale }]  \
			  [expr {  $maxYcoord-($recty-$pbottomleftY)*$Yscale }] \
			  [expr {  ($rectx+$rectw-$pbottomleftX)*$Xscale }] \
			  [expr {  $maxYcoord-($recty+$recth-$pbottomleftY)*$Yscale }] \
			  -outline black -fill yellow -tags recTags$recCounter]

	lappend recId $rect
	lappend recTags recTags$recCounter
	
##	puts $recCounter 
##	puts $recId 
##	puts $recTags
	
	set fileHead [lreplace $fileHead 1 1 [expr {  [lindex $fileHead 1]+1 }]]
##	puts $fileHead
	
	lappend recData "REC" 
	lappend recData $rect
	lappend recData recTags$recCounter
	lappend recData $recCounter
	
	lappend recData $rectx
	lappend recData $recty
    
	lappend recData $rectw
	lappend recData $recth
	
	lappend recData $rectc
##	puts $recData
	set myAppChangedFlag 1
    }
}

proc dispCir { w title startpoint } {

    global palFlag corFlag newFlag
    global cirData
    global circenterX circenterY cirRadius cirCond
    global startpoint1


    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    grid [label $w.top.lab1 -text "Center X Coord"] \
	[entry $w.top.ent1 -textvariable circenterX] -sticky e
    grid [label $w.top.lab2 -text "Center Y Coord"] \
	[entry $w.top.ent2 -textvariable circenterY] -sticky e
    grid [label $w.top.lab3 -text "radius"] \
	[entry $w.top.ent3 -textvariable cirRadius] -sticky e
    grid [checkbutton $w.top.but1 -text current -variable cirCond -anchor w] -sticky e

    if { $corFlag == 0 } {
	set circenterX [lindex $cirData [expr {  $startpoint+2 }]]
	set circenterY [lindex $cirData [expr {  $startpoint+3 }]]
	set cirRadius [lindex $cirData [expr {  $startpoint+4 }]]
	set cirCond [lindex $cirData [expr {  $startpoint+5 }]]
    }

    set startpoint1 $startpoint

    button $w.bot.buttonok -text "OK" -command { 
	if { $corFlag == -1 } {
	    cirOk $circenterX $circenterY $cirRadius $cirCond {}
	    destroy .cirPara
	} else { 
	    cirOk $circenterX $circenterY $cirRadius $cirCond $startpoint1
	    destroy .cirPara
	}


    }
    button $w.bot.buttoncancel -text "CANCEL" -command { destroy .cirPara }
    pack $w.bot.buttonok $w.bot.buttoncancel -side left -fill both
}

proc cirOk { cirx ciry cirr circ startpoint } {
    global myAppChangedFlag
    global cirCounter cirId cirTags
    global Xscale Yscale
    global maxYcoord
    global pbottomleftX pbottomleftY

    global fileHead cirData
    global corFlag
    
    if { $corFlag ==0 } {
##	puts [lindex $cirData [expr {  $startpoint }]]
	.c coords [lindex $cirData [expr {  $startpoint }]] \
	    [expr {  ($cirx-$cirr-$pbottomleftX)*$Xscale }]  \
	    [expr {  $maxYcoord-($ciry-$cirr-$pbottomleftY)*$Yscale }] \
	    [expr {  ($cirx+$cirr-$pbottomleftX)*$Xscale }] \
	    [expr {  $maxYcoord-($ciry+$cirr-$pbottomleftY)*$Yscale }]

	set cirData [lreplace $cirData [expr {  $startpoint+2 }] [expr {  $startpoint+2 }] \
		$cirx]
	set cirData [lreplace $cirData [expr {  $startpoint+3 }] [expr {  $startpoint+3 }] \
		$ciry]
	set cirData [lreplace $cirData [expr {  $startpoint+4 }] [expr {  $startpoint+4 }] \
		$cirr]
	set cirData [lreplace $cirData [expr {  $startpoint+5 }] [expr {  $startpoint+5 }] \
		$circ]
##	puts $cirData
    } else {
	incr cirCounter

	set cir [.c create oval [expr {  ($cirx-$cirr-$pbottomleftX)*$Xscale }]  [expr {  \
		 $maxYcoord-($ciry-$cirr-$pbottomleftY)*$Yscale }] \
		 [expr {  ($cirx+$cirr-$pbottomleftX)*$Xscale }] \
		 [expr {  $maxYcoord-($ciry+$cirr-$pbottomleftY)*$Yscale }] \
		 -outline black -fill red -tags cirTags$cirCounter]    
	
	lappend cirId $cir
	lappend cirTags cirTags$cirCounter
    
	set fileHead [lreplace $fileHead 2 2 [expr {  [lindex $fileHead 2]+1 }]]
##	puts $fileHead

	lappend cirData "CIR" 
	lappend cirData $cir
	lappend cirData cirTags$cirCounter
	lappend cirData $cirCounter

	lappend cirData $cirx
	lappend cirData $ciry

	lappend cirData $cirr
	
	lappend cirData $circ
##	puts $cirData

##	puts $cirCounter 
##	puts $cirId 
##	puts $cirTags
	set myAppChangedFlag 1
    }
}


proc dispDiele { w title startpoint } {

    global palFlag corFlag newFlag
    global dieleData
    global dieleX dieleY dieleWidth dieleHeight dielePermit dielePermea
    global startpoint1

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    grid [label $w.top.lab1 -text "BottomLeft X Coord"] \
	[entry $w.top.ent1 -textvariable dieleX] -sticky e
    grid [label $w.top.lab2 -text "BottomLeft Y Coord"] \
	[entry $w.top.ent2 -textvariable dieleY] -sticky e
    grid [label $w.top.lab3 -text "Width"] \
	[entry $w.top.ent3 -textvariable dieleWidth] -sticky e
    grid [label $w.top.lab4 -text "Height"] \
	[entry $w.top.ent4 -textvariable dieleHeight] -sticky e
    grid [label $w.top.lab5 -text "Permitivity"] \
	[entry $w.top.ent5 -textvariable dielePermit] -sticky e    
    grid [label $w.top.lab6 -text "Permeability"] \
	[entry $w.top.ent6 -textvariable dielePermea] -sticky e    

    if { $corFlag == 0 } {
	set dieleX [lindex $dieleData [expr {  $startpoint+2 }]]
	set dieleY [lindex $dieleData [expr {  $startpoint+3 }]]
	set dieleWidth [lindex $dieleData [expr {  $startpoint+4 }]]
	set dieleHeight [lindex $dieleData [expr {  $startpoint+5 }]]
	set dielePermit [lindex $dieleData [expr {  $startpoint+6 }]]
	set dielePermea [lindex $dieleData [expr {  $startpoint+7 }]]
##	puts $dieleData
    }

    set startpoint1 $startpoint

    button $w.bot.buttonok -text "OK" -command { 
	if { $corFlag == -1 } {
	    dieleOk $dieleX $dieleY $dieleWidth $dieleHeight $dielePermit $dielePermea {}
	    destroy .dielePara
	} else { 
	    dieleOk $dieleX $dieleY $dieleWidth $dieleHeight $dielePermit \
		$dielePermea $startpoint1
	    destroy .dielePara
	}
    }
    button $w.bot.buttoncancel -text "CANCEL" -command { destroy .dielePara }
    pack $w.bot.buttonok $w.bot.buttoncancel -side left -fill both
}

proc dieleOk { dielex dieley dielew dieleh dielepermit dielepermea startpoint } {
    global dieleCounter dieleId dieleTags
    global Xscale Yscale
    global maxYcoord
    global dieleData fileHead
    global pbottomleftX pbottomleftY

    global corFlag
    global myAppChangedFlag

    if { $corFlag ==0 } {
##	puts [lindex $dieleData [expr {  $startpoint }]]
	.c coords [lindex $dieleData [expr {  $startpoint }]] \
	    [expr {  ($dielex-$pbottomleftX)*$Xscale }]  \
	    [expr {  $maxYcoord-($dieley-$pbottomleftY)*$Yscale }] \
	    [expr {  ($dielex+$dielew-$pbottomleftX)*$Xscale }] \
	    [expr {  $maxYcoord-($dieley+$dieleh-$pbottomleftY)*$Yscale }]

	set dieleData [lreplace $dieleData [expr {  $startpoint+2 }] [expr {  $startpoint+2 }] \
		$dielex]
	set dieleData [lreplace $dieleData [expr {  $startpoint+3 }] [expr {  $startpoint+3 }] \
		$dieley]
	set dieleData [lreplace $dieleData [expr {  $startpoint+4 }] [expr {  $startpoint+4 }] \
		$dielew]
	set dieleData [lreplace $dieleData [expr {  $startpoint+5 }] [expr {  $startpoint+5 }] \
		$dieleh]
	set dieleData [lreplace $dieleData [expr {  $startpoint+6 }] [expr {  $startpoint+6 }] \
		$dielepermit]
	set dieleData [lreplace $dieleData [expr {  $startpoint+7 }] [expr {  $startpoint+7 }] \
		$dielepermea]
##	puts $dieleData
    } else {
	incr dieleCounter
	set diele [.c create rectangle [expr {  ($dielex-$pbottomleftX)*$Xscale }]  \
		       [expr {  $maxYcoord-($dieley-$pbottomleftY)*$Yscale }] \
		       [expr {  ($dielex+$dielew-$pbottomleftX)*$Xscale }] \
		       [expr {  $maxYcoord-($dieley+$dieleh-$pbottomleftY)*$Yscale }] \
		       -outline black -fill black -tags dieleTags$dieleCounter]
	lappend dieleId $diele
	lappend dieleTags dieleTags$dieleCounter
	
	set fileHead [lreplace $fileHead 0 0 [expr {  [lindex $fileHead 0]+1 }]]
##	puts $fileHead
	
	lappend dieleData "DIELE" 
	lappend dieleData $diele
	lappend dieleData dieleTags$dieleCounter
	lappend dieleData $dieleCounter
	
	lappend dieleData $dielex
	lappend dieleData $dieley
	
	lappend dieleData $dielew
	lappend dieleData $dieleh
	
	lappend dieleData $dielepermit
	lappend dieleData $dielepermea

	set myAppChangedFlag 1

##	puts $dieleData
	
##	puts $dieleCounter 
##	puts $dieleId 
##	puts $dieleTags
    }
}

proc dispEll { w title startpoint } {
    global palFlag corFlag newFlag
    global ellData
    global ellcenterX ellcenterY ellMajor ellMinor ellCond
    global startpoint1

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    grid [label $w.top.lab1 -text "Center X Coord"] \
	[entry $w.top.ent1 -textvariable ellcenterX] -sticky e
    grid [label $w.top.lab2 -text "Center Y Coord"] \
	[entry $w.top.ent2 -textvariable ellcenterY] -sticky e
    grid [label $w.top.lab3 -text "X Axis Length"] \
	[entry $w.top.ent3 -textvariable ellMajor] -sticky e
    grid [label $w.top.lab4 -text "Y Axis Length"] \
	[entry $w.top.ent4 -textvariable ellMinor] -sticky e
    grid [label $w.top.lab5 -text "Conductivity"] \
	[entry $w.top.ent5 -textvariable ellCond] -sticky e  

    if { $corFlag == 0 } {
	set ellcenterX [lindex $ellData [expr {  $startpoint+2 }]]
	set ellcenterY [lindex $ellData [expr {  $startpoint+3 }]]
	set ellMajor [lindex $ellData [expr {  $startpoint+4 }]]
	set ellMinor [lindex $ellData [expr {  $startpoint+5 }]]
	set ellCond [lindex $ellData [expr {  $startpoint+6 }]]
    }  

    set startpoint1 $startpoint

    button $w.bot.buttonok -text "OK" -command {
	if { $corFlag == -1 } {
	    ellOk $ellcenterX $ellcenterY $ellMajor $ellMinor $ellCond {}
	    destroy .ellPara
	} else { 
	    ellOk $ellcenterX $ellcenterY $ellMajor $ellMinor $ellCond $startpoint1
	    destroy .ellPara
	}

    }
    button $w.bot.buttoncancel -text "CANCEL" -command { destroy .ellPara }
    pack $w.bot.buttonok $w.bot.buttoncancel -side left -fill both
}

proc ellOk { ellx elly ellma ellmi ellc startpoint } {
    global ellCounter ellId ellTags
    global Xscale Yscale
    global maxYcoord
    global fileHead ellData
    global pbottomleftX pbottomleftY
    global corFlag
    global myAppChangedFlag

    if { $corFlag ==0 } {
##	puts [lindex $ellData [expr {  $startpoint }]]
	.c coords [lindex $ellData [expr {  $startpoint }]] \
	    [expr {  ($ellx-$ellma-$pbottomleftX)*$Xscale }]  \
	    [expr {  $maxYcoord-($elly-$ellmi-$pbottomleftY)*$Yscale }] \
	    [expr {  ($ellx+$ellma-$pbottomleftX)*$Xscale }] \
	    [expr {  $maxYcoord-($elly+$ellmi-$pbottomleftY)*$Yscale }]
	set ellData [lreplace $ellData [expr {  $startpoint+2 }] [expr {  $startpoint+2 }] \
		$ellx]
	set ellData [lreplace $ellData [expr {  $startpoint+3 }] [expr {  $startpoint+3 }] \
		$elly]
	set ellData [lreplace $ellData [expr {  $startpoint+4 }] [expr {  $startpoint+4 }] \
		$ellma]
	set ellData [lreplace $ellData [expr {  $startpoint+5 }] [expr {  $startpoint+5 }] \
		$ellmi]
	set ellData [lreplace $ellData [expr {  $startpoint+6 }] [expr {  $startpoint+6 }] \
		$ellc]
##	puts $ellData

    } else {
	incr ellCounter

	set ell [.c create oval [expr {  ($ellx-$ellma-$pbottomleftX)*$Xscale }]  \
		 [expr {  $maxYcoord-($elly-$ellmi-$pbottomleftY)*$Yscale }] \
		 [expr {  ($ellx+$ellma-$pbottomleftX)*$Xscale }] \
		 [expr {  $maxYcoord-($elly+$ellmi-$pbottomleftY)*$Yscale }] \
		 -outline black -fill white -tags ellTags$ellCounter]    
	
	lappend ellId $ell
	lappend ellTags ellTags$ellCounter

	set fileHead [lreplace $fileHead 4 4 [expr {  [lindex $fileHead 4]+1 }]]
##	puts $fileHead

	lappend ellData "ELL" 
	lappend ellData $ell
	lappend ellData ellTags$ellCounter
	lappend ellData $ellCounter

	lappend ellData $ellx
	lappend ellData $elly

	lappend ellData $ellma
	lappend ellData $ellmi

	lappend ellData $ellc
	set myAppChangedFlag 1

##	puts $ellData

##	puts $ellCounter 
##	puts $ellId 
##	puts $ellTags
    }
}

proc dispTri { w title startpoint } {
    global palFlag corFlag newFlag
    global triData
    global triapex1X triapex1Y triapex2X triapex2Y triapex3X triapex3Y triCond
    global startpoint1

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    grid [label $w.top.lab1 -text "Apex 1 X Coord"] \
	[entry $w.top.ent1 -textvariable triapex1X] -sticky e
    grid [label $w.top.lab2 -text "Apex 1 Y Coord"] \
	[entry $w.top.ent2 -textvariable triapex1Y] -sticky e
    grid [label $w.top.lab3 -text "Apex 2 X Coord"] \
	[entry $w.top.ent3 -textvariable triapex2X] -sticky e
    grid [label $w.top.lab4 -text "Apex 2 Y Coord"] \
	[entry $w.top.ent4 -textvariable triapex2Y] -sticky e
    grid [label $w.top.lab5 -text "Apex 3 X Coord"] \
	[entry $w.top.ent5 -textvariable triapex3X] -sticky e
    grid [label $w.top.lab6 -text "Apex 3 Y Coord"] \
	[entry $w.top.ent6 -textvariable triapex3Y] -sticky e
    grid [label $w.top.lab7 -text "Conductivity"] \
	[entry $w.top.ent7 -textvariable triCond] -sticky e    

    if { $corFlag == 0 } {
	set triapex1X [lindex $triData [expr {  $startpoint+2 }]]
	set triapex1Y [lindex $triData [expr {  $startpoint+3 }]]
	set triapex2X [lindex $triData [expr {  $startpoint+4 }]]
	set triapex2Y [lindex $triData [expr {  $startpoint+5 }]]
	set triapex3X [lindex $triData [expr {  $startpoint+6 }]]
	set triapex3Y [lindex $triData [expr {  $startpoint+7 }]]
	set triCond [lindex $triData [expr {  $startpoint+8 }]]
    }  

    set startpoint1 $startpoint

    button $w.bot.buttonok -text "OK" -command { 
	if { $corFlag == -1 } {
	    triOk $triapex1X $triapex1Y $triapex2X $triapex2Y $triapex3X $triapex3Y $triCond {}
	    destroy .triPara
	} else { 
	    triOk $triapex1X $triapex1Y $triapex2X $triapex2Y $triapex3X $triapex3Y $triCond \
		$startpoint1
	    destroy .triPara
	}

    }
    button $w.bot.buttoncancel -text "CANCEL" -command { destroy .triPara }
    pack $w.bot.buttonok $w.bot.buttoncancel -side left -fill both
}

proc triOk { tri1x tri1y tri2x tri2y tri3x tri3y tric startpoint } {
    global triCounter triId triTags
    global Xscale Yscale
    global maxYcoord
    global fileHead triData
    global pbottomleftX pbottomleftY
    global corFlag
    global myAppChangedFlag

    if { $corFlag ==0 } {
##	puts [lindex $triData [expr {  $startpoint }]]
	.c coords [lindex $triData [expr {  $startpoint }]] \
	    [expr {  ($tri1x-$pbottomleftX)*$Xscale }] \
	    [expr {  $maxYcoord-($tri1y-$pbottomleftY)*$Yscale }] \
	    [expr {  ($tri2x-$pbottomleftX)*$Xscale }] \
	    [expr {  $maxYcoord-($tri2y-$pbottomleftY)*$Yscale }] \
	    [expr {  ($tri3x-$pbottomleftX)*$Xscale }] \
	    [expr {  $maxYcoord-($tri3y-$pbottomleftY)*$Yscale }]
	set triData [lreplace $triData [expr {  $startpoint+2 }] [expr {  $startpoint+2 }] \
		$tri1x]
	set triData [lreplace $triData [expr {  $startpoint+3 }] [expr {  $startpoint+3 }] \
		$tri1y]
	set triData [lreplace $triData [expr {  $startpoint+4 }] [expr {  $startpoint+4 }] \
		$tri2x]
	set triData [lreplace $triData [expr {  $startpoint+5 }] [expr {  $startpoint+5 }] \
		$tri2y]	
	set triData [lreplace $triData [expr {  $startpoint+6 }] [expr {  $startpoint+6 }] \
		$tri3x]
	set triData [lreplace $triData [expr {  $startpoint+7 }] [expr {  $startpoint+7 }] \
		$tri3y]
	set triData [lreplace $triData [expr {  $startpoint+8 }] [expr {  $startpoint+8 }] \
		$tric]
##	puts $triData

    } else {

	incr triCounter

	set tri [.c create polygon [expr {  ($tri1x-$pbottomleftX)*$Xscale }] \
		     [expr {  $maxYcoord-($tri1y-$pbottomleftY)*$Yscale }] \
		     [expr {  ($tri2x-$pbottomleftX)*$Xscale }] \
		     [expr {  $maxYcoord-($tri2y-$pbottomleftY)*$Yscale }] \
		     [expr {  ($tri3x-$pbottomleftX)*$Xscale }] \
		     [expr {  $maxYcoord-($tri3y-$pbottomleftY)*$Yscale }] \
		     -outline black -fill white -tags triTags$triCounter]
	
	lappend triId $tri
	lappend triTags triTags$triCounter
	
	set fileHead [lreplace $fileHead 3 3 [expr {  [lindex $fileHead 3]+1 }]]
##	puts $fileHead
	
	lappend triData "TRI" 
	lappend triData $tri
	lappend triData triTags$triCounter
	lappend triData $triCounter
	
	lappend triData $tri1x
	lappend triData $tri1y
	
	lappend triData $tri2x
	lappend triData $tri2y
	
	lappend triData $tri3x
	lappend triData $tri3y
	
	lappend triData $tric
	set myAppChangedFlag 1
	
##	puts $triData
	
##	puts $triCounter 
##	puts $triId 
##	puts $triTags
    }
}


#####################
# Get the apexnumber of polygons
# 
#####################

proc getApexnumdrop { } {

    global apexnum

    set apexnum 4   
    return $apexnum
}

proc getApexnum { } {
    global apexnum
	    
    set apexnum1 4	
    dispPoly .polyPara { Polygon Conductor Parameters } $apexnum1 {}
    return $apexnum1
}

proc dispPoly { w title apexnum startpoint } {
    global palFlag corFlag newFlag
    global pbottomleftX pbottomleftY
    global startpoint1

    global polyCounter polyId polyTags
    global Xscale Yscale
    global maxYcoord
    global apexpoly

    global polyData

    global polyapexX1 polyapexY1 polyapexX2 polyapexY2 polyapexX3 polyapexY3
    global polyapexX4 polyapexY4 polyapexX5 polyapexY5 polyapexX6 polyapexY6
    global polyapexX7 polyapexY7 polyapexX8 polyapexY8 polyapexX9 polyapexY9
    global polyapexX10 polyapexY10 polyapexX11 polyapexY11 polyapexX12 polyapexY12
    global polyapexX13 polyapexY13 polyapexX14 polyapexY14 polyapexX15 polyapexY15
    global polyapexX16 polyapexY16 polyapexX17 polyapexY17 polyapexX18 polyapexY18
    global polyapexX19 polyapexY19 polyapexX20 polyapexY20
    global polyCond

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    set counter 1
    set polyapexlist {}
    set apexpoly $apexnum

    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    while { $counter <= $apexnum } {
	
	global polyapexX polyapexY

##	puts $counter

	grid [label $w.top.lab[expr {  2*$counter }] -text "Apex$counter X Coord"] \
	    [entry $w.top.ent[expr {  2*$counter }] -textvariable polyapexX($counter)] \
	    -sticky e

	grid [label $w.top.lab[expr {  2*$counter+1 }] -text "Apex$counter Y Coord"] \
	    [entry $w.top.ent[expr {  2*$counter+1 }] -textvariable polyapexY($counter)] -sticky e 

	incr counter
    }

    grid [checkbutton $w.top.but1 -text current -variable polyCond -anchor w] -sticky e

    if { $corFlag == 0 } {
	for {set counter 1} {$counter<=$apexnum} {incr counter} {
	    set polyapexX($counter) [lindex $polyData [expr {  $startpoint+2*$counter+1 }]]
	    set polyapexY($counter) [lindex $polyData [expr {  $startpoint+2*$counter+2 }]]
	}
	set polyCond [lindex $polyData [expr {  $startpoint+2*$counter+1 }]]
    }

    set startpoint1 $startpoint

    button $w.bot.buttonok -text "OK" -command { 

	set polyapexlist [expr {  ($polyapexX(1)-$pbottomleftX)*$Xscale }]
	lappend polyapexlist [expr {  $maxYcoord-($polyapexY(1)-$pbottomleftY)*$Yscale }]

	for {set counter 2} {$counter<=$apexpoly} {incr counter} {
            lappend polyapexlist [expr {  ($polyapexX($counter)-$pbottomleftX)*$Xscale }]
	    lappend polyapexlist [expr {  $maxYcoord-($polyapexY($counter)-$pbottomleftY)*$Yscale }]
	}

##	puts $polyapexlist

	if { $corFlag == -1 } {
	    polyOk $polyapexlist $polyCond {}
	} else { 
	    polyOk $polyapexlist $polyCond $startpoint1 
	}

	destroy .polyPara
    }
    button $w.bot.buttoncancel -text "CANCEL" -command { destroy .polyPara }
    pack $w.bot.buttonok $w.bot.buttoncancel -side left -fill both
}

proc polyOk { polyapexlist polyc startpoint } {

    global polyCounter polyId polyTags
    global Xscale Yscale
    global fileHead polyData
    global paletteYsize
    global pbottomleftX pbottomleftY
    global maxYcoord
    global corFlag
    global myAppChangedFlag
    
    if { $corFlag ==0 } {
	
##	puts [lindex $polyData [expr {  $startpoint }]]
	.c coords [lindex $polyData [expr {  $startpoint }]] $polyapexlist
	
	for {set i 0} {$i < [expr {  [llength $polyapexlist]/2 }]} {incr i} {
	    set polyData [lreplace $polyData [expr {  $startpoint+3+2*$i }] \
		    [expr {  $startpoint+3+2*$i }] \
		    [expr {  [lindex $polyapexlist [expr {  2*$i }]]/$Xscale+$pbottomleftX }]]
	    set polyData [lreplace $polyData [expr {  $startpoint+4+2*$i }] \
		    [expr {  $startpoint+4+2*$i }] \
		    [expr {  ($maxYcoord-[lindex $polyapexlist \
		    [expr {  2*$i+1 }]])/$Yscale+$pbottomleftY }]]
	}

	set polyData [lreplace $polyData [expr {  $startpoint+3+2*$i }] [expr {  $startpoint+4+2*$i }] \
		$polyc]
##	puts $polyData
	
    } else {
	
	incr polyCounter
	
	set poly [.c create polygon $polyapexlist \
		-outline black -fill blue -tags polyTags$polyCounter]
	
	lappend polyId $poly
	lappend polyTags polyTags$polyCounter
	
	set fileHead [lreplace $fileHead 5 5 [expr {  [lindex $fileHead 5]+1 }]]
##	puts $fileHead
	
	lappend polyData "POLY" 
	lappend polyData $poly
	lappend polyData polyTags$polyCounter
	lappend polyData $polyCounter
	
	lappend polyData [expr {  [llength $polyapexlist]/2 }]
	
##	puts [llength $polyapexlist]
	
	for {set i 0} {$i < [expr {  [llength $polyapexlist]/2 }]} {incr i} {
	    lappend polyData [expr {  [lindex $polyapexlist [expr {  2*$i }]]/$Xscale+$pbottomleftX }]
	    lappend polyData [expr {  ($maxYcoord-[lindex $polyapexlist \
		    [expr {  2*$i+1 }]])/$Yscale+$pbottomleftY }]
	}
	
	lappend polyData $polyc
	set myAppChangedFlag 1
	
##	puts $polyData
	
##	puts $polyCounter 
##	puts $polyId 
##	puts $polyTags
    }
}



proc dispGnd { w title startpoint } {
    global palFlag corFlag newFlag
    global gndX gndY gndWidth gndHeight gndCond
    global gndData
    global startpoint1

    checkPal

    if { $palFlag < 0 } {
	return $palFlag
    } 

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    grid [label $w.top.lab1 -text "LowerLeft X Coord"] \
	[entry $w.top.ent1 -textvariable gndX] -sticky e
    grid [label $w.top.lab2 -text "LowerLeft Y Coord"] \
	[entry $w.top.ent2 -textvariable gndY] -sticky e
    grid [label $w.top.lab3 -text "Width"] \
	[entry $w.top.ent3 -textvariable gndWidth] -sticky e
    grid [label $w.top.lab4 -text "Height"] \
	[entry $w.top.ent4 -textvariable gndHeight] -sticky e
    grid [checkbutton $w.top.but1 -text current -variable gndCond -anchor w] -sticky e

    if { $corFlag == 0 } {
	set gndX [lindex $gndData [expr {  $startpoint+2 }]]
	set gndY [lindex $gndData [expr {  $startpoint+3 }]]
	set gndWidth [lindex $gndData [expr {  $startpoint+4 }]]
	set gndHeight [lindex $gndData [expr {  $startpoint+5 }]]
	set gndCond [lindex $gndData [expr {  $startpoint+6 }]]
    }
    
    set startpoint1 $startpoint

    button $w.bot.buttonok -text "OK" -command { 
	if { $corFlag == -1 } {
	    gndOk $gndX $gndY $gndWidth $gndHeight $gndCond {}
	    destroy .gndPara
	} else { 
	    gndOk $gndX $gndY $gndWidth $gndHeight $gndCond $startpoint1
	    destroy .gndPara
	}

    }
    button $w.bot.buttoncancel -text "CANCEL" -command { destroy .gndPara }
    pack $w.bot.buttonok $w.bot.buttoncancel -side left -fill both
}

proc gndOk { gndx gndy gndw gndh gndc startpoint } {
    global gndCounter gndId gndTags
    global Xscale Yscale
    global maxYcoord
    global fileHead gndData
    global pbottomleftX pbottomleftY
    global corFlag
    global myAppChangedFlag

    if { $corFlag ==0 } {
##	puts [lindex $gndData [expr {  $startpoint }]]
	.c coords [lindex $gndData [expr {  $startpoint }]] \
	    [expr {  ($gndx-$pbottomleftX)*$Xscale }] \
	    [expr {  $maxYcoord-($gndy-$pbottomleftY)*$Yscale }] \
	    [expr {  ($gndx+$gndw-$pbottomleftX)*$Xscale }] \
	    [expr {  $maxYcoord-($gndy+$gndh-$pbottomleftY)*$Yscale }]

	set gndData [lreplace $gndData [expr {  $startpoint+2 }] [expr {  $startpoint+2 }] \
		$gndx]
	set gndData [lreplace $gndData [expr {  $startpoint+3 }] [expr {  $startpoint+3 }] \
		$gndy]
	set gndData [lreplace $gndData [expr {  $startpoint+4 }] [expr {  $startpoint+4 }] \
		$gndw]
	set gndData [lreplace $gndData [expr {  $startpoint+5 }] [expr {  $startpoint+5 }] \
		$gndh]	
	set gndData [lreplace $gndData [expr {  $startpoint+6 }] [expr {  $startpoint+6 }] \
		$gndc]
##	puts $gndData

    } else {

	incr gndCounter
	
##	puts $Xscale 
##	puts $Yscale

	set gnd [.c create rectangle [expr {  ($gndx-$pbottomleftX)*$Xscale }] \
		     [expr {  $maxYcoord-($gndy-$pbottomleftY)*$Yscale }] \
		     [expr {  ($gndx+$gndw-$pbottomleftX)*$Xscale }] \
		     [expr {  $maxYcoord-($gndy+$gndh-$pbottomleftY)*$Yscale }] \
		     -outline black -fill green -tags gndTags$gndCounter]

	
	lappend gndId $gnd
	lappend gndTags gndTags$gndCounter
	
	set fileHead [lreplace $fileHead 6 6 [expr {  [lindex $fileHead 6]+1 }]]
##	puts $fileHead
	
	lappend gndData "GND" 
	lappend gndData $gnd
	lappend gndData gndTags$gndCounter
	lappend gndData $gndCounter
	
	lappend gndData $gndx
	lappend gndData $gndy
	
	lappend gndData $gndw
	lappend gndData $gndh
	
	lappend gndData $gndc
	set myAppChangedFlag 1

##	puts $gndData

##	puts $gndCounter 
##	puts $gndId 
##	puts $gndTags
    }
}

########################################################################################
#                  Edit the parameters of the palette
########################################################################################

proc Coption { w title flag } {
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global tempdata1 tempdata2 tempdata3 tempdata4
    global newFlag

    if { $newFlag < 0 && $flag == 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 
    
    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    set tempdata1 $paletteXsize
    set tempdata2 $paletteYsize
    set tempdata3 $pbottomleftX
    set tempdata4 $pbottomleftY

#    grid [label $w.top.lab1 -text "Palette X Size"] \
\#	[entry $w.top.ent1 -textvariable paletteXsize] \
\#	[label $w.top.lab2 -text "m"] -sticky e

#    grid [label $w.top.lab3 -text "Palette Y Size"] \
\#	[entry $w.top.ent2 -textvariable paletteYsize] \
\#	[label $w.top.lab4 -text "m"] -sticky e
    grid [label $w.top.lab3 -text "Palette Size"] \
	[entry $w.top.ent2 -textvariable paletteXsize] \
	[label $w.top.lab4 -text "m"] -sticky e
    grid [label $w.top.lab5 -text "Palette Bottomleft X Coord"] \
	[entry $w.top.ent3 -textvariable pbottomleftX] -sticky e
    grid [label $w.top.lab6 -text "Palette Bottomleft Y Coord"] \
	[entry $w.top.ent4 -textvariable pbottomleftY] -sticky e

#    grid [label $w.top.lab5 -text ""] \
	[entry $w.top.ent5 -textvariable ] -sticky e
#    grid [label $w.top.lab6 -text ""] \
	[entry $w.top.ent6 -textvariable ] -sticky e
#    grid [label $w.top.lab7 -text ""] \
	[entry $w.top.ent7 -textvariable ] -sticky e    


    button $w.bot.buttonok -text "OK" -command {
	set paletteYsize [expr {  $paletteXsize/$maxXcoord*$maxYcoord }]
	destroy .palettePara
 	optionOk
    }

    button $w.bot.buttoncancel -text "CANCEL" -command { 
	destroy .palettePara
	set paletteXsize $tempdata1
	set paletteYsize $tempdata2
	set pbottomleftX $tempdata3
	set pbottomleftY $tempdata4
    }
    pack $w.bot.buttonok $w.bot.buttoncancel -side left -fill both
}


proc optionOk { } {

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global palFlag
    global myAppChangedFlag
      
    if { $paletteXsize > 0 && $paletteYsize > 0 } {
	set Xscale [ expr {  $maxXcoord/$paletteXsize  }]
##	puts "begin"
##	puts $maxXcoord
##	puts $paletteXsize
##	puts $Xscale
##	puts "end"
	set Yscale [ expr {  $maxYcoord/$paletteYsize  }]

	set palFlag 0

##	puts "OK"
    }
    
}

proc calpara { w title } {
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global tempdata1 tempdata2 tempdata3 tempdata4
    global tempdata5 tempdata6 tempdata7 tempdata8
    global tempdata9 tempdata10 tempdata11 tempdata12
    global newFlag
    global Nh Nit J
    global Nwx Nwy Nws
    global Np Eps matr
    global freq sigma
    global myAppChangedFlag

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    set tempdata1 $Nh
    set tempdata2 $Nit
    set tempdata3 $J
    set tempdata4 $Nwx
    set tempdata5 $Nwy
    set tempdata6 $Nws
    set tempdata7 $Np
    set tempdata8 $Eps
    set tempdata9 $matr
    set tempdata10 $freq
    set tempdata11 $sigma

    grid [label $w.top.lab3 -text "Wavelet Point Number"] \
	[entry $w.top.ent2 -textvariable Nh] -sticky e
    grid [label $w.top.lab4 -text "Number of Iteration"] \
	[entry $w.top.ent3 -textvariable Nit] -sticky e
    grid [label $w.top.lab5 -text "Resolution Level"] \
	[entry $w.top.ent4 -textvariable J] -sticky e
    grid [label $w.top.lab6 -text "Gauss Quadrature X"] \
	[entry $w.top.ent5 -textvariable Nwx] -sticky e
    grid [label $w.top.lab7 -text "Gauss Quadrature Y"] \
	[entry $w.top.ent6 -textvariable Nwy] -sticky e
    grid [label $w.top.lab8 -text "Number of Segments/Wavelet"] \
	[entry $w.top.ent7 -textvariable Nws] -sticky e    
    grid [label $w.top.lab9 -text "OPQ Seperation"] \
	[entry $w.top.ent8 -textvariable Np] -sticky e
    grid [label $w.top.lab10 -text "STAB Accuracy"] \
	[entry $w.top.ent9 -textvariable Eps] -sticky e
    grid [label $w.top.lab11 -text "Matrix Solution"] \
	[entry $w.top.ent10 -textvariable matr] -sticky e
    grid [label $w.top.lab12 -text "Frequency(Hz)"] \
	[entry $w.top.ent11 -textvariable freq] -sticky e
    grid [label $w.top.lab13 -text "Conductivity(S/m)"] \
	[entry $w.top.ent12 -textvariable sigma] -sticky e    

    button $w.bot.buttonok -text "OK" -command {
	set myAppChangedFlag 1
	destroy .calPara
    }
    button $w.bot.buttoncancel -text "CANCEL" -command { 
	destroy .calPara
	set Nh $tempdata1
	set Nit $tempdata2
	set J $tempdata3
	set Nwx $tempdata4
	set Nwy $tempdata5
	set Nws $tempdata6
	set Np $tempdata7
	set Eps $tempdata8
	set matr $tempdata9
	set freq $tempdata10
	set sigma $tempdata11
    }
    pack $w.bot.buttonok $w.bot.buttoncancel -side left -fill both
}


########################################################################################
#                  Check if the palette parameters have been setup
########################################################################################

proc checkPal { } {

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY
    global palFlag
      
    if { $paletteXsize > 0 && $paletteYsize > 0 } {
	set palFlag 0
    } else {
	set palFlag -1

	toplevel .prompted -class Dialog
	wm title .prompted "error"
	wm iconname .prompted Dialog
	frame .prompted.top -relief raised -bd 1
	pack .prompted.top -side top -fill both

	message .prompted.msg -width 8c -justify left -relief flat -bd 2 \
	    -text "You must input the palette parameters first."
	button .prompted.okbutton -text "OK" -command { destroy .prompted }
	pack .prompted.msg .prompted.okbutton -side top -padx 2m -pady 2m

    }
    return $palFlag
}

proc promptInfo { message } {

	toplevel .prompted -class Dialog
	wm title .prompted "Info"
	wm iconname .prompted Dialog
	frame .prompted.top -relief raised -bd 1
	pack .prompted.top -side top -fill both

	message .prompted.msg -width 8c -justify left -relief flat -bd 2 \
	    -text $message
	button .prompted.okbutton -text "OK" -command { destroy .prompted }
	pack .prompted.msg .prompted.okbutton -side top -padx 2m -pady 2m

}

########################################################################################
#   Update data package after deleting and adding new components
########################################################################################
proc updateData { } {
    
}

########################################################################################

    #--------------------------------------------------
    #
    #  File Procedures
    #
    #  Note that opening, saving, and closing files
    #  are all intertwined.  This code assumes that
    #  new/open/close/exit may lose some data.
    #
    #--------------------------------------------------

set myAppFileName ""
set myAppChangedFlag 0
set myAppFileTypes {
    {{data files}  {.dat}            }
    {{tcl files}   {.tcl .tk}        }
    {{text files}  {.txt}            }
    {{All Files}        *            }
}

proc myAppFileNew { } {
    global myAppFileName
    global myAppFileTypes
    global myAppChangedFlag
    global newFlag
    global palFlag
    global paletteXsize paletteYsize
    
    myAppFileClose

    set filename [ tk_getSaveFile  -filetypes  $myAppFileTypes \
	    -initialdir "."]
    puts "file: $filename"
    if {$filename != ""} {
	set indx [string first ".dat" $filename]
	if { $indx > -1 } {
	    set myAppFileName [string range $filename 0 [expr {  ($indx - 1) }]]
	} else {
	    set myAppFileName $filename
	}
    }
    
    Coption .palettePara { Palette Parameters } 1

    tkwait window .palettePara

    if { $palFlag == 0 } {

	initc 0
	.c configure -background gray
	.dummy.statusCon configure -text "Your have setup a new file named $myAppFileName"
	
	set newFlag 0
#	set myAppFileName ""
	set myAppChangedFlag 0
    } 
    puts "new file: $myAppFileName"
}

proc myAppFileOpen { {filename ""} } {
    global myAppFileName
    global myAppChangedFlag
    global myAppFileTypes

    global fileHead

    global dieleData recData cirData triData ellData polyData gndData

    global recCounter cirCounter dieleCounter ellCounter triCounter polyCounter \
	gndCounter
    global recTags cirTags dieleTags ellTags triTags polyTags gndTags
    global recId cirId dieleId ellId triId polyId gndId

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY

    global newFlag

    global Nh Nit J
    global Nwx Nwy Nws
    global Np Eps matr
    global freq sigma
    
    myAppFileClose

    if {$filename == ""} {
	set filename [tk_getOpenFile -filetypes $myAppFileTypes]
    }
    
    if {$filename != ""} {
	puts "Opening $filename"

	if { [catch {open $filename r} fp] } {
	    error "Cannot Open File $filename for Reading"
	} else {

	    set indx [string first ".dat" $filename]
	    if { $indx > -1 } {
		set myAppFileName [string range $filename 0 [expr {  ($indx - 1) }]]
	    } else {
		set myAppFileName $filename
	    }
 
	    #--------------------------------------------------
	    # insert code for "open" operation
	    #--------------------------------------------------
	    ### .t insert end [read $fp [file size $filename]]

	    #    need to read the data, redraw them on the palette and 
	    #    renew the Id number in xxxID and xxxData, renew counter.
	    #    So just redraw all of them
	    
	    set newFlag 0

	    .c configure -background gray

	    gets $fp paletteXsize 
	    gets $fp paletteYsize 
	    gets $fp Xscale
	    gets $fp Yscale
	    gets $fp pbottomleftX 
	    gets $fp pbottomleftY
	    
	    gets $fp Nh
	    gets $fp Nit
	    gets $fp J
	    gets $fp Nwx
	    gets $fp Nwy
	    gets $fp Nws
	    gets $fp Np
	    gets $fp Eps
	    gets $fp matr
	    gets $fp freq
	    gets $fp sigma

	    gets $fp tempfileHead
	    
	    gets $fp tempdieleData 
	    gets $fp temprecData 
	    gets $fp tempcirData 
	    gets $fp temptriData 
	    gets $fp tempellData 
	    gets $fp temppolyData 
	    gets $fp tempgndData
	    
	    set curNum [lindex $tempfileHead 0]
	    for {set counter 0} {$counter < $curNum} {incr counter} {
		set corFlag -1
		set dielex [lindex $tempdieleData [expr {  $counter*10+4 }]]
		set dieley [lindex $tempdieleData [expr {  $counter*10+5 }]]
		set dielew [lindex $tempdieleData [expr {  $counter*10+6 }]]
		set dieleh [lindex $tempdieleData [expr {  $counter*10+7 }]]
		set dielepermit [lindex $tempdieleData [expr {  $counter*10+8 }]]
		set dielepermea [lindex $tempdieleData [expr {  $counter*10+9 }]]
		dieleOk $dielex $dieley $dielew $dieleh $dielepermit $dielepermea {}
	    }	
	    set curNum [lindex $tempfileHead 1]
	    for {set counter 0} {$counter < $curNum} {incr counter} {
		set corFlag -1
		set recx [lindex $temprecData [expr {  $counter*9+4 }]]
		set recy [lindex $temprecData [expr {  $counter*9+5 }]]
		set recw [lindex $temprecData [expr {  $counter*9+6 }]]
		set rech [lindex $temprecData [expr {  $counter*9+7 }]]
		set recc [lindex $temprecData [expr {  $counter*9+8 }]]
		rectOk $recx $recy $recw $rech $recc {}
	    }
	    set curNum [lindex $tempfileHead 2]
	    for {set counter 0} {$counter < $curNum} {incr counter} {
		set corFlag -1
		set cirx [lindex $tempcirData [expr {  $counter*8+4 }]]
		set ciry [lindex $tempcirData [expr {  $counter*8+5 }]]
		set cirr [lindex $tempcirData [expr {  $counter*8+6 }]]
		set circ [lindex $tempcirData [expr {  $counter*8+7 }]]
		cirOk $cirx $ciry $cirr $circ {}
	    }
	    set curNum [lindex $tempfileHead 3]
	    for {set counter 0} {$counter < $curNum} {incr counter} {
		set corFlag -1
		set tri1x [lindex $temptriData [expr {  $counter*11+4 }]]
		set tri1y [lindex $temptriData [expr {  $counter*11+5 }]]
		set tri2x [lindex $temptriData [expr {  $counter*11+6 }]]
		set tri2y [lindex $temptriData [expr {  $counter*11+7 }]]
		set tri3x [lindex $temptriData [expr {  $counter*11+8 }]]
		set tri3y [lindex $temptriData [expr {  $counter*11+9 }]]
		set tric [lindex $temptriData [expr {  $counter*11+10 }]]
		triOk $tri1x $tri1y $tri2x $tri2y $tri3x $tri3y $tric {}
	    }
	    set curNum [lindex $tempfileHead 4]
	    for {set counter 0} {$counter < $curNum} {incr counter} {
		set corFlag -1
		set ellx [lindex $tempellData [expr {  $counter*9+4 }]]
		set elly [lindex $tempellData [expr {  $counter*9+5 }]]
		set ellma [lindex $tempellData [expr {  $counter*9+6 }]]
		set ellmi [lindex $tempellData [expr {  $counter*9+7 }]]
		set ellc [lindex $tempellData [expr {  $counter*9+8 }]]
		ellOk $ellx $elly $ellma $ellmi $ellc {}
	    }
	    set curNum [lindex $tempfileHead 5]
##	    puts $temppolyData
	    if {$curNum > 0} {
		for {set counter 0} {$counter < $curNum} {incr counter} {
		    set corFlag -1
		    if { [lsearch $temppolyData POLY] == 0 } {
##			puts "hhh"
			set tempapexnum [lindex $temppolyData 4]
##			puts $tempapexnum
			set tempapex {}
			for { set i 1 } { $i <= $tempapexnum } {incr i} {
			    lappend tempapex [expr {  ([lindex $temppolyData [expr {  3+2*$i }]] \
				-$pbottomleftX)*$Xscale }]
			    lappend tempapex [expr {  $maxYcoord-([lindex $temppolyData \
				[expr {  4+2*$i }]]-$pbottomleftY)*$Yscale }]
			}
			set polyc [lindex $temppolyData [expr {  5+2*$tempapexnum }]]
		    } 
		    set temppolyData [lreplace $temppolyData 0 [expr {  5+2*$tempapexnum }]]
##		    puts $tempapex
##		    puts $temppolyData
		    polyOk $tempapex $polyc {}
		}
	    }

	    set curNum [lindex $tempfileHead 6]
	    for {set counter 0} {$counter < $curNum} {incr counter} {
		set corFlag -1
		set gndx [lindex $tempgndData [expr {  $counter*9+4 }]]
		set gndy [lindex $tempgndData [expr {  $counter*9+5 }]]
		set gndw [lindex $tempgndData [expr {  $counter*9+6 }]]
		set gndh [lindex $tempgndData [expr {  $counter*9+7 }]]
		set gndc [lindex $tempgndData [expr {  $counter*9+8 }]]
		gndOk $gndx $gndy $gndw $gndh $gndc {}
	    }
	}
     close $fp

#     set myAppFileName $filename
     set myAppChangedFlag 0
    }
}

proc myAppFileClose { } {

    global myAppFileName
    global myAppChangedFlag

    

    if { $myAppChangedFlag } {

	myAppPromptForSave

    }

    

    delall

    initc 1

    .c configure -background black

 #--------------------------------------------------
 # insert code for "close" operation
 #--------------------------------------------------
 ### .t delete 1.0 end

    set myAppFileName ""
    set myAppChangedFlag 0

}

proc myAppFileSave { {filename ""} } {
    global myAppFileName
    global myAppFileTypes
    global myAppChangedFlag

    global fileHead

    global dieleData recData cirData triData ellData polyData gndData

    global recCounter cirCounter dieleCounter ellCounter triCounter polyCounter \
	gndCounter
    global recTags cirTags dieleTags ellTags triTags polyTags gndTags
    global recId cirId dieleId ellId triId polyId gndId

    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY

    global Nh Nit J
    global Nwx Nwy Nws
    global Np Eps matr
    global freq sigma

    puts "file: $filename  myAppFileName: $myAppFileName"


    if { $filename == "" && $myAppFileName != "" } {
	set filename $myAppFileName
    } elseif { $filename == "" && $myAppFileName == "" } {
	set filename [tk_getSaveFile -filetypes $myAppFileTypes]
	if { $filename != "" } {
	    set myAppFileName $filename	
	}
    }
    
    if { $filename != "" } {
	set fullfilename [format "%s.dat" $filename]
	if { [catch {open $fullfilename w+ } fp] } {
	    error "Cannot write to $fullfilename"
	}
	puts "Writing $fullfilename"

	puts $fp $paletteXsize 
	puts $fp $paletteYsize 
	puts $fp $Xscale
	puts $fp $Yscale
	puts $fp $pbottomleftX 
	puts $fp $pbottomleftY

	puts $fp $Nh
	puts $fp $Nit
	puts $fp $J
	puts $fp $Nwx
	puts $fp $Nwy
	puts $fp $Nws
	puts $fp $Np
	puts $fp $Eps
	puts $fp $matr
	puts $fp $freq
	puts $fp $sigma

	puts $fp $fileHead
	puts $fp $dieleData 
	puts $fp $recData 
	puts $fp $cirData 
	puts $fp $triData 
	puts $fp $ellData 
	puts $fp $polyData 
	puts $fp $gndData

     #--------------------------------------------------
     # insert code for "save" operation
     #--------------------------------------------------
     ### puts -nonewline [.t get 1.0 end]

     close $fp
     set myAppFileName $filename
     set myAppChangedFlag 0
    }
}

proc myAppFileSaveAs { } {
    global myAppFileTypes

    set filename [tk_getSaveFile -filetypes $myAppFileTypes]
    if { $filename != "" } {
	set myAppFileName $filename
	myAppFileSave $filename
    }
}

proc myAppPromptForSave { } {
    
    set answer [tk_messageBox -title "save?" -type yesno -icon question -message "Do you want to save the changes?"]
    
    if { $answer == "yes" } {
	myAppFileSaveAs
    }
    
}

proc myAppExit { } {

    myAppFileClose
    exit
}

#########################################################################
#     NAME: about
#     PURPOSE: show the about information including name and version
#
#########################################################################

proc about {} {

#     tk_messageBox -message "Muti-conductor Parameters Computation \n Electric Packaging Lab.\n Arizona State University\n 2000.8"
    promptInfo "Calculation of Resistance and\n Inductance of Transmission\n            Lines! \n\n    Copyright 2000.12 EPL\n   Electric Packaging Lab.\nElectrical Engineering Dept.  \n  Arizona State University\n\n           2000.12"
}

proc eplIntro {} {

    promptInfo "This program is developed by\n\n   Electric Packaging Lab\n Eletrical Engineering Dept. \n  Arizona State University \n\n    Tel: (480)965-7048\n http://quantum.eas.asu.edu/"
}

proc printPara {} {
    
    global newFlag

    global pData

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 
    
    setdata

##    puts $pData
    exec lpr <<$pData &

}

proc printcal {} {
    
    global newFlag

    global pcal

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 
    
    caldata

##    puts $pcal
    exec lpr <<$pcal &

}

proc printGraph {} {

    global myAppFileName
    global maxXcoord maxYcoord

    set pssurfix ".ps"
    if { $myAppFileName  != "" } {
	set pssurfix $myAppFileName$pssurfix
    } else {
	promptInfo "You must save the palette first!"
	return -1
    }

    .c create rectangle 0 0 [expr {  $maxXcoord-1 }] [expr {  $maxYcoord-1 }] \
		      -outline black -tags printframe

    set psFlag [.c postscript -width 24c -height 16c -colormode gray -file $pssurfix -rotate 1]

    .c delete printframe

    if { $psFlag == "" } {
	exec lpr $pssurfix & 
    } else {
	promptInfo "The Postscript file can not be generated!"
	return -1	
    }

}

proc setTag { id idArray tagArray dataArray } {
    global curPosition
    set tempPosition [lsearch -exact $idArray $id]
    set tempTag [lindex $tagArray $tempPosition]
    set curPosition [lsearch -exact $dataArray $tempTag]
    return $curPosition
}

##############################################
#              Test 
##############################################


proc processfreqs { flg } {
    global _freqFrame
    global freq1
    global freq1
    global freq2
    global freq3
    global freq4
    global freq5
    global freq6
    global freq7
    global freq8
    global freq9
    global freq10
    global freq11
    global freq12
    global freq13
    global freq14
    global freq15
    global freq16
    global freq17
    global freq18
    global freq19
    global freq20
    global freq

##    puts "Destroy the dialog"
    destroy $_freqFrame

    set freqList ""

    for { set ii 1 } { $ii < 21 } { incr ii } {
	set cmnd [format {set frq $freq%s} $ii]
	eval $cmnd
	puts "Frequency $frq"
	if { $frq <= 0.0 } {
	    break
	}
	lappend freqList $frq
	set freq $frq
	puts ""
	puts "________________ Freq: $freq __________________"
	startcal1 3
    }

##    puts "$freqList"
    checkResult
}

proc buildFreqList {} {
    global _freqFrame
    global freq1
    global freq1
    global freq2
    global freq3
    global freq4
    global freq5
    global freq6
    global freq7
    global freq8
    global freq9
    global freq10
    global freq11
    global freq12
    global freq13
    global freq14
    global freq15
    global freq16
    global freq17
    global freq18
    global freq19
    global freq20


    set _freqFrame [toplevel .freqs \
	    -relief sunken -borderwidth 2]

    set freq1 1e9
    set freq2 2e9
    set freq3 3e9
    set freq4 4e9
    set freq5 5e9
    set freq6 6e9
    set freq7 7e9
    set freq8 8e9
    set freq9 9e9
    set freq10 10e9
    set freq11 20e9
    set freq12 26e9
    set freq13 0
    set freq14 0
    set freq15 0
    set freq16 0
    set freq17 0
    set freq18 0
    set freq19 0
    set freq20 0


    for {set ii 1 } { $ii < 21 } { incr ii } {
	set var "freq$ii"
	set et [ entry $_freqFrame.e$ii -width 20 -textvariable $var ]
	pack $et -side top -expand true -fill x
    }

    set but1 [button $_freqFrame.but1 \
	    -text OK  -borderwidth 1 \
	    -command "processfreqs 1" \
	    -relief ridge]

    set but2 [button $_freqFrame.but2 \
	    -text Cancel  -borderwidth 1 \
	    -command "processfreqs 0" \
	    -relief ridge]
    pack $but1 $but2 -side bottom -expand true -fill x
}



proc startcal1 { flag } {
    global env
    global myAppFileName
    global myAppDirName
    
    global fileHead

    global dieleData recData cirData triData ellData polyData gndData

    global newFlag

    global pData pcal

    global Nh Nit J
    global Nwx Nwy Nws
    global Np Eps matr
    global freq sigma

##    puts "newFlag $newFlag"
##    puts "hhh"

    if { $newFlag < 0 } {
	promptInfo "You must setup a new palette first!"
	return $newFlag
    } 

#    set fileId [ open "/users/zhichao/tcltk/cal/data.in" w+ ]
    set fullfilename [format "%s.in" $myAppFileName]
    set fileId [ open $fullfilename w+ ]

    set gndnumber [lindex $fileHead 6]
    set recnumber [lindex $fileHead 1]
    set cirnumber [lindex $fileHead 2]
    set tranumber [lindex $fileHead 5]

    puts $fileId $gndnumber
    puts $fileId $recnumber
    puts $fileId $cirnumber
    puts $fileId $tranumber

    if { $gndnumber > 0 } {
	for { set i 1 } { $i <= $gndnumber } {incr i} {
	    puts $fileId [lindex $gndData [expr {  ($i-1)*9+4 }]]
	    puts $fileId [lindex $gndData [expr {  ($i-1)*9+5 }]]
	    puts $fileId [lindex $gndData [expr {  ($i-1)*9+6 }]]
	    puts $fileId [lindex $gndData [expr {  ($i-1)*9+7 }]]
	    puts $fileId [lindex $gndData [expr {  ($i-1)*9+8 }]]
	    
	}
    }

    if { $recnumber > 0 } {
	for { set i 1 } { $i <= $recnumber } {incr i} {
	    puts $fileId [lindex $recData [expr {  ($i-1)*9+4 }]]
	    puts $fileId [lindex $recData [expr {  ($i-1)*9+5 }]]
	    puts $fileId [lindex $recData [expr {  ($i-1)*9+6 }]]
	    puts $fileId [lindex $recData [expr {  ($i-1)*9+7 }]]
	    puts $fileId [lindex $recData [expr {  ($i-1)*9+8 }]]
	    
	}
    }

    if { $cirnumber > 0 } {
	for { set i 1 } { $i <= $cirnumber } {incr i} {
	    puts $fileId [lindex $cirData [expr {  ($i-1)*8+4 }]]
	    puts $fileId [lindex $cirData [expr {  ($i-1)*8+5 }]]
	    puts $fileId [lindex $cirData [expr {  ($i-1)*8+6 }]]
	    puts $fileId [lindex $cirData [expr {  ($i-1)*8+7 }]]
	    
	}
    }

    if { $tranumber > 0 } {
	for { set i 1 } { $i <= $tranumber } {incr i} {
	    puts $fileId [lindex $polyData [expr {  ($i-1)*14+5 }]]
	    puts $fileId [lindex $polyData [expr {  ($i-1)*14+6 }]]
	    puts $fileId [lindex $polyData [expr {  ($i-1)*14+7 }]]
	    puts $fileId [lindex $polyData [expr {  ($i-1)*14+8 }]]
	    puts $fileId [lindex $polyData [expr {  ($i-1)*14+9 }]]
	    puts $fileId [lindex $polyData [expr {  ($i-1)*14+10 }]]
	    puts $fileId [lindex $polyData [expr {  ($i-1)*14+11 }]]
	    puts $fileId [lindex $polyData [expr {  ($i-1)*14+12 }]]
	    puts $fileId [lindex $polyData [expr {  ($i-1)*14+13 }]]
	}
    }

    puts $fileId $Nh
    puts $fileId $Nit
    puts $fileId $J
    puts $fileId $Nwx
    puts $fileId $Nwy
    puts $fileId $Nws
    puts $fileId $Np
    puts $fileId $Eps
    puts $fileId $matr
    puts $fileId $freq
    puts $fileId $sigma

    close $fileId

##    puts $flag 
    if {$flag == 1} {
	setdata
	caldata
	#set fp [ open "/users/zhichao/tcltk/cal/caldata.sys" w+ ]
	set fp [ open "caldata.sys" w+ ]
	puts $fp $pData
	puts $fp $pcal
	close $fp
	#exec emacs "/users/zhichao/tcltk/cal/caldata.sys" &
	exec emacs "$myAppDirName/caldata.sys" &
    } elseif {$flag == 2} {
	set cursr [.c cget -cursor]
	.mbar configure -cursor watch	
	.c configure -cursor watch	
	update
	set cmnd {$env(CALCRL_LIBRARY)/calcRL $myAppFileName}
	set hd [catch { eval exec $cmnd} result]
	.mbar configure -cursor $cursr
	.c configure -cursor $cursr	
	update
	puts $result
	checkResult
    } elseif {$flag == 3} {
	set cursr [.c cget -cursor]
	.mbar configure -cursor watch	
	.c configure -cursor watch	
	update
	set cmnd {$env(CALCRL_LIBRARY)/calcRL \
		$myAppFileName append}
	set hd [catch { eval exec $cmnd} result]
	.mbar configure -cursor $cursr
	.c configure -cursor $cursr	
	update
	puts $result
    }
}

proc checkResult { } {
    global myAppFileName

    #exec xemacs /users/zhichao/tcltk/cal/data.out &
    set fullfilename [format "%s.out" $myAppFileName]
    exec emacs $fullfilename &
}

proc killpro { hd } {

    global pid

    toplevel .apexwindow -class Dialog
    wm title .apexwindow "Calculation"
    wm iconname .apexwindow Dialog

    frame .apexwindow.textframe -width 10c -height 6c
    frame .apexwindow.buttonframe -width 10c -height 2c
    label .apexwindow.textframe.label -text "In Calculation, Please wait..."

    set pid $hd

    button .apexwindow.buttonframe.bbutton -text Break -command { 

	exec kill -9 $pid
	destroy .apexwindow

    }

    pack .apexwindow.textframe .apexwindow.buttonframe -side top -padx 1m -pady 2m
    pack .apexwindow.textframe.label -side left -padx 1m -pady 2m
    pack .apexwindow.buttonframe.bbutton \
	-side left -padx 1m -pady 2m
}

proc setdata { } {

    global fileHead
    global dieleData recData cirData triData ellData polyData gndData
    global newFlag

    global pData
       
    set gndnumber [lindex $fileHead 6]
    set recnumber [lindex $fileHead 1]
    set cirnumber [lindex $fileHead 2]
    set tranumber [lindex $fileHead 5]

    set nl "\n"
    set sp "   "
    set sp1 " "
    set sp5 "     "
    set sep "     -------------------------------------"
    set hh "           The Geometrical Parameters"

    set gn "        Ground    Number:"
    set rn "        Rectangle Number:"
    set cn "        Circle    Number:"
    set tn "        Trapezoid Number:"

    set h1 "        GROUND PLANE"
    set h2 "        RECTANGLE"
    set h3 "        CIRCLE"
    set h4 "        TRAPEZOID"
    
    set r1 "         lowerleft x:"
    set r2 "         lowerleft y:"
    set r3 "               width:"
    set r4 "              height:"
    
    set c1 "        center x:"
    set c2 "        center y:"
    set c3 "          radius:"

    set t1 "        apex x"
    set t2 "        apex y"
    set t3 ":"

    set c  "        current:"

    set pData $nl$sep$nl$hh$nl$sep$nl$nl

    set pData $pData$gn$sp$gndnumber$nl
    set pData $pData$rn$sp$recnumber$nl
    set pData $pData$cn$sp$cirnumber$nl
    set pData $pData$tn$sp$tranumber$nl$nl$sep$nl
    
    if { $gndnumber > 0 } {
	for { set i 1 } { $i <= $gndnumber } { incr i } {
	    set pData $pData$h1$i$nl$nl
	    set pData $pData$r1$sp[lindex $gndData [expr {  ($i-1)*9+4 }]]$nl
	    set pData $pData$r2$sp[lindex $gndData [expr {  ($i-1)*9+5 }]]$nl
	    set pData $pData$r3$sp[lindex $gndData [expr {  ($i-1)*9+6 }]]$nl
	    set pData $pData$r4$sp[lindex $gndData [expr {  ($i-1)*9+7 }]]$nl
	    #set pData $pData$sp5$c$sp[lindex $gndData [expr {  ($i-1)*9+8 }]]$nl	
	    set pData $pData$nl
	}
	set pData $pData$sep$nl
    }

    if { $recnumber > 0 } {
	for { set i 1 } { $i <= $recnumber } { incr i } {
	    set pData $pData$h2$i$nl$nl
	    set pData $pData$r1$sp[lindex $recData [expr {  ($i-1)*9+4 }]]$nl
	    set pData $pData$r2$sp[lindex $recData [expr {  ($i-1)*9+5 }]]$nl
	    set pData $pData$r3$sp[lindex $recData [expr {  ($i-1)*9+6 }]]$nl
	    set pData $pData$r4$sp[lindex $recData [expr {  ($i-1)*9+7 }]]$nl
	    #set pData $pData$sp5$c$sp[lindex $recData [expr {  ($i-1)*9+8 }]]$nl	  
	    set pData $pData$nl
	}
	set pData $pData$sep$nl
    }

    if { $cirnumber > 0 } {
	for { set i 1 } { $i <= $cirnumber } { incr i } {
	    set pData $pData$h3$i$nl$nl
	    set pData $pData$c1$sp[lindex $cirData [expr {  ($i-1)*8+4 }]]$nl
	    set pData $pData$c2$sp[lindex $cirData [expr {  ($i-1)*8+5 }]]$nl
	    set pData $pData$c3$sp[lindex $cirData [expr {  ($i-1)*8+6 }]]$nl
	    #set pData $pData$sp1$c$sp[lindex $cirData [expr {  ($i-1)*8+7 }]]$nl	  
	    set pData $pData$nl
	}
	set pData $pData$sep$nl
    }

    if { $tranumber > 0 } {
	for { set i 1 } { $i <= $tranumber } { incr i } {
	    set pData $pData$h4$i$nl$nl
	    for { set j 1 } { $j <= 4 } { incr j } {
		set pData $pData$t1[expr {  $j }]$t3$sp[lindex $polyData [expr {  ($i-1)*14+3+$j*2 }]]$nl
		set pData $pData$t2[expr {  $j }]$t3$sp[lindex $polyData [expr {  ($i-1)*14+4+$j*2 }]]$nl
	    }	    
	    #set pData $pData$c$sp[lindex $polyData [expr {  ($i-1)*14+13 }]]$nl
	    set pData $pData$nl
	}
    }
 
}

proc caldata { } {

    global Nh Nit J
    global Nwx Nwy Nws
    global Np Eps matr
    global freq sigma
    global pcal

    set t1 "       Wavelet Point Number:"
    set t2 "        Number of Iteration:"
    set t3 "           Resolution Level:"
    set t4 "         Gauss Quadrature X:"
    set t5 "         Gauss Quadrature Y:"
    set t6 "      \# of Segments/Wavelet:"
    set t7 "             OPQ Seperation:"
    set t8 "              STAB Accuracy:"
    set t9 "            Matrix Solution:"
    set t10 "              Frequency(Hz):"
    set t11 "          Conductivity(S/m):"

    set nl "\n"
    set sp "   "

    set sep "     -------------------------------------"
    set hh "            The Calculation Parameters"

    set pcal $nl$sep$nl$hh$nl$sep$nl$nl
    set pcal $pcal$t1$sp$Nh$nl
    set pcal $pcal$t2$sp$Nit$nl
    set pcal $pcal$t3$sp$J$nl
    set pcal $pcal$t4$sp$Nwx$nl
    set pcal $pcal$t5$sp$Nwy$nl
    set pcal $pcal$t6$sp$Nws$nl
    set pcal $pcal$t7$sp$Np$nl
    set pcal $pcal$t8$sp$Eps$nl
    set pcal $pcal$t9$sp$matr$nl
    set pcal $pcal$t10$sp$freq$nl
    set pcal $pcal$t11$sp$sigma$nl
    

}

proc delall { } {
    
    global fileHead
    global recTags cirTags dieleTags ellTags triTags polyTags gndTags

    set gndnumber [lindex $fileHead 6]
    set recnumber [lindex $fileHead 1]
    set cirnumber [lindex $fileHead 2]
    set tranumber [lindex $fileHead 5]
   
    if { $gndnumber > 0 } {
	for { set i 0 } { $i < $gndnumber } {incr i} {
	    .c delete [lindex $gndTags $i]
	}
    }
   
    if { $recnumber > 0 } {
	for { set i 0 } { $i < $recnumber } {incr i} {
	    .c delete [lindex $recTags $i]
	}
    }
   
    if { $cirnumber > 0 } {
	for { set i 0 } { $i < $cirnumber } {incr i} {
	    .c delete [lindex $cirTags $i]
	}
    } 
  
    if { $tranumber > 0 } {
	for { set i 0 } { $i < $tranumber } {incr i} {
	    .c delete [lindex $polyTags $i]
	}
    }    
}

proc initc { flag } {

    global fileHead;# the number of each kind of conductor and dielectric
    
    global dieleData recData cirData triData ellData polyData gndData
    # global variables for saving all parameters of each kind
    global palFlag
    global corFlag
    global newFlag

    global recCounter cirCounter dieleCounter ellCounter triCounter polyCounter \
	gndCounter
    global recTags cirTags dieleTags ellTags triTags polyTags gndTags
    global recId cirId dieleId ellId triId polyId gndId
    
    global maxXcoord maxYcoord
    global paletteXsize paletteYsize Xscale Yscale
    global pbottomleftX pbottomleftY

    global Nh Nit J
    global Nwx Nwy Nws
    global Np Eps matr
    global freq sigma
    global pData pcal
    
    set fileHead { 0 0 0 0 0 0 0 }; # dieleNum recNum cirNum triNum ellNum polyNum gndNum

    set corFlag -1
    set newFlag -1

    set recCounter 0
    set cirCounter 0
    set dieleCounter 0
    set ellCounter 0
    set triCounter 0
    set polyCounter 0
    set gndCounter 0

    set recId ""
    set cirId ""
    set dieleId ""
    set ellId ""
    set triId ""
    set polyId ""
    set gndId ""

    set recData ""
    set cirData ""
    set dieleData ""
    set ellData ""
    set triData ""
    set polyData ""
    set gndData ""

    set recTags ""
    set cirTags ""
    set dieleTags ""
    set ellTags ""
    set triTags ""
    set polyTags ""
    set gndTags ""
      
    set pData ""
    set pcal  ""
    
    set maxXcoord 879.00000
    set maxYcoord 586.00000

    if { $flag == 1 } {

	set paletteXsize 0
	set paletteYsize 0

	set pbottomleftX 0
	set pbottomleftY 0
	
	set palFlag -1

	set Xscale 0
	set Yscale 0
	
    }
    
    set Nh 1024
    set Nit 40
    set J 6
    
    set Nwx 8
    set Nwy 10
    set Nws 5

    set Np 0.0
    set Eps 1.0e-5
    set matr 1

    set freq 1.0e6
    set sigma 5.6e7

    bind .c <Button-1> { }
    bind .c <B1-Motion> { }
    bind .c <ButtonRelease-1> { }
    bind .c <Motion> { }
    
}

proc main { argc argv } {
    global myAppDirName
    global myAppFileName
    global_init


    if { $argc == 1 } {
        set myAppFileName [lindex $argv 0]
        puts "file: $myAppFileName"
##	puts "file: $myAppFileName"
	myAppFileOpen $myAppFileName
    }
}


main $argc $argv

