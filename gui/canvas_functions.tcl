#----------------------------------------------------------------
#
#  canvas_functions.tcl
#
#
#  TNT GUI Canvas related callbacks and functions.
#
#   ::gui::_canvas_zoom
#   ::gui::_canvas_zoom_fit
#   ::gui::_canvas_zooming_mode
#   ::gui::_canvas_zoomStart
#   ::gui::_canvas_zoomMove
#   ::gui::_canvas_zoomEnd
#   ::gui::_canvas_redraw
#   ::gui::_canvas_toggleAnnotation
#   ::gui::_canvas_select
#   ::gui::_canvas_print
#
#
#  Bob Techentin, April 20, 2004
#  Copyright 2004 Mayo Foundation.  All rights reserved.
#  $Id: canvas_functions.tcl,v 1.7 2004/07/26 13:36:32 techenti Exp $
#
#----------------------------------------------------------------


#----------------------------------------------------------------
#
#  ::gui::_canvas_zoom
#
#  Zoom the view in the canvas by a factor.  If a new
#  centerpoint is specified, recenter the canvas view at
#  the new centerpoint.
#
#  This function is usually not called directly by the
#  user, but by other functions in this module.
#
# Arguments:
#    c         Canvas widget
#    factor    zoom factor.  If factor>1, then zoom in,
#              if factor<1, then zoom out.
#    xcenter
#    ycenter   (optional) X and Y centerpoints for new
#              view.
#
# Results:
#  This procedure has the side effect of rescaling all
#  canvas items and adjusting the scrollregion and xview/yview.
#  Although this changes the coordinates of all the
#  canvas items, it saves a cumulative scale factor, so
#  new canvas items can be drawn and scaled to match the
#  rest of the items in the view.
#
#----------------------------------------------------------------
proc ::gui::_canvas_zoom {c factor {xcenter ""} {ycenter ""}} {

    #  Check to see if we have anything to show
    if {[$c find all] eq ""} {
	return
    }

    #  Check that factor is valid
    if { $factor <= 0.0 } {
	error "Invalid Zoom Factor"
    }

    #  Get canvas width and height, in pixels
    set w [winfo width  $c]
    set h [winfo height $c]

    #  Get display corner points, in canvas units
    set xa [$c canvasx  0]
    set xe [$c canvasx $w]
    set ya [$c canvasy  0]
    set ye [$c canvasy $h]

    #  Compute the new, scaled center point
    if { $xcenter eq "" } {
	set xcenter [expr {$factor * ($xe + $xa) / 2.0}]
	set ycenter [expr {$factor * ($ye + $ya) / 2.0}]
    } else {
	set xcenter [expr {$factor * $xcenter}]
	set ycenter [expr {$factor * $ycenter}]
    }

    #  Scale all canvas items and the scrollregion
    $c scale all 0 0 $factor $factor
    foreach {x0 y0 x1 y1} [$c bbox all] {break}
    $c configure -scrollregion [list $x0 $y0 $x1 $y1]

    #  Compute new xview/yview fractions to move display
    #  to correct location
    set newxa [expr {$xcenter - $w * 0.5}]
    set newya [expr {$ycenter - $h * 0.5}]
    set newxview [expr {(1.0 * $newxa - $x0) / ($x1 - $x0)}]
    set newyview [expr {(1.0 * $newya - $y0) / ($y1 - $y0)}]
    $c xview moveto $newxview
    $c yview moveto $newyview

    #  Save the scale factor for later
    set ::gui::canvasScaleFactor [expr {$::gui::canvasScaleFactor * $factor}]
}


#----------------------------------------------------------------
#
#  ::gui::_canvas_zoom_fit
#
#  Zoom the canvas to view everything in the window.
#  This function essentially computes a bounding box
#  for everything on the canvas, computes a scale factor
#  and calls _canvas_zoom.
#
# Arguments:
#   c         Name of canvas widget
#
# Results:
#   Rescales everything on the canvas so that it fits
#   on the view.  Resets the scroll region, and saves
#   the zoom factor.
#
#----------------------------------------------------------------
proc ::gui::_canvas_zoom_fit {c} {

    #  Check to see if we have anything to show
    if {[$c find all] eq ""} {
	return
    }

    #  Get canvas width and height, in pixels
    set w [winfo width  $c]
    set h [winfo height $c]

    #  Get scroll region bounding box
    foreach {x0 y0 x1 y1} [$c bbox withtag shape] {break}
    set dw [expr {$x1 - $x0}]
    set dh [expr {$y1 - $y0}]

    #  compute scale factor
    if { (1.0*$dw/$w) > (1.0*$dh/$h) } {
	set factor [expr {0.9 * $w / $dw}]
    } else {
	set factor [expr {0.9 * $h / $dh}]
    }

    #  Scale all canvas items and the scrollregion
    $c scale all 0 0 $factor $factor
    set x0 [expr {$x0 * $factor}]
    set y0 [expr {$y0 * $factor}]
    set x1 [expr {$x1 * $factor}]
    set y1 [expr {$y1 * $factor}]
    $c configure -scrollregion [$c bbox all]

    #  Save the scale factor for later
    set ::gui::canvasScaleFactor [expr {$::gui::canvasScaleFactor * $factor}]
}



