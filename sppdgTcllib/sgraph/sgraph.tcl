###############################################################
#
#  sgraph
#
#  This "supergraph" is an enhanced BLT graph widget.
#  The sgraph includes the standard BLT graph, plus
#  a number of buttons for Zoom, Save, and Print
#  functionality.  Sgraph also includes scrollbars.
#
#  usage:
#
#     package require sgraph
#     set g [sgraph::sgraph .myGraph]
#     pack $g -expand y -fill both
#
#
#  Implementation:
#
#  The sgraph command actually creates a frame widget and
#  chucks all the interesting extra stuff (like buttons and
#  the "real graph" widget) into the frame.  The widget
#  command is aliased (using a technique outlined at 
#  http://wiki.tcl.tk/1146) so that you can directly
#  access all the regular blt::graph subcommands and
#  configuration options.
#
#
#  Added calls that can be used in the application:
#        ::sgraph::Save
#           graph      - graph created with ::sgraph::sgraph
#           psfilename - name of the PostScript file
#        ::sgraph::Print
#           graph      - graph crated with ::sgraph::sgraph
#           flg        - 1 to popup print options dialog
#                        0 for no popup, will use existing
#                          values
#        ::sgraph::SetPrinter
#           prntrNme   - name of the printer
#        ::sgraph::SetLandscape
#           val        -  0 for portrait
#                      -  1 for landscape
#        ::sgraph::SetMaxpect        0 or 1
#           val        -  0 for no maxpect
#                      -  1 for maxpect
#        ::sgraph::SetDecorations    0 or 1
#           val        -  0 for no decorations
#                      -  1 for decorations
#        ::sgraph::SetColormode      color or grayscale
#           val        -  color
#                      -  grayscale
#
################################################################
package provide sgraph 1.0

package require Tk
package require BLT
package require BWidget

################################################################
# namespace for sgraph
################################################################
namespace eval ::sgraph {

    # Set up the display of the Zoom box outline
    variable interval
    variable afterId

    # Index of currently active graph
    variable curIndx -1

    # Currently active graph
    variable curGraph 0

    # Last zoom extents
    variable oldxA     0
    variable oldyA     0
    variable oldxB     0
    variable oldyB     0

    # First time into zoom
    variable first

    # Index for for grph array
    variable grph_cnt   -1

    # array of grph identifiers
    variable grph
    array set grph {}

    # arrays of graph extents
    variable grph_xmin
    variable grph_ymin
    variable grph_xmax
    variable grph_ymax

    # Icon images for the zoom-in, zoom-out, 
    # zoom-all and print buttons
    variable print
    variable viewall
    variable viewin
    variable viewout
    variable saveas
    variable info

    
    variable active_mode
    array set active_mode {}
    variable text_frme    0

    # Variables used for printing/saving to 
    # an output file
    variable dlg      0
    variable landscape   1
    variable maxpect     1
    variable decorations 0
    variable colormode   color
    variable save_file   0

    # Printer info collected on unix
    variable current_printer {}
    variable printer_list  {}
    variable default_printer  {}

    # Output file name
    variable file_name {}

    namespace export sgraph
}

image create photo sgraph::viewout -data {
   R0lGODlhEAAQAIYAAPwCBHSi1ISu3Hym1HSizGSWxJS65LTW9OT2/Pz+/PT+
   /PT6/KTK5DxmjOz2/Oz+/Lzm/BxGdHSe1Lza9Nzq/FSKtIyy5Lze/KzW/AQy
   VLTW/KzS/LTS/JTC9MTq/MTi/KzK/KTG/KTC/Jy+/BQuRKzG/Jy6/JSy/Jy2
   /DRmpAQOHER6rJSy9Iym9GSK9FyKpIyCBDxypHSi3HyW/DxuxFxqfPT+dPzm
   bHRCBAQmPBxWlARGfMRyDOSiLNzCnOzCfIRKDMSGNOTGlIxODOzChIROBGyi
   1MyORLRyFER6tAxSjJxeJGQ6BCQWBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAA
   LAAAAAAQABAAAAeggACCAQIDBAWCiYoABgcICQoLDA2LggYOCQkLmw8QEYsS
   E5mbCBQUEBWLFqObphcXGBmKAw6kFBcaGxwdsokBHh/BHBsgISIjvYIkIhMH
   GyUiJicoKSqLKxcc0CwtLS4vMJUxMiLcMzQ1Njc4lTk67zsqPD0+P+yVi0BB
   Qvb4i0NBiPwoIkGCERn+hhw5giTJCiU7/AEosoSJxEVNAPgJBAAh/mhDcmVh
   dGVkIGJ5IEJNUFRvR0lGIFBybyB2ZXJzaW9uIDIuNQ0KqSBEZXZlbENvciAx
   OTk3LDE5OTguIEFsbCByaWdodHMgcmVzZXJ2ZWQuDQpodHRwOi8vd3d3LmRl
   dmVsY29yLmNvbQA7
}

image create photo sgraph::viewin -data {
   R0lGODlhEAAQAIYAAPwCBHSi1Iyy5Hym3Hym1GSWxJS65Lza/OT2/Pz+/PT+
   /KTK5DxmlJS67PT6/Pz6/Lzm/BRGhHSe1MTe/OTy/Oz2/Mzm/NTq/FSKtJS2
   7PT2/MTi/LTa/AQyVOTu/Lze/LTS/JTC9MTq/Mzi/KzK/KTC/KS+/BQuRJzG
   /KzO/LTO/KS6/Jy2/DRmpAQOHEx+rLTK/Jy+/Iym/Jyq/GSK/GSOrIyCBHSi
   5HyW/DxuxFxqhPT+dPzqbHRCBAQmPBxenBxSjBxanARGfMRyDOymNOTCnPTG
   fIRKDMyGNOzGlISu7GSOzIROBPTGjJSy9EyGvIxODNSORMSWTLxyFDx6tBxe
   lKRiJGQ6BCQWBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAA
   LAAAAAAQABAAAAesgACCAQIDBAWCiYoABgcICQoJCwyLgg0OCQkPmgoQEYsS
   E5mYFBUWFxiLGZmbGhoWGxMcHYoDChoOFR4fByAcIbSJAyIXIxYgICQkJSbB
   gicovCkqJSssKy0uiy8bIDAxLDIzNDU2lS031TI4OTo7PD2VPj9AQUIuQ0RF
   RvGVi0dIkvATpGSJPwBMkDQxwkSCkyc3DkKJImUKlSdVhBxEaOUKACVVNgrC
   AsBPIAAh/mhDcmVhdGVkIGJ5IEJNUFRvR0lGIFBybyB2ZXJzaW9uIDIuNQ0K
   qSBEZXZlbENvciAxOTk3LDE5OTguIEFsbCByaWdodHMgcmVzZXJ2ZWQuDQpo
   dHRwOi8vd3d3LmRldmVsY29yLmNvbQA7
}

image create photo sgraph::viewall -data {
   R0lGODlhEAAQAIYAAISu5Iyy5Iyy7Hyu5Hyq7HSm5Gyi7GSe5GSe7GSi7ESa
   7AQ2XAT+BLTa/JTK/Hy+/FSq/DSW/AyG/AR+/AR2/AR6/AQyVJS65Mze9MzW
   9LTK9KTO/JzO9JzC/IzK9IS+9Iy27Fy2/PyipOwiLPQ2PPSutPz+/PT6/PT+
   /Pzu9PReZPwqNPxOTJy+5AQyXKTS/PSOlPwCBPwmLPTS1Oz+/Oz6/ORqdIS+
   7AQ2ZOxyfPQmLPQiLOTK1OT+/OT6/KTC7PS6xPRyfOzu9OT2/NyyxOTy/OTq
   /ITC9Nzy/Nz2/NT2/ITG9KS+7NT6/NTy/Nz6/Oy6xORufMzy/NTi9JzO/PQq
   NOxufPwCDPSSnNTu/PQmNAQ2VPyirPSuvPxOXGSq5Iy+7IS67HzC9AQeNAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAwA
   LAAAAAAQABAAAAfsgAABAgMEBQYHCAcJBwYKCwwCDQ4PEBESExQVFROcFgwX
   GBkaGhscHR4fIB8dIZ8cIiMkJSYnJigpKissLS4MLzAxMjO1JzQ1NjErNzgM
   GzA5Ojs8PT4+OTE5KiCQPykmQEFCKEM1RDlFRke+LbU0ND1CQ0g+SUpKS59M
   KO/H80VKmjh54kGfEBpQcghJgkQJkShSpjz4RAVGkCojqDlRYuWKDRUAmnnA
   EmNajyxOpEhJpuXGll9cZJDo0mNIkScYdHn50OxLBzBhbngQM9QDgBs3Qvha
   4AKHCxcWnlqwgKMqjjEMsmrdyjWrn0AAIf5oQ3JlYXRlZCBieSBCTVBUb0dJ
   RiBQcm8gdmVyc2lvbiAyLjUNCqkgRGV2ZWxDb3IgMTk5NywxOTk4LiBBbGwg
   cmlnaHRzIHJlc2VydmVkLg0KaHR0cDovL3d3dy5kZXZlbGNvci5jb20AOw==
}
image create photo sgraph::print -data {
   R0lGODlhEAAQAIYAAPwCBHSm/KzG/IS2/LTS/Pz+/Ozy/KS63Gye7ISu/Ozu
   /Pz6/OTm/JzC/ExmpKTG/PT2/Mze/NTi/NTm/OTq/CQ6dHyi/OTu/LzW/LzS
   /MTW/NTW/Nzu/HSO1MzK/LzC9LS69PT+/Mzi/LTO/CQ6ZPz29GRenFRKfKSi
   zNTq/MTa/HyS1Ly6/Pz2/Ly+zISKpFRShFRelDRGnNze/Ozq/JSSrGRijDQ2
   ZFxytJSOvJR2vEwufNzS/Hx+tLy+3GRqjGRmjNze7PTy/JR+1BwKLExWpLzW
   9Iyi3ISGzJym/HyC/KTO5PT6/JSizFx6zKSm5ISS/IR+9CxSrERmtJSWzHyG
   /IyK9DQiTOT2/FyC3EQ2jHRerJyi/DQeTCw+dFyC1FyG5AQOJEQubAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAA
   LAAAAAAQABAAAAe7gACCgwABAgOEiYQEBQYHCIqECQoFlQsMDQ6RDwUQDBES
   ExQVihYLFxgZGhscHYkeHyAhIqoYESMkhB4lJicoHykqKiIrhCwFLS4vJDAx
   IBERMoQfMyULBTQ1Njc2ODk6O4I8Kjg9Gy3YPj9AQUJDRABFRipHMUgoQgUK
   DBtJSvBLKhVg0sRJjCczNiiBEoWIlCY0BHaaQsVDlSpWrgwisaISlixatnjg
   EqWLIi9fwIQRJEaMyUgwB/kJBAAh/mhDcmVhdGVkIGJ5IEJNUFRvR0lGIFBy
   byB2ZXJzaW9uIDIuNQ0KqSBEZXZlbENvciAxOTk3LDE5OTguIEFsbCByaWdo
   dHMgcmVzZXJ2ZWQuDQpodHRwOi8vd3d3LmRldmVsY29yLmNvbQA7
}

image create photo sgraph::saveas -data {
   R0lGODlhEAARAIUAAPwCBGRebFxaZOTm9OTi7ExWZPz+/NTS7MTC5Pz6/MS+
   3Nza7Nza9LS21MzO5NTK5CwqLIR+tFxabLSu1Ozm9JSSxOTa5DQyPISCvBQW
   HAQCBIyKvFRSdOTu9GxmpHRyrHx+tBwaHOTe9Kyq1GxqpKSmzBwWHGRinHx6
   tFxenExKjFxanAwKFGxqjAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAALAAAAAAQABEAAAaR
   QIBwKAwQj0fBQIBEBggFg7EpFEANWGYzcAhgv1OiACEwJL5ZYkCBQHgTC8Zi
   0a02FAIH+fA4zCEREgETDQIUAgcVARQWFBcYGBkaZBQMhxUbHB0QHh8ZQhoU
   FBOWEhggIR+eRBoHIhMUIwIfJKtHGiIUFSUmJyifTRojvCm2VBoZKisZHFRD
   GpHNzkMsLU0yQQAh/mhDcmVhdGVkIGJ5IEJNUFRvR0lGIFBybyB2ZXJzaW9u
   IDIuNQ0KqSBEZXZlbENvciAxOTk3LDE5OTguIEFsbCByaWdodHMgcmVzZXJ2
   ZWQuDQpodHRwOi8vd3d3LmRldmVsY29yLmNvbQA7
}