#----------------------------------------------------------------
#
#  Define "zooming" bindings, which are
#  activated by _canvas_zooming_mode and
#  removed by _canvas_zoomEnd
#
#----------------------------------------------------------------
bind zooming <ButtonPress-1>   {::gui::_canvas_zoomStart %W %x %y}
bind zooming <B1-Motion>       {::gui::_canvas_zoomMove  %W %x %y}
bind zooming <ButtonRelease-1> {::gui::_canvas_zoomEnd   %W %x %y}


#----------------------------------------------------------------
#
#  ::gui::_canvas_zooming_mode
#
#  Set the appearance and behavior for a zoom selection -
#  where the users clicks and drags a rectangle to specify the
#  the new viewport.
#
# Arguments:
#    c          canvas widget
#
# Results:
#  Current canvas bindings are saved.  Cursor is changed,
#  and new bindtags are set so that zoomStart, zoomMove and 
#  zoomEndwill be called.
#
#----------------------------------------------------------------
proc ::gui::_canvas_zooming_mode {c} {

    $c configure -cursor crosshair
    set ::gui::_canvas_save_bindtags [bindtags $c]
    bindtags $c zooming
}


#----------------------------------------------------------------
#
#  ::gui::_canvas_zoomStart
#
#  Start a zoom selection.  Saves the initial coordinates
#  and start drawing a selection rectangle.
#
#  This procedure is usually called by a <ButtonDown> event
#
# Arguments:
#   c         Name of canvas widget
#   x, y      (x,y) coordinate for first corner of rectangle
#
# Results:
#    Saves starting point and creates a temporary
#    rectangle on the canvas.
#
#----------------------------------------------------------------
proc ::gui::_canvas_zoomStart {c x y} {

    #  Save the starting point
    set x [$c canvasx $x]
    set y [$c canvasy $y]
    set ::gui::_canvas_zoom_startx $x
    set ::gui::_canvas_zoom_starty $y

    #  Draw a zoom rectangle
    $c create rectangle $x $y $x $y  -tags zoomBox

}


#----------------------------------------------------------------
#
#  ::gui::_canvas_zoomMove
#
#  Continue a zoom selection.  Adjust the selection
#  rectangle to follow the drag.
#
# Arguments:
#   c         Name of canvas widget
#   x, y      New (x,y) coordinate for second corner of rectangle
#
# Results:
#   Moves temporary rectangle on canvas
#
#----------------------------------------------------------------
proc ::gui::_canvas_zoomMove {c x y} {
    #  Move the rectangle
    set x [$c canvasx $x]
    set y [$c canvasy $y]
    $c coords zoomBox \
	$::gui::_canvas_zoom_startx $::gui::_canvas_zoom_starty $x $y
}