################################################################
# sgraph 
#
#   w   : the name of the frame to be created - this frame
#         will contain the graph, buttons and the scroll-bars
#
#   args: arguments to be used when the graph is being 
#         created
#
# This routine wil overload the frame widget so all commands 
# sent to the frame will be passed to the graph widget.
#
################################################################
proc ::sgraph::sgraph { w args } {

    #
    # Increment the counter for the grph array
    #
    incr sgraph::grph_cnt

    #
    # Create a frame and a graph
    #
    frame $w
    set grph [eval blt::graph $w.realgraph $args]

    #
    #  Create scrollbars and configure them to work with x and y axis
    #
    set scrllx [scrollbar $w.hs1 \
	    -command [list $w.realgraph axis view x ] \
	    -orient horizontal]
    set scrlly [scrollbar $w.vs1 \
	    -command [list $w.realgraph axis view y] \
	    -orient vertical]
    $grph axis configure x -scrollcommand [list $scrllx set]
    $grph axis configure y -scrollcommand [list $scrlly set]
    

    set sgraph::active_mode($sgraph::grph_cnt) "   Active mode"
    set sgraph::text_frme [LabelFrame $w.infofrme -anchor w \
	    -textvariable sgraph::active_mode($sgraph::grph_cnt) -side right]
    set frme [$sgraph::text_frme getframe]
    #
    #  Create active buttons in a buttonbox
    #
    set bbox [ButtonBox $frme.bbox]
    $bbox add -image sgraph::viewin \
	    -helptext "Zoom In by defining diagonal corners of a rectangle" \
	    -command [list sgraph::ZoomIn $sgraph::grph_cnt]
    $bbox add -image sgraph::viewout \
	    -helptext "Zoom Out by picking the center point of the new window" \
	    -command [list sgraph::ZoomOut $sgraph::grph_cnt]
    $bbox add -image sgraph::viewall \
	    -helptext "Display the full graph" \
	    -command [list sgraph::ZoomAll $sgraph::grph_cnt]
    $bbox add -image sgraph::saveas \
	    -helptext "Save to a file as PostScript or Metafile" \
	    -command [list sgraph::SaveGraph $sgraph::grph_cnt]
    $bbox add -image sgraph::print \
	    -helptext "Output the graph to a printer" \
	    -command [list sgraph::PrintTheGraph $sgraph::grph_cnt]

    #
    #  Grid the buttons, graph, and scrollbars
    #
    grid $bbox -sticky w
    grid $sgraph::text_frme -sticky w

    grid $grph    $scrlly  -sticky news
    grid $scrllx           -sticky news

    #
    #  Set for resizing the graph - row 1, column 0 will grow
    #  if the window is enlarged.
    #
    grid columnconfigure $w 0 -weight 1
    grid rowconfigure $w 1 -weight 1

    #
    # Save the graph and graph count. Set flag that graph
    # graph was just created
    #
    set sgraph::first($sgraph::grph_cnt) 1
    set sgraph::grph($sgraph::grph_cnt) $grph
    set sgraph::curIndx $sgraph::grph_cnt
    set sgraph::curGraph $grph
    #
    # If unix, this will get the list of available printers
    # and the default printer
    #
    sgraph::InitializePrinter

    #
    # Keep the original frame widget command - won't be using
    #
    rename $w _$w
    
    #
    # Use the interpreter command aliases to take advantage of
    # the ability to add extra arguments to the command via
    # the alias. The following command will Send all frame 
    # widget commands to the graph widget
    interp alias {} $w {} $grph

    #
    # Return the frame widget
    #
    return $w ;  # like the original graph command 
}

option add *zoomOutline.dashes		4	
option add *zoomOutline.lineWidth	2
option add *zoomOutline.xor		yes


################################################################
# Allow the user to set the printer and PostScript options.
################################################################
proc ::sgraph::SetPrinter { prntrNme } {
    set sgraph::current_printer $prntrNme
}

proc ::sgraph::SetLandscape { val } {
    set sgraph::landscape $val
}

proc ::sgraph::SetMaxpect { val } {
    set sgraph::maxpect $val
}

proc ::sgraph::SetDecorations { val } {
    set sgraph::decorations $val
}

proc ::sgraph::SetColormode { val } {
    if { $val != "color" && $val != "grayscale" } {
	tk_messageBox -message "Colormode of \"$val\" not valid: \
		\"color\" or \"grayscale\" are the valid values"
	return
    }
    set sgraph::colormode $val
}


################################################################
# Generate hardcopy of the graph with a flag whether to popup
# the dialog to set the printer/PostScript options.
################################################################
proc ::sgraph::Print { graph flg } {

    foreach { indx g } [array get sgraph::grph] {
	if { [string first $graph $g] > -1 } {
	    sgraph::PrintTheGraph $indx $flg
	}
    }
}

################################################################
# Draw an outline of the rectangle that will define the area of
# the graph to display
################################################################
proc ::sgraph::Box { graph } {

    #
    # Sometimes a pick is not inside the data window and sets value to ""
    #
    if { ($sgraph::info($graph,A,x) == "") || \
	    ($sgraph::info($graph,B,x) == "") } {
	return
    }
    #
    # Save the lower-left and upper-right coordinates of the window
    #
    if { $sgraph::info($graph,A,x) > $sgraph::info($graph,B,x) } { 
	set x1 [$graph xaxis invtransform $sgraph::info($graph,B,x)]
	set y1 [$graph yaxis invtransform $sgraph::info($graph,B,y)]
	set x2 [$graph xaxis invtransform $sgraph::info($graph,A,x)]
	set y2 [$graph yaxis invtransform $sgraph::info($graph,A,y)]
    } else {
	set x1 [$graph xaxis invtransform $sgraph::info($graph,A,x)]
	set y1 [$graph yaxis invtransform $sgraph::info($graph,A,y)]
	set x2 [$graph xaxis invtransform $sgraph::info($graph,B,x)]
	set y2 [$graph yaxis invtransform $sgraph::info($graph,B,y)]
    }
    #
    # Draw the zoom outline
    #
    set coords { $x1 $y1 $x2 $y1 $x2 $y2 $x1 $y2 $x1 $y1 }
    if { [$graph marker exists "zoomOutline"] } {
	$graph marker configure "zoomOutline" -coords $coords 
    } else {
	set X [lindex [$graph xaxis use] 0]
	set Y [lindex [$graph yaxis use] 0]
	$graph marker create line -coords $coords -name "zoomOutline" \
	    -mapx $X -mapy $Y
	set interval $sgraph::interval
	set id [after $interval [list sgraph::MarchingAnts $graph 0]]
	set sgraph::afterId $id
    }
}