#----------------------------------------------------------------
#
#  ::gui::_canvas_zoomEnd
#
#  Finished selecting a new zoom area.  (on ButtonRelease)
#  Restore the "normal" appearance and behaviors to the
#  canvas, then compute the values necessary to call
#  _canvas_zoom.
#
# Arguments:
#    c          canvas widget
#    x, y       Final (x,y) coordinate for second corner of rectangle
#
# Results:
#    Resets cursor and restores canvas bindings.  Computes
#    relative zoom factor and centerpoint for new zoom view.
#    (...which rescales everything on the canvas)
#
#----------------------------------------------------------------
proc ::gui::_canvas_zoomEnd {c x y} {
    #  Restore appearance and behaviors
    $c configure -cursor {}
    bindtags $c $::gui::_canvas_save_bindtags
    $c delete zoomBox

    #  Get final xy coordinates
    set x0 $::gui::_canvas_zoom_startx
    set y0 $::gui::_canvas_zoom_starty
    set x1 [$c canvasx $x]
    set y1 [$c canvasy $y]

    #  Now compute new zoombox and move everything.
    #  Work in terms of the centerpoint
    set xrange [expr {abs($x1 - $x0)}]
    set yrange [expr {abs($y1 - $y0)}]
    if { $xrange == 0 || $yrange == 0 } {
	return
    }
    set xcenter [expr {($x0 + $x1) / 2.0}]
    set ycenter [expr {($y0 + $y1) / 2.0}]
    #  We could get coords for x=0 and x=width, and subtract
    #  to get the canvas coordinates width, but its really just
    #  the same as the canvas widget width, in pixels.
    set w [winfo width  $c]
    set h [winfo height $c]
    set xfactor [expr {1.0 * $w / $xrange}]
    set yfactor [expr {1.0 * $h / $yrange}]
    if { $xfactor > $yfactor } {
	set factor $yfactor
    } else {
	set factor $xfactor
    }
    _canvas_zoom $c $factor $xcenter $ycenter
}


#----------------------------------------------------------------
#
#  ::gui::_canvas_redraw
#
#  Redraw the cross section canvas from scratch.
#
# Arguments:
#    none
#
# Results:
#  Deletes all the existing canvas objects and
#  redraws them using the canvasDraw visitor.
#  Then rescales according to save scale factor.
#  (This all happens so fast that users might not
#   even notice.)
#
#----------------------------------------------------------------
proc ::gui::_canvas_redraw {} {

    set c $::gui::_canvas

    #  Delete everything
    $c delete all

    #  Check to see if there is something to draw
    if { [llength $::Stackup::structureList] > 0 } {

	#  Make sure we've got a canvasDraw visitor, then call it
	if { [itcl::find objects ::gui::_canvasDraw_visitor] == "" } {
	    canvasDraw ::gui::_canvasDraw_visitor
	}
	Stackup::accept ::gui::_canvasDraw_visitor $c

	#  Rescale to the previous view.
	$c scale all 0 0 $::gui::canvasScaleFactor -$::gui::canvasScaleFactor


	#  Add a title
	if { $::gui::_annotateFlag  && ($::gui::_title ne "")} {
	    foreach {x0 y0 x1 y1} [$c bbox withtag shape] {break}
	    set x [expr {($x0 + $x1) / 2}]
	    set y [expr {$y0 * 1.1}]
	    $c create text $x $y  \
		-justify center -text $::gui::_title
	}

    }

}



#----------------------------------------------------------------
#
#  ::gui::_canvas_toggleAnnotation
#
#  Cross section annotations (arrows and dimensions)
#  are controlled with a global variable, which can
#  be toggled, presumably from the menu.
#
# Arguments:
#    none
#
# Results:
#  Note that turning annotations on or off forces a redraw.
#
#----------------------------------------------------------------
proc ::gui::_canvas_toggleAnnotation {} {

    ::gui::_canvasDraw_visitor configure -annotate $::gui::_annotateFlag
    ::gui::_canvas_redraw
}

#----------------------------------------------------------------
#
#  ::gui::_canvas_select
#
#  Select an item on the canvas, highlighting it.
#
#  _canvas_select is bound to mouse clicks, and finds the
#  current item in the canvas, looks up the
#  object name in the tags, and highlights it.
#
#  The API expects %x,%y from a mouse click, although we
#  just use the 'current' object from the canvas.  If
#  there were overlapping objects, then it might be
#  necessary to 'find overlapping' and cycle throught
#  the objects found.
#
# Arguments:
#    c          canvas widget
#    x, y       Final (x,y) coordinate for second corner of rectangle
#
# Results:
#  The canvas 'current' object is selected, decoded, and
#  passed to ::gui::highlightItem which changes its color.
#
#----------------------------------------------------------------
proc ::gui::_canvas_select { c x y } {

    #  Find the canvas object under the click
    set obj [$c find withtag current]

    #  Get the name of the Stackup object
    set objname [lindex [$c gettags $obj] 0]

    #  If the tag name is "current", then we probably
    #  got a click on the title or some other un-tagged
    #  graphical element.
    if { $objname ne "current" } {
	#  Highlight on the canvas and the tree
	::gui::highlightItem $objname
    }
}