################################################################
# Set up to display the marching ants for the box outline
################################################################
proc ::sgraph::MarchingAnts { graph offset } {

    incr offset
    if { [$graph marker exists zoomOutline] } {
	$graph marker configure zoomOutline -dashoffset $offset 
	set interval $sgraph::interval
	set id [after $interval [list sgraph::MarchingAnts $graph $offset]]
	set sgraph::afterId $id
    }
}

################################################################
# Add an event to the graph
################################################################
proc ::sgraph::AddBindTag { graph name } {
    set oldtags [bindtags $graph]
    if { [lsearch $oldtags $name] < 0 } {
        bindtags $graph [concat $name $oldtags]
    }
}

################################################################
# Reset the display of the active mode to "Active Mode"
################################################################
proc ::sgraph::ResetActiveMode {} {

    set sgraph::active_mode($sgraph::curIndx) "   \"Active Mode\""

    sgraph::TurnOffActions
}

################################################################
# Turn off all actions and set the cursor to the crosshair
################################################################
proc ::sgraph::TurnOffActions {} {

    #
    # Turn off all events
    #
    bind bltZoomGraph <ButtonPress-1> {}
    bind bltZoomGraph <ButtonRelease-1> {}
    bind $sgraph::curGraph <Motion> {}
    bind bltZoomOutGraph <ButtonPress-1> {}

    $sgraph::curGraph configure -cursor crosshair
}

################################################################
# Display the full graph
################################################################
proc ::sgraph::ZoomAll { indx } {

    #
    # Check that the indx value is valid.
    #
    if { $indx < 0  || $indx >= [array size sgraph::grph] } {
	tk_messageBox \
		-message "Graph indx of $indx to sgraph::PrintTheGraph is out of range!"
	return
    }

    #
    # Check if have switched graphs -- if so, reset sgraph::active_mode
    # to let user know that no action is active on the graph just left.
    #
    if { $sgraph::curIndx != $indx } {
	sgraph::ResetActiveMode
    }

    #
    # Save the index to the currently active graph
    #
    set sgraph::curIndx $indx
    set sgraph::curGraph $sgraph::grph($indx)

    set sgraph::active_mode($sgraph::curIndx) \
	    "   \"Display full extents of graph\""

    #
    # Delete any part of the zoom outline
    #
    eval $sgraph::curGraph marker delete \
	    [$sgraph::curGraph marker names "zoom*"]

    #
    # Turn off all events
    #
    sgraph::TurnOffActions

    #
    # Display the full extents of the graph
    #
    foreach axis { x y } {
#	puts "Autoscale the $axis axis"
#	puts "$axis limits     : [$sgraph::curGraph axis limits $axis]"
#	puts "$axis min        : [$sgraph::curGraph axis cget $axis -min]"
#	puts "$axis max        : [$sgraph::curGraph axis cget $axis -max]"
#	puts "$axis stepsize   :  [$sgraph::curGraph axis cget $axis -stepsize]"
#	puts "$axis tickdivider:  [$sgraph::curGraph axis cget $axis -tickdivider]"
#	puts "$axis majorticks :  [$sgraph::curGraph axis cget $axis -majorticks]"
#	puts "$axis minorticks :  [$sgraph::curGraph axis cget $axis -minorticks]"
	if {  [catch { $sgraph::curGraph axis configure $axis -min {} -max {} } result] != 0 } {
	    puts " ===> configure of $axis axis failed"

	}
##	$sgraph::curGraph axis configure $axis -min {} -max {}
##	$sgraph::curGraph axis configure $axis -min {} -max {}

#    puts "Done with the ZoomAll ... returning!"
    }
}

################################################################
# Process the pan/zoom
################################################################
proc ::sgraph::ProcessZoom { graph } {

    #
    # Delete any part of the zoom outline
    #
    eval $graph marker delete [$graph marker names "zoom*"]
    if { [info exists sgraph::info($graph,afterId)] } {
	after cancel $sgraph::info($graph,afterId)
    }

    #
    # Get the corners of the zoom outline
    #
    set x1 $sgraph::info($graph,A,x)
    set y1 $sgraph::info($graph,A,y)
    set x2 $sgraph::info($graph,B,x)
    set y2 $sgraph::info($graph,B,y)

    if { ($x1 == $x2) || ($y1 == $y2) } { 
	# No change so return
	return
    }

    #
    # The busy command provides a simple means to block keyboard, 
    # button, and pointer events from Tk widgets, while overriding 
    # the widget's cursor with a configurable busy cursor
    #
    blt::busy hold $graph 
    # This update lets the busy cursor take effect.
    update

    #
    # Convert the (x,y) value to graph coordinates
    #
    set min [$graph axis invtransform x $x1]
    set max [$graph axis invtransform x $x2]
    if { $min == $max } {
	# 
	# Zoom window too small - use the previous zoom
	# coordinates
	#
	set sgraph::info($graph,A,x) $sgraph::oldxA
	set sgraph::info($graph,A,y) $sgraph::oldyA
	set sgraph::info($graph,B,x) $sgraph::oldxB
	set sgraph::info($graph,B,y) $sgraph::oldyB

	blt::busy release $graph
	return
    }


    #
    # Check if the x-min is smaller than the defined extents
    #
    if { $min < $sgraph::grph_xmin($sgraph::curIndx) } {
	set min $sgraph::grph_xmin($sgraph::curIndx)
	set sgraph::info($graph,A,x) \
		[$graph axis transform x $min]
    }
    #
    # Check if the x-max is larger than the defined extents
    #
    if { $max > $sgraph::grph_xmax($sgraph::curIndx) } {
	set max $sgraph::grph_xmax($sgraph::curIndx)
	set sgraph::info($graph,B,x) \
		[$graph axis transform x $max]
    }

    #
    # Configure the x axis with new values
    #
    if { $min > $max } { 
	$graph axis configure x -min $max -max $min
    } else {
	$graph axis configure x -min $min -max $max
    }

    #
    #  Convert the (x,y) value to graph coordinates
    #
    set max [$graph axis invtransform y $y1]
    set min [$graph axis invtransform y $y2]
    if { $min == $max } {
	# 
	# Zoom window too small - use the previous zoom
	# coordinates
	#
	set sgraph::info($graph,A,x) $sgraph::oldxA
	set sgraph::info($graph,A,y) $sgraph::oldyA
	set sgraph::info($graph,B,x) $sgraph::oldxB
	set sgraph::info($graph,B,y) $sgraph::oldyB

	blt::busy release $graph
	return
    }

    #
    # Check if the y-min is smaller than the defined extents
    #
    if { $min < $sgraph::grph_ymin($sgraph::curIndx) } {
	set min $sgraph::grph_ymin($sgraph::curIndx)
	set sgraph::info($graph,A,y) \
		[$graph axis transform y $min]
    }

    #
    # Check if the y-max is larger than the defined extents
    #
    if { $max > $sgraph::grph_ymax($sgraph::curIndx) } {
	set max $sgraph::grph_ymax($sgraph::curIndx)
	set sgraph::info($graph,B,y) \
		[$graph axis transform y $max]
    }

    #
    # Configure the y axis with new values
    #
    if { $min > $max } { 
	$graph axis configure y -min $max -max $min
    } else {
	$graph axis configure y -min $min -max $max
    }

    # This "update" forces the graph to be redrawn
    update
    
    blt::busy release $graph
    
    # 
    # Save the current zoom settings
    #
    set sgraph::oldxA $x1
    set sgraph::oldyA $y1
    set sgraph::oldxB $x2
    set sgraph::oldyB $y2
}