#---------------------------------------------------------
#
#  ::gui::_canvas_print
#
#  Print the canvas picture of the cross section.
#
#  We could create a dialog which utilizes the postscript
#  features of the canvas and special Windows extensions
#  to provide a uniform printing interface.  But that's
#  just too much work for today.
#
#  Instead, we use the Iwidgets canvas print dialog, which
#  does pretty much everything we want on Unix.
#
#  On windows, we look for a program called PrFile32,
#  which can send the postscript output to a printer.
#  Hopefully, the user has a postscript printer.
#
# Arguments:
#    none
#
# Results:
#  IWidgets print dialog is created.  Windows or unix
#  print command is executed.
#
#---------------------------------------------------------
proc ::gui::_canvas_print { } {

    #---------------------------------------------------------
    #  Create the print dialog, if necessary
    #---------------------------------------------------------
    if { $::gui::dialog(canvasPrint,dialog) eq "" } {

	#---------------------------------------------------------
	#  Set defaults for attributes
	#---------------------------------------------------------
	set output "printer"
	set orient "landscape"
	set pagesize "letter"

	#---------------------------------------------------------
	#  Figure out what our default print command should be
	#---------------------------------------------------------
	if { $::tcl_platform(platform) eq "windows"} {

	    #---------------------------------------------------------
	    # On windows, we hope that someone has kindly installed
	    #  PrFile32.exe from http://www.lerup.com/printfile.
	    #  For this application, we expect it to be in
	    #  on the path.
	    #---------------------------------------------------------
	    set printcmd [auto_execok "prfile32"]
	    if { $printcmd ne "" } {
		append printcmd " /- /q"
	    } else {
		set output "file"
	    }

	} else {

	    #---------------------------------------------------------
	    #  On Unix, we depend on "lpstat -d" for default printer name
	    #---------------------------------------------------------
	    set printcmd "lp"
	    catch {
		set result [exec lpstat -d]
		if {[scan $result "system default destination: %s" printer]} {
		    append printcmd " -d$printer"
		}
	    }
	}

	#---------------------------------------------------------
	#  Create the Iwidget dialog
	#---------------------------------------------------------
	package require Iwidgets
	set dlg [iwidgets::canvasprintdialog .canvasPrintDialog \
		     -orient $orient -pagesize $pagesize \
		     -printcmd $printcmd -output $output]
	$dlg setcanvas $::gui::_canvas

	#---------------------------------------------------------
	#  For some odd reason, we have to tell the dialog what do
	#  do when the user clicks on the "Print" button.  The 
	#  demo code uses an application modal dialog and checks 
	#  the return code.  But we don't want a modal dialog here.
	#  So we configure the "Print" button to call "print."
	#---------------------------------------------------------
	$dlg buttonconfigure 0 -command [list ::gui::printAndDeactivate $dlg]

	#---------------------------------------------------------
	#  Save the widget name for next time
	#---------------------------------------------------------
	set ::gui::dialog(canvasPrint,dialog) $dlg
    } else {
	set dlg $::gui::dialog(canvasPrint,dialog)
    }


    #---------------------------------------------------------
    #  Activate the dialog
    #---------------------------------------------------------
    set filename "$::gui::_nodename.ps"
    $dlg configure -filename $filename
    $dlg activate
}


#---------------------------------------------------------
#
#  ::gui::printAndDeactivate
#
#  Helper proc for the printCanvasDialog, calls the
#  print method and deactivates (closes) the dialog.
#
# Arguments:
#    dlg      name of the canvasprintdialog widget
#
# Results:
#  IWidgets print dialog is deactivated and 
#  the print method is executed.
#
#---------------------------------------------------------
proc  ::gui::printAndDeactivate {dlg} {

    #---------------------------------------------------------
    #  On Unix systems we can just call the print method.
    #  When printing to a file, we can just call the print
    #  method.  But printing to a printer on Windows is broken.
    #  Unfortunately, Iwidgets 4.0.2 has a bug in that it
    #  uses temporary files named "/tmp/xge...", which 
    #  just doesn't work on Windows.
    #---------------------------------------------------------
    if {($::tcl_platform(platform) ne "windows") || 
	([$dlg cget -output] eq "file")} {
	$dlg print
    } else {

	#---------------------------------------------------------
	#  On windows, we're going to have to define our
	#  own temporary file name, print to that file,
	#  then run the print command, as is done in
	#  iwidgets::Canvasprintbox::print
	#---------------------------------------------------------
	set savefilename [$dlg cget -filename]
	set tmpfile [::fileutil::tempfile xge]
	$dlg configure -filename $tmpfile
	$dlg configure -output "file"
	$dlg print
	set cmd "[$dlg cget -printcmd] < $tmpfile"
	eval exec $cmd &

	#  now put it all back
	$dlg configure -filename $savefilename
	$dlg configure -output "printer"
    }

    #  now we're done printing
    $dlg deactivate
}