################################################################
# Get the coordinates
################################################################
proc ::sgraph::GetCoords { graph x y index } {

    #
    # Save the screen coordinates for the mouse pick
    #
    if { [$graph cget -invertxy] } {
        set sgraph::info($graph,$index,x) $y
        set sgraph::info($graph,$index,y) $x
    } else {
        set sgraph::info($graph,$index,x) $x
        set sgraph::info($graph,$index,y) $y
    }
}


################################################################
# Zoom in on the graph
################################################################
proc ::sgraph::ZoomIn {indx} {
    
    #
    # Check that the indx value is valid.
    #
    if { $indx < 0  || $indx >= [array size sgraph::grph] } {
	tk_messageBox -message "Graph indx of $indx to \
		sgraph::PrintTheGraph is out of range!"
	return
    }

    #
    # Check if have switched graphs -- if so, reset sgraph::active_mode
    # to let user know that no action is active on the graph just left.
    #
    if { $sgraph::curIndx != $indx } {
	sgraph::ResetActiveMode
    }

    #
    # Save the index to the currently active graph
    #
    set sgraph::curIndx $indx

    set sgraph::curGraph $sgraph::grph($indx)

    set sgraph::active_mode($sgraph::curIndx) \
	    "   \"Zoom In: MB1 drag MB1 to define rectangle\""
    update

    #
    # The first time this routine is called, save the extents of
    # the graph. These extents are used to stop the zooming out.
    #
    if { $sgraph::first($indx) } {
	#
	# Get the extents of the graph. A list of the minimum and maximum 
	# limits for the axis is returned. The order of the list is min max.
	#
	set strg [$sgraph::curGraph axis limits x]
	scan $strg {%f %f} xmin xmax
	set strg [$sgraph::curGraph axis limits y]
	scan $strg {%f %f} ymin ymax
	if { $xmax == 0 && $ymax == 0 } {
	    return
	}

	set wdth_adjx [expr { abs($xmax - $xmin) * 0.2 }]
	set hght_adjy [expr { abs($ymax - $ymin) * 0.2 }]

	set xmin [expr { $xmin - $wdth_adjx }]
	set ymin [expr { $ymin - $hght_adjy }]
	set xmax [expr { $xmax + $wdth_adjx }]
	set ymax [expr { $ymax + $hght_adjy }]

	set sgraph::grph_xmin($indx) $xmin
	set sgraph::grph_ymin($indx) $ymin
	set sgraph::grph_xmax($indx) $xmax
	set sgraph::grph_ymax($indx) $ymax

	set sgraph::first($indx) 0
    }
    sgraph::InitVars $sgraph::curGraph
    
    $sgraph::curGraph configure -cursor crosshair

    bind bltZoomOutGraph <ButtonRelease-1> {}

    bind bltZoomGraph <ButtonPress-1> { sgraph::SetZoomPoint %W %x %y 0 }
    bind bltZoomGraph <ButtonRelease-1> { sgraph::SetZoomPoint %W %x %y 0 }

    sgraph::AddBindTag $sgraph::curGraph bltZoomGraph 
}


################################################################
# Zoom out on the graph
################################################################
proc ::sgraph::ZoomOut { indx } {
    
    #
    # Check that the indx value is valid.
    #
    if { $indx < 0  || $indx >= [array size sgraph::grph] } {
	tk_messageBox -message "Graph indx of $indx to \
		sgraph::PrintTheGraph is out of range!"
	return
    }

    #
    # Check if have switched graphs -- if so, reset sgraph::active_mode
    # to let user know that no action is active on the graph just left.
    #
    if { $sgraph::curIndx != $indx } {
	sgraph::ResetActiveMode
    }

    #
    # Save the index to the currently active graph
    #
    set sgraph::curIndx $indx
    set sgraph::curGraph $sgraph::grph($indx)

    set sgraph::active_mode($sgraph::curIndx) "   \"Zoom Out: MB1 on new center-point\""

    #
    # Delete any part of the marching ants rectangle for defining
    # a new zoom window
    #
    eval $sgraph::curGraph marker delete \
	    [$sgraph::curGraph marker names "zoom*"]

    set sgraph::info($sgraph::curGraph,corner) "A"

    #
    # Turn off the actions for the zoom-in
    #
    bind bltZoomGraph <ButtonPress-1> {}
    bind bltZoomGraph <ButtonRelease-1> {}
    bind $sgraph::curGraph <Motion> {}

    $sgraph::curGraph configure -cursor center_ptr
    #
    # Set the actions for the zoom out
    #
    bind bltZoomOutGraph <ButtonRelease-1> { sgraph::SetZoomPoint %W %x %y 1 }
    sgraph::AddBindTag $sgraph::curGraph bltZoomOutGraph 
}


################################################################
# Initialize the vars
################################################################
proc ::sgraph::InitVars { graph } {
    set sgraph::interval 100
    set sgraph::afterId 0
    set sgraph::info($graph,A,x) {}
    set sgraph::info($graph,A,y) {}
    set sgraph::info($graph,B,x) {}
    set sgraph::info($graph,B,y) {}
    set sgraph::info($graph,stack) {}
    set sgraph::info($graph,corner) A
}