#---------------------------------------------------------
#  ::fileutil
#
#  The ::fileutil::tempfile function is copied from
#  tcllib 1.6 to help work around a bug in the
#  iwidgets::canvasPrintDialog.  By including the
#  routine directly, we do not depend on tcllib.
#
#---------------------------------------------------------
namespace eval ::fileutil {}


# ::fileutil::tempdir --
#
#	Return the correct directory to use for temporary files.
#	Python attempts this sequence, which seems logical:
#
#       1. The directory named by the `TMPDIR' environment variable.
#
#       2. The directory named by the `TEMP' environment variable.
#
#       3. The directory named by the `TMP' environment variable.
#
#       4. A platform-specific location:
#            * On Macintosh, the `Temporary Items' folder.
#
#            * On Windows, the directories `C:\\TEMP', `C:\\TMP',
#              `\\TEMP', and `\\TMP', in that order.
#
#            * On all other platforms, the directories `/tmp',
#              `/var/tmp', and `/usr/tmp', in that order.
#
#        5. As a last resort, the current working directory.
#
# Arguments:
#	None.
#
# Side Effects:
#	None.
#
# Results:
#	The directory for temporary files.

proc ::fileutil::TempDir {} {
    global tcl_platform env
    set attempdirs [list]

    foreach tmp {TMPDIR TEMP TMP} {
	if { [info exists env($tmp)] } {
	    lappend attempdirs $env($tmp)
	}
    }

    switch $tcl_platform(platform) {
	windows {
	    lappend attempdirs "C:\\TEMP" "C:\\TMP" "\\TEMP" "\\TMP"
	}
	macintosh {
	    set tmpdir $env(TRASH_FOLDER)  ;# a better place?
	}
	default {
	    lappend attempdirs [file join / tmp] \
		[file join / var tmp] [file join / usr tmp]
	}
    }

    foreach tmp $attempdirs {
	if { [file isdirectory $tmp] && [file writable $tmp] } {
	    return $tmp
	}
    }

    # If nothing else worked...
    return [pwd]
}

if { [package vcompare [package provide Tcl] 8.4] < 0 } {
    proc ::fileutil::tempdir {} {
	return [TempDir]
    }
} else {
    proc ::fileutil::tempdir {} {
	return [file normalize [TempDir]]
    }
}

# ::fileutil::tempfile --
#
#   generate a temporary file name suitable for writing to
#   the file name will be unique, writable and will be in the 
#   appropriate system specific temp directory
#   Code taken from http://mini.net/tcl/772 attributed to
#    Igor Volobouev and anon.
#
# Arguments:
#   prefix     - a prefix for the filename, p
# Results:
#   returns a file name
#

proc ::fileutil::TempFile {prefix} {
    set tmpdir [tempdir]

    set chars "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
    set nrand_chars 10
    set maxtries 10
    set access [list RDWR CREAT EXCL TRUNC]
    set permission 0600
    set channel ""
    set checked_dir_writable 0
    set mypid [pid]
    for {set i 0} {$i < $maxtries} {incr i} {
 	set newname $prefix
 	for {set j 0} {$j < $nrand_chars} {incr j} {
 	    append newname [string index $chars \
		    [expr {([clock clicks] ^ $mypid) % 62}]]
 	}
	set newname [file join $tmpdir $newname]
 	if {[file exists $newname]} {
 	    after 1
 	} else {
 	    if {[catch {open $newname $access $permission} channel]} {
 		if {!$checked_dir_writable} {
 		    set dirname [file dirname $newname]
 		    if {![file writable $dirname]} {
 			return -code error "Directory $dirname is not writable"
 		    }
 		    set checked_dir_writable 1
 		}
 	    } else {
 		# Success
		close $channel
 		return $newname
 	    }
 	}
    }
    if {[string compare $channel ""]} {
 	return -code error "Failed to open a temporary file: $channel"
    } else {
 	return -code error "Failed to find an unused temporary file name"
    }
}

if { [package vcompare [package provide Tcl] 8.4] < 0 } {
    proc ::fileutil::tempfile {{prefix {}}} {
	return [TempFile $prefix]
    }
} else {
    proc ::fileutil::tempfile {{prefix {}}} {
	return [file normalize [TempFile $prefix]]
    }
}