################################################################
# Calculate the new zoom window
################################################################
proc ::sgraph::CalculateZoom { graph scalex scaley } {

    #
    # Lower-left corner
    #
    set xmid $sgraph::info($graph,A,x)
    set ymid $sgraph::info($graph,A,y)

    #
    # Change to graph coordinates
    #
    set xmid [$graph axis invtransform x $xmid]
    set ymid [$graph axis invtransform y $ymid]

    #
    # Get the current zoom x-window coordinates and
    # calculate the width
    #
    set xmin [$graph axis invtransform x $sgraph::oldxA]
    set xmax [$graph axis invtransform x $sgraph::oldxB]
    set curwidth [expr { abs($xmax - $xmin) }]

    #
    # Get the current zoom y-window coordinates and
    # calculate the height
    #
    set ymax [$graph axis invtransform y $sgraph::oldyA]
    set ymin [$graph axis invtransform y $sgraph::oldyB]
    set curheight [expr { abs($ymax - $ymin) }]

    #
    # Calculate the size of the new zoom window.
    #
    set xhalf [expr { $curwidth * $scalex }]
    set yhalf [expr { $curheight * $scaley }]

    set mnx [expr { ($xmid - $xhalf) }]
    set mny [expr { ($ymid + $yhalf) }]
    set mxx [expr { ($xmid + $xhalf) }]
    set mxy [expr { ($ymid - $yhalf) }]

    #
    # Save a screen coordinates
    #
    set sgraph::info($graph,A,x) [$graph axis transform x $mnx]
    set sgraph::info($graph,A,y) [$graph axis transform y $mny]
    set sgraph::info($graph,B,x) [$graph axis transform x $mxx]
    set sgraph::info($graph,B,y)  [$graph axis transform y $mxy]

}

################################################################
################################################################
proc ::sgraph::SetZoomPoint { graph x y isZoomOut } {

    if { ![info exists sgraph::info($graph,corner)] } {
	sgraph::InitVars $graph
    }

    sgraph::GetCoords $graph $x $y $sgraph::info($graph,corner)

    #
    # Is this a zoom-out request?
    #
    if { $isZoomOut } {
	#
	# Calculate the new zoom coordinate for the graph
	#
	sgraph::CalculateZoom $graph 1.2 1.2

	#
	# Process the zoom request and redraw the graph
	#
	sgraph::ProcessZoom $graph 
	return
    }

    # 
    # Is this the first corner of the zoom-in request?
    #
    if { $sgraph::info($graph,corner) == "A" } {
	if { ![$graph inside $x $y] } {
	    return
	}

	#
	# First corner selected, start watching motion events
	#
	bind $graph <Motion> { 
	    sgraph::GetCoords %W %x %y B
	    sgraph::Box %W
	}
	set sgraph::info($graph,corner) B
    } else {
	#
	# The ButtonPress-1 and ButtonRelease-1 have been done.
	# Check if this is a zoom rectangle or a single point.
	# If single point, calculate the size of the zoom-in 
	# window.
	#
	set x1 $sgraph::info($graph,A,x)
	set y1 $sgraph::info($graph,A,y)
	set x2 $sgraph::info($graph,B,x)
	set y2 $sgraph::info($graph,B,y)
	if { ([expr { abs ($x1 - $x2) }] < 5) && \
	     ([expr { abs ($y1 - $y2) }] < 5) } {
	 sgraph::CalculateZoom $graph 0.4 0.4
        } 

	#
	# Delete the modal binding
	#
	bind $graph <Motion> { }

	#
	# Process the zoom request
	#
	sgraph::ProcessZoom $graph 

	#
	# Reset to the first corner of a zoom-in
	#
	set sgraph::info($graph,corner) A
    }
}

################################################################
# Save the graph to a PostScript file without popping up the
# file dialog.
################################################################
proc ::sgraph::Save { graph psfilename} {

    set filename $psfilename
    foreach { indx g } [array get sgraph::grph] {
	if { [string first $graph $g] > -1 } {

	    #
	    # If the .ps extention isn't part of the file name, add
	    #
	    if { [string first ".ps" $filename ] < 0 } {
		set sgraph::file_name [format {%s%s} $filename \
			".ps"]
	    } else {
		set sgraph::file_name $filename
	    }
	    
	    sgraph::CreatePS $indx 1 1 $sgraph::file_name
	
	}
    }
}

################################################################
# Save the graph to a Metafile file without popping up the
# file dialog.
################################################################
proc ::sgraph::SaveMetafile { graph mffilename} {

    set filename $mffilename
    foreach { indx g } [array get sgraph::grph] {
	if { [string first $graph $g] > -1 } {

	    #
	    # If the .emf extention isn't part of the file name, add
	    #
	    if { [string first ".emf" $filename ] < 0 } {
		set sgraph::file_name [format {%s%s} $filename \
			".emf"]
	    } else {
		set sgraph::file_name $filename
	    }
	    
	    sgraph::CreatePS $indx 1 0 $sgraph::file_name
	
	}
    }
}


################################################################
# Save the graph to a PostScript file or a metafile (Windows
# only)
################################################################
proc ::sgraph::SaveGraph { indx } {

    #
    # Check that the indx value is valid.
    #
    if { $indx < 0  || $indx >= [array size sgraph::grph] } {
	tk_messageBox \
		-message "Graph indx of $indx to sgraph::PrintTheGraph is out of range!"
	return
    }

    #
    # Check if have switched graphs -- if so, reset sgraph::active_mode
    # to let user know that no action is active on the graph just left.
    #
    if { $sgraph::curIndx != $indx } {
	sgraph::ResetActiveMode
    }

    #
    # Save the index to the currently active graph
    #
    set sgraph::curIndx $indx
    set sgraph::curGraph $sgraph::grph($indx)

    set sgraph::active_mode($sgraph::curIndx) "   \"Save the Graph\""

    #
    # Turn off the actions for the zoom-in
    #
    sgraph::TurnOffActions


    set sgraph::dlg [Dialog .dlg -parent . -modal local \
	    -title "Save Graph to File"]
    $sgraph::dlg add -name cancel -text Cancel \
	    -command sgraph::PopDownDlg
    $sgraph::dlg add -name saveps     -text "Save PostScript" \
	    -command [list sgraph::SaveToFile $sgraph::curIndx 1 PostScript]
    if { $::tcl_platform(platform) == "windows" } {
	$sgraph::dlg add -name savemeta    -text "Save Metafile" \
		-command [list sgraph::SaveToFile $sgraph::curIndx 0 Metafile]
    }
    $sgraph::dlg draw

}

################################################################
# Process the PostScript options
################################################################
proc ::sgraph::UpdatePsOptions { wdgt item } {
    
    set sgraph::$item [$wdgt cget -value]

}

proc ::sgraph::PopDownDlg {} {

    destroy $sgraph::dlg
    set sgraph::active_mode($sgraph::curIndx) "   \"Active Mode\""

}

################################################################
# Save the PostScript output of the graph to a file
################################################################
proc ::sgraph::SaveToFile { indx  saveps type } {
    set pstypelist {{{PostScript files} {.ps} }
        {{All Files}        *                   }
    }
    set metatypelist {{{Metafile files} {.emf} }
        {{All Files}        *                   }
    }

    if { $saveps } {
	set typelist $pstypelist
	set ext ".ps"
    } else {
	set typelist $metatypelist
	set ext ".emf"
    }

    #
    # If first time, set the default directory to the
    # current directory. Otherwise, you the last 
    # directory selected.
    #
    if { [string length $sgraph::file_name] < 1 } {
	set dir "."
    } else {
	set dir [file dirname $sgraph::file_name]
    }

    # 
    # Request a file name for the PostScript file
    #
    set rtrn [tk_getSaveFile -filetypes $typelist \
            -initialdir $dir -title "Save as $type File" \
	    -initialfile "out$ext"]

    if { $rtrn == "" } {
	set sgraph::save_file 0
	return 0
    }

    #
    # If the .ps extention isn't part of the file name, add
    #
    if { [string first $ext $rtrn ] < 0 } {
	set sgraph::file_name [format {%s%s} $rtrn \
		$ext]
    } else {
	set sgraph::file_name $rtrn
    }


    sgraph::CreatePS $indx 1 $saveps $sgraph::file_name
	
    sgraph::PopDownDlg
    return 1
}

################################################################
# Update the PostScript image. Save to a file as PostScript or
# Metafile and send to a printer.
################################################################
proc ::sgraph::CreatePS {indx savetofile {saveps 1} {filename "none"} } {

    #
    # Is it going to be color or grayscale?
    #
    if { [string equal $sgraph::colormode "grayscale"] } {
	set clrmode gray
    } else {
	set clrmode color
    }

    #
    # Configure the postscript options
    $sgraph::grph($indx) postscript configure \
	    -colormode $clrmode \
	    -landscape $sgraph::landscape \
	    -maxpect $sgraph::maxpect \
	    -decorations  $sgraph::decorations

    #
    # Save to a file?
    #
    if { $savetofile } {
	if { $saveps } {
	    #
	    # Save a postscript file
	    #
	    $sgraph::grph($indx) postscript output $filename
	} else {
	    #
	    # Save a metafile
	    #
	    $sgraph::grph($indx) snap -format emf $filename
	}
	puts stdout "wrote file \"$filename\"."
	#
	# Return since not sending to printer
	#
	return
    }

    if { $::tcl_platform(platform) == "windows" } {
	#
	# Draw directly to the print device - this may not
	# work on all printers but quality is much better than
	# using print1
	#
	$sgraph::grph($indx) print2 
    } else {
	#
	# Generate a PostScript file and send this to the selected
	# printer. The file that is written is "out.ps".
	#
	$sgraph::grph($indx) postscript output "out.ps"
	set cmd {lp -d$sgraph::current_printer out.ps}
	eval exec $cmd
    }

}

################################################################
# Generate the hardcopy of the graph and delete the print
# options dialog
################################################################
proc ::sgraph::CreatePSdestroyDlg {} {

    sgraph::CreatePS $sgraph::curIndx 0
    sgraph::PopDownDlg

}

################################################################
# Popup the print options dialog
################################################################
proc ::sgraph::PopupPrintOptions {} {

    #
    # Create the dialog
    #
    set sgraph::dlg [Dialog .dlg -parent . -modal local \
	    -title "Set Printer Information" ]
    
    $sgraph::dlg add -name cancel -text Cancel \
	    -command sgraph::PopDownDlg
    $sgraph::dlg add -name print  -text Print \
	    -command sgraph::CreatePSdestroyDlg
    
    if { $::tcl_platform(platform) != "windows" } {
	
	set tpfrme [$sgraph::dlg getframe]
	grid $tpfrme -sticky news
	
	#   ---------------------------------
	#   | Printer: |princess           |v|
	#   ---------------------------------
	#
	set ptrfrme [frame $tpfrme.prntfrme \
		-borderwidth 2]
	set ptrlbl [label $ptrfrme.lbl -text "Printer:"]
	
	set ptrlstbx [ComboBox $ptrfrme.lb \
		-textvariable sgraph::current_printer \
		-values $sgraph::printer_list \
		-font {courier 12 bold}]
	pack $ptrlbl $ptrlstbx -side left -expand yes -fill both
	grid $ptrfrme -sticky news
	
	#
	#    -PostScript Options---------------
	#    | landscape   | o yes | o no      | 
	#    | maxpect     | o yes | o no      | 
	#    | decorations | o yes | o no      | 
	#    | colormode   | o yes | o no      |
	#    ----------------------------------
	#
	set lblfrme [TitleFrame $tpfrme.leftf \
		-text "PostScript Options" ]
	set opfrme [$lblfrme getframe]
	
	foreach bool { landscape maxpect decorations } {
	    set wl $opfrme.$bool-label
	    label $wl -text "$bool" -font *courier*-r-*12* \
		    -anchor w
	    
	    set wy $opfrme.$bool-yes
	    radiobutton $wy -text "yes" -variable sgraph::$bool \
		    -value 1 -anchor w \
		    -command [list sgraph::UpdatePsOptions $wy $bool]
	    
	    set wn $opfrme.$bool-no
	    radiobutton $wn -text "no" -variable sgraph::$bool \
		    -value 0 -anchor w \
		    -command [list sgraph::UpdatePsOptions $wn $bool]
	    grid $wl $wy $wn -sticky news 
	}
	set wl [label $opfrme.modes -text "colormode" \
		-font *courier*-r-*12* -anchor w]
	
	foreach m { color grayscale } {
	    radiobutton $opfrme.$m -text $m -variable sgraph::colormode \
		 -value $m -anchor w \
		 -command [list sgraph::UpdatePsOptions $opfrme.$m colormode] 
	}
	grid $wl $opfrme.color $opfrme.grayscale -sticky news
	grid $lblfrme -pady 10 -sticky news
	
	grid columnconfigure $tpfrme 0 -weight 1
    }
    
    $sgraph::dlg draw
}

################################################################
# Generate hardcopy of the graph
################################################################
proc ::sgraph::PrintTheGraph { indx {do_popups 1} } {

    #
    # Check that the indx value is valid.
    #
    if { $indx < 0  || $indx >= [array size sgraph::grph] } {
	tk_messageBox \
		-message "Graph indx of $indx to sgraph::PrintTheGraph is out of range!"
	return
    }

    #
    # Check if have switched graphs -- if so, reset sgraph::active_mode
    # to let user know that no action is active on the graph just left.
    #
    if { $sgraph::curIndx != $indx } {
	sgraph::ResetActiveMode
    }

    #
    # Save the index to the currently active graph
    #
    set sgraph::curIndx $indx
    set sgraph::curGraph $sgraph::grph($indx)

    set sgraph::active_mode($sgraph::curIndx) "   \"Print the Graph\""

    sgraph::TurnOffActions

    # Check if user wants the popup to define the printer and
    # PostScript settings or to print with the defaults.
    if { $do_popups } {
	#
	# Create the dialog
	#
	sgraph::PopupPrintOptions

    } else {
	sgraph::CreatePS $indx 0
    }
}


################################################################
# If this is unix, get a list of available printers.
################################################################
proc ::sgraph::InitializePrinter {} {
    
    if { $::tcl_platform(platform) == "windows" } {
	return
    }

    #  Try 'lpstat -a' to get a printer list
    if { [catch {exec lpstat -a} result] == 0 } {

	#  if it worked, then parse printer names
	foreach line [split $result "\n"] {
	    lappend sgraph::printer_list [lindex $line 0]
	}

	#  get default printer from last word of 'lpstat -d'
	catch {exec lpstat -d} result
	set sgraph::default_printer [lindex [split $result " "] end]
	set sgraph::current_printer $sgraph::default_printer
    }
}


################################################################
# Exit the application
################################################################
proc ::sgraph::exit {} {
    exit
}

################################################################
#  Standalone Testing Code
################################################################
if { $::argv0 == [info script] } {


set auto_path [linsert $auto_path 0 /sppdg/software/tools/public_domain/lib]
set auto_path [linsert $auto_path 0 /sppdg/software/tools/public_domain/hpux11/lib]

    if { $::tcl_platform(platform) == "windows" } {
	console show
    } else {
	package require Console
	toplevel .console
	pack [console .console.t] -expand yes -fill both
	wm protocol .console WM_DELETE_WINDOW {sgraph::exit}
    } 


    #  Create some data for the graphs
    blt::vector create ::xan
    blt::vector create ::xa
    blt::vector create ::xb
    blt::vector create ::y1a
    blt::vector create ::y2a
    blt::vector create ::y1b
    blt::vector create ::y2b

    xan seq 0 3000
    xb seq 0 4000
    xa  expr { 0.1 * xan } 
    y1a expr { 0.4 * sin(3.14159 * xan * 0.004) }
    y2a expr { cos(3.14159 * xan * 0.005) }
    y1b expr { sin(3.14159 * xb * 0.003) }
    y2b expr { 2 * cos(3.14159 * xb * 0.006) }


    #  Create a notebook widget, then two graph windows
    set noteBk [blt::tabset .t -relief sunken -borderwidth 1]

    set ::grph1 [sgraph::sgraph $noteBk.graph1 -relief sunken -borderwidth 5 \
	    -title "Test 1"]
    set ::grph2 [sgraph::sgraph $noteBk.graph2]


    $noteBk insert end graph1 -text "Test 1" -window $::grph1 -fill both 
    $noteBk insert end graph2 -text "Test 2" -window $::grph2 -fill both



    #  Add data to graphs
    ##$::grph1 configure -title "Test 1"
    $::grph2 configure -title "Test 2"

    $::grph1 element create s1 -symbol ""  \
	    -linewidth 2 -color blue -xdata ::xa -ydata ::y1a

    $::grph1 element create s2 -symbol ""  \
	    -linewidth 2 -color green -xdata ::xa -ydata ::y2a 

    $::grph2 element create s1 -symbol ""  \
	    -linewidth 2 -color red -xdata ::xb -ydata ::y1b

    $::grph2 element create s2 -symbol ""  \
	    -linewidth 2 -color black -xdata ::xb -ydata ::y2b 


            # Setup the X-Axis data in GHz

    $::grph1 configure -relief sunken

        # Hide the Legend
        #$::grph1 legend configure -hide yes        
        
        # Show the Grid
        $::grph1 grid configure -hide no -color black       
        
        # Don't display a symbol for each data point
        $::grph1 element configure s1 -symbol "" -color red -label "1" 
	$::grph1 xaxis configure -title "Frequency in GHz" -titlefont {courier 16}
        $::grph1 element configure s2 -symbol "" -color blue -label "2"
          set units "Amplitude in Measurement Units(MU)"
        $::grph1 yaxis configure -title $units -titlefont {courier 16}      

	$::grph1 axis configure y -stepsize 60 -subdivisions 1
##            set startF [expr { $startFreq * double(1) } ] 
    set startF 0.0
##            set stopF [expr { $stopFreq * double(1) } ]
    set stopF 200.0
#    $::grph1 axis configure x -min $startF -max $stopFreq
    $::grph1 axis configure x -min $startF -max $stopF

    set stepSize [ expr { ($stopF - $startF) / 10 } ]

##            $::grph1 axis configure x -stepsize $stepSize -subdivisions 1
    $::grph1 axis configure x -stepsize $stepSize -subdivisions 1

	$::grph1 axis configure y -min -1 -max 1
#	$::grph1xs axis configure y -majorticks {60 120 180 240 300 360 420 480 540}
	$::grph1 marker create text -text "Marker" -coords { 100 100 } -name normalMarker
		
	set units "Amplitude in "
	$::grph1 yaxis configure -title $units -titlefont {courier 16}
	 
        Blt_ZoomStack $::grph1


    pack $noteBk -side top -expand yes -fill both


##    sgraph::Print $::grph1 0
##    sgraph::Print $::grph2 1
}








