#----------------------------------------------------------------
#
#  gui_elements.tcl
#
#  Callback functions for TNT GUI operation.
#   ::gui::_selectNode
#   ::gui::_fillGlobalValues
#   ::gui::_highlightSelected
#   ::gui::_drpendComp
#   ::gui::_nameChangedWarning
#   ::gui::_modify
#   ::gui::_delete
#   ::gui::_deleteAll
#   ::gui::_addGroundPlane
#   ::gui::_chckElementGrnd
#   ::gui::_chckElementDiel
#   ::gui::_chckElementCond
#   ::gui::_buildTree
#   ::gui::_addRectDielectric
#   ::gui::_addDielectric
#   ::gui::_insertInTree
#   ::gui::_addConductor
#   ::gui::_configureColor
#   ::gui::_readMaterialList
#   ::gui::guiSetupPrinter
#   ::gui::_finishPrinter
#   ::gui::_putDielectricLayerToFile
#   ::gui::_putRectangleDielectricToFile
#   ::gui::_putRectangleConductorsToFile
#   ::gui::_putCircleConductorsToFile
#   ::gui::_putTrapezoidConductorsToFile
#   ::gui::_putGroundPlaneToFile
#   ::gui::guiReadNewPermittivityList
#   ::gui::guiReadNewLossTangentList
#   ::gui::guiReadNewConductivityList
#   ::gui::_logDesignAttributes
#   ::gui::_setDefaultUnits
#   ::gui::_getWidthHeight
#
#
#  Sharon Zahn
#  October 9, 2002
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved
#  $Id: gui_elements.tcl,v 1.16 2004/07/20 14:51:55 techenti Exp $
#
#----------------------------------------------------------------

package require Itcl

package provide gui 2.0

#------------------------------------------------------
#  New/Update/Rename Structure Procedures
#
#  When creating a new cross section structure,
#  set all dialog fields to defaults, open the
#  dialog, check the results, create the new element.
#------------------------------------------------------

proc ::gui::newStructure {type} {

    #------------------------------------------------------
    # Create new object, in global namespace
    #  A simple "::#auto" would do the trick, but we'll create
    #  an abbreviated name, based on the class type, but
    #  with maximum of four letters per word
    #  (e.g. RectangleConductors -> RectCond)
    #------------------------------------------------------
    foreach word [regexp -inline -all {[A-Z][^A-Z]*} $type] {
	append name [string range $word 0 3]
    }
    set i 1
    while {[info command ::$name$i] != ""} {incr i}
    set objname $name$i
    set obj [$type ::$objname]

    #------------------------------------------------------
    # Create dialog, if necessary
    #------------------------------------------------------
    set dlg $::gui::dialog($type,dialog)
    if { ! [winfo exists $dlg] } {
	set dlg [::gui::_createStructureDialog $type]
    }

    #------------------------------------------------------
    #  Set dialog field values from object public variables
    #------------------------------------------------------
    set ::gui::dialog($type,name) $objname
    foreach var [$obj info variable] {
	if { [$obj info variable $var -protection] == "public" } {
	    lappend attributes [namespace tail $var]
	}
    }
    foreach var $attributes {
	set ::gui::dialog($type,$var) [$obj cget -$var]
    }

    #------------------------------------------------------
    #  Configure the dialog for "new", draw it, and
    #  interpret the result
    #------------------------------------------------------
    $dlg itemconfigure 0 -state normal     ;# add
    $dlg itemconfigure 1 -state disabled   ;# modify
    $dlg itemconfigure 2 -state disabled   ;# delete
    $dlg configure -default 0
    set result [$dlg draw]
    switch -- $result {
	-1 -
	3 {
	    #---------------------------
	    #  Cancel
	    #---------------------------
	    itcl::delete object $obj
	    return
	}

	0 {
	    #---------------------------
	    #  Add new structure
	    #---------------------------
	    #  Insert the structure name into the GUI tree
	    #  (We don't have to add to the canvas, as the object
	    #  automatically inserts itself into ::Stackup.)
	    $::gui::_tree insert 0 root $objname -text $objname
	    #  Copy dialog values to object
	    foreach var $attributes {
		$objname configure -$var $::gui::dialog($type,$var)
	    }
	    # Check the name, in case they tried
	    # to rename the object.
	    set newname $::gui::dialog($type,name)
	    if { $newname != $objname } {
		::gui::renameStructure $objname $newname
	    }
	}
    }

    #---------------------------------------
    # Redraw/rezoom the canvas
    #---------------------------------------
    ::gui::_canvas_redraw
    ::gui::_canvas_zoom_fit $::gui::_canvas
}


#----------------------------------------------------------------
#
#  ::gui::updateStructure
#
#  Open the structure dialog to allow the user to
#  change parameters of the specified cross section object.
#
#  This function is usually called by a double-click on
#  the GUI.
#
# Arguments:
#    objname   Name of cross section object
#
# Results:
#  This procedure will create (if necessary) a structure
#  entry dialog and set the entry values from the object.
#  It will draw the dialog and wait for the user to close
#  it.  The object will be updated, or it may be deleted.
#  Then the canvas is redrawn.
#
#----------------------------------------------------------------
proc ::gui::updateStructure {objname} {

    #------------------------------------------------------
    # fine object and type
    #------------------------------------------------------
    set obj [namespace which -command $objname]
    if { $obj eq "" } {
	return
    }
    set type [namespace tail [$obj info class]]

    #------------------------------------------------------
    # Make sure the dialog exists
    #------------------------------------------------------
    set dlg $::gui::dialog($type,dialog)
    if { ! [winfo exists $dlg] } {
	set dlg [::gui::_createStructureDialog $type]
    }

    #------------------------------------------------------
    #  Set dialog field values from object public variables
    #------------------------------------------------------
    set ::gui::dialog($type,name) $objname
    foreach var [$obj info variable] {
	if { [$obj info variable $var -protection] == "public" } {
	    lappend attributes [namespace tail $var]
	}
    }
    foreach var $attributes {
	set ::gui::dialog($type,$var) [$obj cget -$var]
    }

    #------------------------------------------------------
    #  Configure the dialog for "update", draw it, and
    #  interpret the result
    #------------------------------------------------------
    $dlg itemconfigure 0 -state disabled   ;# add
    $dlg itemconfigure 1 -state normal     ;# modify
    $dlg itemconfigure 2 -state normal     ;# delete
    $dlg configure -default 1
    set result [$dlg draw]
    switch -- $result {
	-1 -
	3 {
	    #---------------------------
	    #  Cancel
	    #---------------------------
	    return
	}

	1 {
	    #---------------------------
	    #  Modify structure object
	    #---------------------------
	    #  Copy dialog values to object
	    foreach var $attributes {
		$obj configure -$var $::gui::dialog($type,$var)
	    }
	    # Check the name, in case they tried
	    # to rename the object.
	    set newname $::gui::dialog($type,name)
	    if { $newname != $objname } {
		::gui::renameStructure $objname $newname
	    }
	}

	2 {
	    #---------------------------
	    #  Delete from stackup
	    #---------------------------
	    #  Remove from the GUI tree
	    $::gui::_tree delete $objname
	    #  delete object
	    itcl::delete object $obj
	}
    }

    #---------------------------------------
    # Redraw the canvas
    #---------------------------------------
    ::gui::_canvas_redraw
}

proc ::gui::renameStructure {oldname newname} {

    #------------------------------------------------------
    #  Don't overwrite an existing command name.
    #
    #  It would be nice if we could restrict only
    #  to renaming over existing object names,
    #  but we have to impose stricter limits.
    #  If we allow the user to rename a structure
    #  to something like "if", then most of this
    #  program won't run. :-(
    #
    #  Some adventurous day, we may put all the
    #  model objects in a separate namespace, so
    #  there will be less restrictions.
    #------------------------------------------------------
    if { [info command ::$newname] != "" } {
	tk_messageBox -title "Cannot rename" -icon warning -type ok \
	    -message "Cannot rename '$oldname' to '$newname' because it
conflicts with an existing object or Tcl command."
	return
    }

    #  otherwise, its OK.  Rename the object.
    set oldobj [namespace which -command $oldname]
    set newobj [namespace qualifiers $oldobj]::$newname
    rename $oldobj $newobj

    #  Update the layer stackup
    set i [lsearch $::Stackup::structureList $oldobj]
    set ::Stackup::structureList \
	[lreplace $::Stackup::structureList $i $i $newobj]

    #  Update the GUI
    set i [$::gui::_tree index $oldname]
    $::gui::_tree delete $oldname
    $::gui::_tree insert $i root $newname -text $newname
}



#------------------------------------------------------
#
#  ::gui::highlightItem
#
#  Apply highlighting to the specified cross section
#  object in both the canvas and the tree.
#
#------------------------------------------------------
proc ::gui::highlightItem { objname } {

    #  Select the object on the canvas
    ::gui::_canvasDraw_visitor setTagToSelect $objname
    ::gui::_canvas_redraw

    #  Set or clear the tree selection
    if { $objname eq "" } {
	$::gui::_tree selection clear
    } else {
	#  Set tree selection
	$::gui::_tree selection set $objname
    }
}


##################################################################
# Select a component from the tree structure.
##################################################################
proc ::gui::_selectNode { node } {

    $::gui::_tree selection clear
    $::gui::_tree selection set $node
    update

    set ::gui::_currentNode $node
    set objnme [$::gui::_tree itemcget $node -text]

    #  Select item in the canvas
    ::gui::_canvasDraw_visitor setTagToSelect $objnme
    ::gui::_canvas_redraw

}


##################################################################
# Fill the appropriate global values from selected component.
##################################################################
proc ::gui::_fillGlobalValues {objnme} {


    #----------------------------------
    # Is this object a groundplane?
    #----------------------------------
    if { [$objnme isa GroundPlane] } {
	set ::gui::_gname         $objnme
	#	    set ::gui::_gthickness     [$objnme cget -thickness]
	return
    }

    
    #----------------------------------
    # Is this object a dielectric?
    #----------------------------------
    if { [$objnme isa DielectricLayer] } {
	set ::gui::_dname         $objnme
	set ::gui::_thickness     [$objnme cget -thickness]
	set ::gui::_permittivity  [$objnme cget -permittivity]
	set ::gui::_permeability  [$objnme cget -permeability]
	set ::gui::_lossTangent   [$objnme cget -lossTangent]
	return
    }

    #----------------------------------
    # Is this object a rectangle-dielectric?
    #----------------------------------
    if { [$objnme isa RectangleDielectric] } {
	set ::gui::_dRname         $objnme
	set ::gui::_dheight       [$objnme cget -height]
	set ::gui::_dwidth        [$objnme cget -width]
	set ::gui::_dpermittivity  [$objnme cget -permittivity]
	set ::gui::_dpermeability  [$objnme cget -permeability]
	set ::gui::_dlossTangent   [$objnme cget -lossTangent]
	set ::gui::_dnumber       [$objnme cget -number]
	set ::gui::_dpitch        [$objnme cget -pitch]
	set ::gui::_dxoffset      [$objnme cget -xOffset]
	set ::gui::_dyoffset      [$objnme cget -yOffset]
	return
    }

    #----------------------------------
    # Is this object a rectangle-conductor?
    #----------------------------------
    if { [$objnme isa RectangleConductors] } {
	set ::gui::_rname         $objnme
	set ::gui::_rheight       [$objnme cget -height]
	set ::gui::_rwidth        [$objnme cget -width]
	set ::gui::_rconductivity [$objnme cget -conductivity]
	set ::gui::_rnumber       [$objnme cget -number]
	set ::gui::_rpitch        [$objnme cget -pitch]
	set ::gui::_rxoffset      [$objnme cget -xOffset]
	set ::gui::_ryoffset      [$objnme cget -yOffset]
	return
    }

    #----------------------------------
    # Is this object a circle-conductor?
    #----------------------------------
    if { [$objnme isa CircleConductors] } {
	set ::gui::_cname         $objnme
	set ::gui::_cdiameter     [$objnme cget -diameter]
	set ::gui::_cconductivity [$objnme cget -conductivity]
	set ::gui::_cnumber       [$objnme cget -number]
	set ::gui::_cpitch        [$objnme cget -pitch]
	set ::gui::_cxoffset      [$objnme cget -xOffset]
	set ::gui::_cyoffset      [$objnme cget -yOffset]
	return
    }

    #----------------------------------
    # Is this object a trapezoid-conductor?
    #----------------------------------
    if { [$objnme isa TrapezoidConductors] } {
	set ::gui::_tname         $objnme
	set ::gui::_theight       [$objnme cget -height]
	set ::gui::_ttopwidth     [$objnme cget -topWidth]
	set ::gui::_tbotwidth     [$objnme cget -bottomWidth]
	set ::gui::_tconductivity [$objnme cget -conductivity]
	set ::gui::_tnumber       [$objnme cget -number]
	set ::gui::_tpitch        [$objnme cget -pitch]
	set ::gui::_txoffset      [$objnme cget -xOffset]
	set ::gui::_tyoffset      [$objnme cget -yOffset]
	return
    }
}



##################################################################
# Highlight the selected item in the graphic window.
##################################################################
proc ::gui::_highlightSelected { node objnme } {

    $::gui::_tree selection set $node
    update

    set ::gui::_currentNode $node

    #    set objnme [$tree itemcget $node -text]

    set objnme [format {::%s} $objnme]
    #------------------------------------------------
    # Highlight the appropriate entry form for this item.
    #------------------------------------------------
    set ::gui::selectedObject $objnme

    #------------------------------------------------
    # Put the currently defined values into the 
    # appropriate form.
    #------------------------------------------------
    #----------------------------------
    # Get rid of the leading ::
    #----------------------------------
    set lgt [string length $objnme]
    set nme1 [string range $objnme 2 $lgt]
    
    ::gui::_fillGlobalValues $nme1

}

##################################################################
# Finish moving the selected component in the tree.
##################################################################
proc ::gui::_drpendComp { plistbox psource listdrp \
	opertr type datav } {

    set itemToMovePos [$::gui::_tree index $datav]

    set pos [lindex $listdrp 2]

    if { $pos > $itemToMovePos } {
	set mvto [expr {$pos - 1}]
    } else {
	set mvto $pos
    }

    if { $mvto < 1 } {
	set mvto 0
    }

    $::gui::_tree move root $datav $mvto

    set lst $::Stackup::structureList
    set lgt [expr {[llength $lst] - 1}]
    
    set indx1 [expr {$lgt - $itemToMovePos}]
    set it1 [lindex $lst $indx1]

    set indx2 [expr {$lgt - $pos}]

    set cnt 0
    set newlist ""

    if { $indx2 < 0 } {
	lappend newlist $it1
    }

    foreach snme $::Stackup::structureList {
	if { $cnt == $indx2 } {
	    if { [string compare $snme $it1] != 0 } {
		lappend newlist $snme
	    }
	    if { $cnt > -1 } {
		lappend newlist $it1
	    }
	} else {
	    if { [string compare  $snme $it1] != 0 } {
		lappend newlist $snme
	    }
	}
	incr cnt
    }
	
    set ::Stackup::structureList $newlist

    #---------------------------------------
    # Redraw the canvas.
    #---------------------------------------
    ::gui::_canvas_redraw
}

##########################################################
# Warn the user that this cannot be done.
##########################################################
proc ::gui::_nameChangedWarning { name newname } {

    set strg "Name: $name cannot be changed!  $newname is ignored." 
    ::gui::guiPopupWarning $strg
}

##################################################################
# Set the selected component values to the global values.
##################################################################
proc ::gui::_modify {} {

    if { [string length [$::gui::_tree selection get]] < 2 } {

	::gui::guiPopupWarning "Select the object you wish to modify."
	return
    }

    set elmnt [$::gui::_tree itemcget $::gui::_currentNode -text]

    if { [$elmnt isa GroundPlane] } {
	if { [string compare $elmnt $::gui::_gname] != 0 } {
	    ::gui::_nameChangedWarning $elmnt $::gui::_gname
	}

#	$elmnt configure -thickness $::gui::_gthickness

	#---------------------------------------
	# Redraw the canvas.
	#---------------------------------------
	::gui::_canvas_redraw
    }

    if { [$elmnt isa DielectricLayer] } {
	if { [string compare $elmnt $::gui::_dname] != 0 } {
	    ::gui::_nameChangedWarning $elmnt $::gui::_dname
	}

	$elmnt configure -thickness $::gui::_thickness
	$elmnt configure -permittivity $::gui::_permittivity
	$elmnt configure -permeability $::gui::_permeability
	$elmnt configure -lossTangent  $::gui::_lossTangent
	
	#---------------------------------------
	# Redraw the canvas.
	#---------------------------------------
	::gui::_canvas_redraw

	return
    }

    if { [$elmnt isa RectangleDielectric] } {
	if { [string compare $elmnt $::gui::_dname] != 0 } {
	    ::gui::_nameChangedWarning $elmnt $::gui::_dRname
	}

	$elmnt configure -permittivity $::gui::_dpermittivity
	$elmnt configure -permeability $::gui::_dpermeability
	$elmnt configure -lossTangent  $::gui::_dlossTangent
	$elmnt configure -height       $::gui::_dheight
	$elmnt configure -width        $::gui::_dwidth
	$elmnt configure -number       $::gui::_rnumber
	$elmnt configure -pitch        $::gui::_dpitch
	$elmnt configure -xOffset      $::gui::_dxoffset
	$elmnt configure -yOffset      $::gui::_dyoffset
	
	#---------------------------------------
	# Redraw the canvas.
	#---------------------------------------
	::gui::_canvas_redraw

	return
    }

    if { [$elmnt isa RectangleConductors] } {
	if { [string compare $elmnt $::gui::_rname] != 0 } {
	    ::gui::_nameChangedWarning $elmnt $::gui::_rname
	}
	$elmnt configure -height       $::gui::_rheight
	$elmnt configure -width        $::gui::_rwidth
	$elmnt configure -conductivity $::gui::_rconductivity
	$elmnt configure -number       $::gui::_rnumber
	$elmnt configure -pitch        $::gui::_rpitch
	$elmnt configure -xOffset            $::gui::_rxoffset
	$elmnt configure -yOffset            $::gui::_ryoffset

	#---------------------------------------
	# Redraw the canvas.
	#---------------------------------------
	::gui::_canvas_redraw

	return
    }

    if { [$elmnt isa CircleConductors] } {
	if { [string compare $elmnt $::gui::_cname] != 0 } {
	    ::gui::_nameChangedWarning $elmnt $::gui::_cname
	}

	$elmnt configure -diameter     $::gui::_cdiameter
	$elmnt configure -conductivity $::gui::_cconductivity
	$elmnt configure -number       $::gui::_cnumber
	$elmnt configure -pitch        $::gui::_cpitch
	$elmnt configure -xOffset            $::gui::_cxoffset
	$elmnt configure -yOffset            $::gui::_cyoffset

	#---------------------------------------
	# Redraw the canvas.
	#---------------------------------------
	::gui::_canvas_redraw
	return
    }


    if { [$elmnt isa TrapezoidConductors] } {
	if { [string compare $elmnt $::gui::_tname] != 0 } {
	    ::gui::_nameChangedWarning $elmnt $::gui::_tname
	}

	$elmnt configure -height       $::gui::_theight
	$elmnt configure -topWidth     $::gui::_ttopwidth
	$elmnt configure -bottomWidth  $::gui::_tbotwidth
	$elmnt configure -conductivity $::gui::_tconductivity
	$elmnt configure -number       $::gui::_tnumber
	$elmnt configure -pitch        $::gui::_tpitch
	$elmnt configure -xOffset            $::gui::_txoffset
	$elmnt configure -yOffset            $::gui::_tyoffset

	#---------------------------------------
	# Redraw the canvas.
	#---------------------------------------
	::gui::_canvas_redraw

	return
    }

}


##########################################################
# Delete all the cross section structures
##########################################################
proc ::gui::_deleteAll {} {

    #---------------------------------------
    #  Delete everything from the layer stackup,
    #  the tree, and canvas.
    #---------------------------------------
    foreach obj $::Stackup::structureList {
	catch {itcl::delete object $obj}
    }
    set ::Stackup::structureList [list]
    $::gui::_tree delete [$::gui::_tree nodes root]
    $::gui::_canvas delete all

    #---------------------------------------
    #  set global variables to a sane state
    #---------------------------------------
    set ::gui::_currentNode ""
    set ::gui::_tagSelected ""
    set ::gui::_title ""
    set ::gui::_num_c_segs 10
    set ::gui::_num_p_segs 10
}

##########################################################
# Add a groundplane.
##########################################################
proc ::gui::_addGroundPlane {} {

    if { [string length $::gui::_gname] < 1 } {
	incr ::gui::_grndCount
	set nme [format {::G%s} G$::gui::_grndCount]
    } else {
	set nme [format {::%s} $::gui::_gname]
    }

    set result [lsearch -exact \
            [itcl::find objects -class GroundPlane] $nme]
    if { $result > -1 } {
        set msgdlg [MessageDlg .msg \
                -message "$nme exists. Use a different name." \
                -title Warning -type ok]
        return
    }
    #---------------------------------------
    # Insert the ground-plane item into the tree.
    #---------------------------------------
    ::gui::_insertInTree $nme $nme

    #---------------------------------------
    # Create the ground-plane object.
    #---------------------------------------
    GroundPlane $nme

    #---------------------------------------
    # Increment the ground-plane count (to 
    # create unique names).
    #---------------------------------------
    incr ::gui::_grndCount

    #---------------------------------------
    # Redraw the canvas.
    #---------------------------------------
    ::gui::_canvas_redraw
}

##########################################################
# Check the ground, increment the ground count, and
#   insert into the tree.
##########################################################
proc ::gui::_chckElementGrnd { obj lstg } {

    foreach item $lstg {
	if { [string compare $obj $item] == 0 } {
	    # Increment the dielectric element count.
	    incr ::gui::_grndCount
	    
	    #---------------------------------------
	    # Insert the dielectric item into the tree.
	    #---------------------------------------
	    ::gui::_insertInTree $item
	    break
	}
    }
}

##########################################################
# Check the dielectric, increment the dielectric count, and
#   insert into the tree.
##########################################################
proc ::gui::_chckElementDiel { obj lstd } {

    foreach item $lstd {
	if { [string compare $obj $item] == 0 } {
	    
	    # Increment the dielectric element count.
	    incr ::gui::_dielCount
	    
	    #---------------------------------------
	    # Insert the dielectric item into the tree.
	    #---------------------------------------
	    ::gui::_insertInTree $item $item
	    break
	}
    }
}

##########################################################
# Check what kind of conductor this is, count according to
#   the type (circle, rectangle, or trapezoid), and insert
#   into the tree.
##########################################################
proc ::gui::_chckElementCond { obj lstr lstc lstt } {
	
    foreach item $lstr {
	if { [string compare $obj $item] == 0 } {
	    
	    # Increment the dielectric element count.
	    incr ::gui::_condCount
	    
	    #---------------------------------------
	    # Insert the dielectric item into the tree.
	    #---------------------------------------
	    ::gui::_insertInTree $item $item
	    break
	}
    }

    foreach item $lstc {
	if { [string compare $obj $item] == 0 } {
	    
	    # Increment the dielectric element count.
	    incr ::gui::_condCount
	    
	    #---------------------------------------
	    # Insert the dielectric item into the tree.
	    #---------------------------------------
	    ::gui::_insertInTree $item $item
	    break
	}
    }

    foreach item $lstt {
	if { [string compare $obj $item] == 0 } {
	    
	    # Increment the dielectric element count.
	    incr ::gui::_condCount

	    #---------------------------------------
	    # Insert the dielectric item into the tree.
	    #---------------------------------------
	    ::gui::_insertInTree $item $item
	    break
	}
    }

}


##########################################################
# Add a dielectric.
##########################################################
proc ::gui::_addRectDielectric {} {
    

    if { [string length $::gui::_dRname] < 1 } {
	incr ::gui::_dielCount
	set nme [format {::Diel%d} $::gui::_dielCount]
    } else {
	set nme [format {::%s} $::gui::_dRname]
    }
    
    # Increment the dielectric element count.
    #incr ::gui::_dielCount
 
    #---------------------------------------
    # Insert the dielectric item into the tree.
    #---------------------------------------
    ::gui::_insertInTree $nme $nme

    set result [lsearch -exact \
	    [itcl::find objects -class RectangleDielectric] $nme]
    if { $result > -1 } {
	set msgdlg [MessageDlg .msg \
		-message "$nme exists. Use a different name." \
		-title Warning -type ok]
	return
    }

    set shape [RectangleDielectric $nme \
	    -width $::gui::_dwidth \
	    -height $::gui::_dheight \
	    -permittivity $::gui::_dpermittivity \
	    -permeability $::gui::_dpermeability \
	    -lossTangent $::gui::_dlossTangent \
	    -number $::gui::_dnumber \
	    -pitch $::gui::_dpitch \
	    -xOffset $::gui::_dxoffset \
	    -yOffset $::gui::_dyoffset]

    #---------------------------------------
    # Redraw the canvas.
    #---------------------------------------
    ::gui::_canvas_redraw
}

##########################################################
# Add a dielectric.
##########################################################
proc ::gui::_addDielectric {} {
    
    if { [string length $::gui::_dname] < 1 } {
	incr ::gui::_dielCount
	set nme [format {::D%d} $::gui::_dielCount]
    } else {
	set nme [format {::%s} $::gui::_dname]
    }

    set result [lsearch -exact \
            [itcl::find objects -class DielectricLayer] $nme]
    if { $result > -1 } {
        set msgdlg [MessageDlg .msg \
                -message "$nme exists. Use a different name." \
                -title Warning -type ok]
        return
    }

    # Increment the dielectric element count.
    #incr ::gui::_dielCount

    #---------------------------------------
    # Insert the dielectric item into the tree.
    #---------------------------------------
    ::gui::_insertInTree $nme $nme

    #--------------------------------------
    # Create the dielectric object.
    #---------------------------------------
    DielectricLayer $nme -permittivity $::gui::_permittivity \
			 -permeability $::gui::_permeability \
			 -lossTangent $::gui::_lossTangent \
			 -thickness $::gui::_thickness
    #---------------------------------------
    # Redraw the canvas.
    #---------------------------------------
    ::gui::_canvas_redraw

}

##########################################################
# Insert the new item (a groundplane, dielectric, or a
#   conductor) into the design tree.
##########################################################
proc ::gui::_insertInTree { objname } { 
    $::gui::_tree insert 0 root $objname -text $objname
}


##########################################################
# Add a conductor.  
#
#   val:  rectangle, circle or trapezoid
##########################################################
proc ::gui::_addConductor {val} {
    
    if { [string compare $val "rectangle"] == 0} {

	if { [string length $::gui::_rname] < 1 } {
	    incr ::gui::_condCount
	    set nme [format {::Rect%d} $::gui::_condCount]
	} else {
	    set nme [format {::%s} $::gui::_rname]
	}

        set result [lsearch -exact \
                [itcl::find objects -class RectangleConductors] $nme]
        if { $result > -1 } {
            set msgdlg [MessageDlg .msg \
                    -message "$nme exists. Use a different name." \
                    -title Warning -type ok]
            return
	}
	set shape [RectangleConductors $nme -width $::gui::_rwidth\
	   -height $::gui::_rheight -conductivity $::gui::_rconductivity \
	   -number $::gui::_rnumber -pitch $::gui::_rpitch -xOffset $::gui::_rxoffset \
	   -yOffset $::gui::_ryoffset]
    } 

    if { [string compare $val "circle"] == 0} {

	if { [string length $::gui::_cname] < 1 } {
	    incr ::gui::_condCount
	    set nme [format {::Circ%d} $::gui::_condCount]
	} else {
	    set nme [format {::%s} $::gui::_cname]
	}

        set result [lsearch -exact \
                [itcl::find objects -class CircleConductors] $nme]
        if { $result > -1 } {
            set msgdlg [MessageDlg .msg \
                    -message "$nme exists. Use a different name." \
                    -title Warning -type ok]
            return
        }

	set shape [CircleConductors $nme \
		-diameter $::gui::_cdiameter \
		-conductivity $::gui::_cconductivity \
		-number $::gui::_cnumber -pitch $::gui::_cpitch \
		-xOffset $::gui::_cxoffset -yOffset $::gui::_cyoffset]
    } 

    if { [string compare $val "trapezoid"] == 0} {

	if { [string length $::gui::_tname] < 1 } {
	    incr ::gui::_condCount
	    set nme [format {::Trap%d} $::gui::_condCount]
	} else {
	    set nme [format {::%s} $::gui::_tname]
	}


        set result [lsearch -exact \
                [itcl::find objects -class TrapezoidConductors] $nme]
        if { $result > -1 } {
	    set msgdlg [MessageDlg .msg \
		    -message "$nme exists. Use a different name." \
		    -title Warning -type ok]
	    return
	}

	set shape [TrapezoidConductors $nme \
		-topWidth $::gui::_ttopwidth \
		-bottomWidth $::gui::_tbotwidth -height $::gui::_theight \
		-conductivity $::gui::_tconductivity \
		-number $::gui::_tnumber -pitch $::gui::_tpitch \
		-xOffset $::gui::_txoffset -yOffset $::gui::_tyoffset]
    } 


    #---------------------------------------
    # Insert the conductor item into the tree.
    #---------------------------------------
    ::gui::_insertInTree $shape $shape

    incr ::gui::_condCount
    
    #---------------------------------------
    # Redraw the canvas.
    #---------------------------------------
    ::gui::_canvas_redraw

}



##########################################################
# Read in the new material attribute list.
# 
#   title      : title for the pop-up window
#   arrList    : list of name/value attributes from the file.
#   flg        : true - file name being passed in
#                false - no file name so pop-up a file-selector
#   defaultFile: name of the file to open 
##########################################################
proc ::gui::_readMaterialList { title arrList flg defaultFile } {

    if { ! $flg } {
	set name $defaultFile
    } else {
	set typelist {
	    {{mmtl files}   {.list}     }
	    {{All Files}        *             }
	}

	set name [ tk_getOpenFile  -filetypes $typelist \
		-title $title ]
	#  Evade Windows double-click bug
	if { $::tcl_platform(platform) eq "windows" } {
	    ::gui::drainEventQueue
	}

	if { [string length $name] < 1 } {
	    return
	}
    }

    if { [array exists $arrList]  } {
	unset $arrList
    }

    puts "Reading $name"

    set fp [open $name r]
    
    while { [gets $fp lne] > -1 } {
	set cname [lindex $lne 0]
	set cvalue [lindex $lne 1]
	set cmd [format "set %s(%s) %s" $arrList $cname $cvalue]
	eval $cmd
    }
    
    close $fp
}



##########################################################
# Set up a printer.
##########################################################
proc ::gui::guiSetupPrinter {} {

    set ::gui::_savePrinterName $::gui::_printerName
    set ::gui::_saveColorMode $::gui::_colorMode

    if {! [winfo exists .prntFrame]} {

	set ::gui::_prntFrame [frame .prntFrame \
		-relief sunken -borderwidth 5]
	set seglbl [Label $::gui::_prntFrame.seglbl \
		-justify center -text "Printer Options"]
	grid $seglbl -columnspan 2 -sticky news -padx 4 -pady 4
	
	guiBuildLabelEntry $::gui::_prntFrame ::gui::_printerName "Printer:" 

	set lb [Label $::gui::_prntFrame.lb -label "Color Mode:"]
	set sb [SpinBox $::gui::_prntFrame.sb \
	    -textvariable ::gui::_colorMode \
	    -values { color grey mono } ]
	grid $lb $sb -sticky news -padx 4 -pady 4
	
	set but1 [::gui::guiBuildButton $::gui::_prntFrame "OK" \
		"::gui::_finishPrinter 1" "Print"]
	set but2 [::gui::guiBuildButton $::gui::_prntFrame "Cancel" \
		"::gui::_finishPrinter 0" "Cancel the Print"]
	grid $but1 $but2 -sticky nw
    }
    place $::gui::_prntFrame -x 75 -y 300
}

##########################################################
# Finish setting up a printer.
##########################################################
proc ::gui::_finishPrinter { opt } {

    place forget $::gui::_prntFrame

    if { $opt } {
	puts "*Setup Printer:  $::gui::_printerName  $::gui::_colorMode*"
	return
    }
    
    set ::gui::_printerName $::gui::_savePrinterName
    set ::gui::_colorMode $::gui::_saveColorMode

}


##########################################################
# Output the dielectric attributes.
##########################################################
proc ::gui::_putDielectricLayerToFile { nme fp } {

    set thickness     [$nme cget -thickness]
    set permittivity  [$nme cget -permittivity]
    set permeability  [$nme cget -permeability]
    set lossTangent   [$nme cget -lossTangent]
    puts $fp "$nme: DielectricLayer"
    puts $fp " thikkckness   : $thickness"
    puts $fp " permittivity: $permittivity"
    puts $fp " permeability: $permeability"
    puts $fp " lossTangent : $lossTangent"
}


##########################################################
# Output the dielectric attributes.
##########################################################
proc ::gui::_putRectangleDielectricToFile { nme fp } {

    set thickness     [$nme cget -thickness]
    set permittivity  [$nme cget -permittivity]
    set permeability  [$nme cget -permeability]
    set lossTangent   [$nme cget -lossTangent]
    set dheight       [$nme cget -height]
    set dwidth        [$nme cget -width]
    set dnumber       [$nme cget -number]
    set dpitch        [$nme cget -pitch]
    set dxoffset      [$nme cget -xOffset]
    set dyoffset      [$nme cget -yOffset]
    puts $fp "$nme: DielectricLayer"
    puts $fp " thickness   : $thickness"
    puts $fp " permittivity: $permittivity"
    puts $fp " permeability: $permeability"
    puts $fp " lossTangent : $lossTangent"
    puts $fp " height      : $dheight"
    puts $fp " width       : $drwidth"
    puts $fp " number      : $dnumber"
    puts $fp " pitch       : $dpitch"
    puts $fp " x-offset    : $dxoffset"
    puts $fp " y-offset    : $dyoffset"
}


##########################################################
# Output the rectangle conductor attributes.
##########################################################
proc ::gui::_putRectangleConductorsToFile { nme fp } {

    set rheight       [$nme cget -height]
    set rwidth        [$nme cget -width]
    set rconductivity [$nme cget -conductivity]
    set rnumber       [$nme cget -number]
    set rpitch        [$nme cget -pitch]
    set rxoffset      [$nme cget -xOffset]
    set ryoffset      [$nme cget -yOffset]
    puts $fp "$nme: RectangleConductors"
    puts $fp " height      : $rheight"
    puts $fp " width       : $rwidth"
    puts $fp " conductivity: $rconductivity"
    puts $fp " number      : $rnumber"
    puts $fp " pitch       : $rpitch"
    puts $fp " x-offset    : $rxoffset"
    puts $fp " y-offset    : $ryoffset"
}



##########################################################
# Output the circle conductor attributes.
##########################################################
proc ::gui::_putCircleConductorsToFile { nme fp } {

    set cdiameter     [$nme cget -diameter]
    set cconductivity [$nme cget -conductivity]
    set cnumber       [$nme cget -number]
    set cpitch        [$nme cget -pitch]
    set cxoffset      [$nme cget -xOffset]
    set cyoffset      [$nme cget -yOffset]
    puts $fp "$nme: CircleConductors"
    puts $fp " diameter    : $cdiameter"
    puts $fp " conductivity: $cconductivity"
    puts $fp " number      : $cnumber"
    puts $fp " pitch       : $cpitch"
    puts $fp " x-offset    : $cxoffset"
    puts $fp " y-offset    : $cyoffset"
}

##########################################################
# Output the trapezoid conductor attributes.
##########################################################
proc ::gui::_putTrapezoidConductorsToFile { nme fp } {

    set theight       [$nme cget -height]
    set ttopwidth     [$nme cget -topWidth]
    set tbotwidth     [$nme cget -bottomWidth]
    set tconductivity [$nme cget -conductivity]
    set tnumber       [$nme cget -number]
    set tpitch        [$nme cget -pitch]
    set txoffset      [$nme cget -xOffset]
    set tyoffset      [$nme cget -yOffset]

    puts $fp "$nme: TrapezoidConductors"
    puts $fp " height      : $theight"
    puts $fp " topWidth    : $ttopwidth"
    puts $fp " botWidth    : $tbotwidth"
    puts $fp " conductivity: $tconductivity"
    puts $fp " number      : $tnumber"
    puts $fp " pitch       : $tpitch"
    puts $fp " x-offset    : $txoffset"
    puts $fp " y-offset    : $tyoffset"
}


##########################################################
# Output the ground plane attributes.
##########################################################
proc ::gui::_putGroundPlaneToFile { nme fp } {

    set gname         $nme
#   set gthickness    [$nme cget -thickness]
    puts $fp "$nme: GroundPlane"
#thickness   : "$thickness"
}

##########################################################
# Read in a new permittivity-list.
##########################################################
proc ::gui::guiReadNewPermittivityList {} {

    _readMaterialList "Read Permittivity List" ::gui::_pValList 1 \
	    [file join $::gui::scriptDir permittivity.list]

    #  _pValList is actually an array.  Lets copy the values
    #  into a proper list.
    set  ::gui::_permittivityList [list]
    foreach { name value } [ array get ::gui::_pValList ] {
	lappend ::gui::_permittivityList "$name $value"
    }

}

##########################################################
# Read in a new loss-tangent list.
##########################################################
proc ::gui::guiReadNewLossTangentList {} {

    _readMaterialList "Read lossTangent List" ::gui::_ltValList 1 \
	    [file join $::gui::scriptDir loss_tangent.list]

    #  _ltValList is actually an array.  Lets copy the values
    #  into a proper list.
    set  ::gui::_losstangentList [list]
    foreach { name value } [ array get ::gui::_ltValList ] {
	lappend ::gui::_losstangentList "$name $value"
    }

}

##########################################################
# Read in a new conductivity-list.
##########################################################
proc ::gui::guiReadNewConductivityList {} {

    _readMaterialList "Read Conductivity List" ::gui::_cValList 1 \
	    [file join $::gui::scriptDir conductivity.list]

    #  _cValList is actually an array.  Lets copy the values
    #  into a proper list.
    set  ::gui::_conductivityList [list]
    foreach { name value } [ array get ::gui::_cValList ] {
	lappend ::gui::_conductivityList "$name $value"
    }

}


##########################################################
# Log the attributes of the design.
##########################################################
proc ::gui::_logDesignAttributes { fp } {

    #
    # General attributes.
    #
    puts $fp "UNITS\t\t\t$::Stackup::defaultLengthUnits"
    puts $fp "COUPLING_LENGTH\t\t$::Stackup::couplingLength"
    puts $fp "RISE_TIME\t\t$::Stackup::riseTime"
    puts $fp "CSEG\t\t\t$::gui::_num_c_segs"
    puts $fp "DSEG\t\t\t$::gui::_num_p_segs"

    set lst [itcl::find objects -class DielectricLayer]
    set lstrd [itcl::find objects -class RectangleDielectric]
    set numDiel [expr { [llength $lst] + [llength $lstrd] }]
    puts $fp "NUM_DIEL\t\t$numDiel"
    
    #
    # Dielectric attributes.
    #
    foreach itm $lst {
	if { [string length $itm] == 0 } {
	    break
	}
	set nme [string range $itm 2 [string length $itm]]
	set strg [format {%s_THICKNESS} $nme]
	puts $fp "$strg\t\t[$itm cget -thickness]"
	set strg [format {%s_PERMITTIVITY} $nme]
	puts $fp "$strg\t\t[$itm cget -permittivity]"
	set strg [format {%s_LOSSTANGENT} $nme]
	puts $fp "$strg\t\t[$itm cget -lossTangent]"
    }

    foreach itm $lstrd {
	if { [string length $itm] == 0 } {
	    break
	}
	set nme [string range $itm 2 [string length $itm]]
	set strg [format {%s_PERMITTIVITY} $nme]
	puts $fp "$strg\t\t[$itm cget -permittivity]"
	set strg [format {%s_LOSSTANGENT} $nme]
	puts $fp "$strg\t\t[$itm cget -lossTangent]"
	set num [$itm cget -number]
	puts $fp "$strg\t\t$num"
	if { $num > 1 } {
	    set pitch [$itm cget -pitch]
	} else {
	    set pitch 0.0
	}
	set strg [format {%s_PITCH} $nme]
	puts $fp "$strg\t\t$pitch"
	set strg [format {%s_DISTANCE} $nme]
	puts $fp "$strg\t\t[$itm cget -xOffset]"
	set wdt [$itm cget -width]
	set strg [format {%s_WIDTH} $nme]
	puts $fp "$strg\t\t\t$wdt"
	set strg [format {%s_HEIGHT} $nme]
	set hgt [$itm cget -height]
	puts $fp "$strg\t\t\t$hgt"
    }


    set st [itcl::find objects -class RectangleConductors]
    set lst ""
    foreach itm $st {
	lappend lst $itm
    }
    set st [itcl::find objects -class CircleConductors]
    foreach itm $st {
	lappend lst $itm
    }
    set st [itcl::find objects -class TrapezoidConductors]
    foreach itm $st {
	lappend lst $itm
    }
    set numConds [llength $lst]
    puts $fp "NUM_COND_SETS\t\t$numConds"
    set shapes { RECTANGLE CIRCLE TRAPEZOID }

    #
    # Conductor attributes.
    #
    foreach itm $lst {

	if { [string length $itm] == 0 } {
	    break
	}

	set nme [string range $itm 2 [string length $itm]]
	set num [$itm cget -number]
	set strg [format {%s_NUM_COND} $nme]
	puts $fp "$strg\t\t$num"
	if { $num > 1 } {
	    set pitch [$itm cget -pitch]
	} else {
	    set pitch 0.0
	}
	set strg [format {%s_PITCH} $nme]
	puts $fp "$strg\t\t$pitch"
	set strg [format {%s_COND} $nme]
	puts $fp "$strg\t\t[$itm cget -conductivity]"
	set strg [format {%s_DISTANCE} $nme]
	puts $fp "$strg\t\t[$itm cget -xOffset]"
	set strg [format {%s_SHAPE} $nme]

	if { [$itm isa RectangleConductors] } {
	    set opt 0
	} 
	if { [$itm isa CircleConductors] } {
	    set opt 1
	} 
	if { [$itm isa TrapezoidConductors] } {
	    set opt 2
	}

	puts $fp "$strg\t\t[lindex $shapes $opt]"
	set strg [format {%s_UW} $nme]
	switch -- $opt {
	    0 {
		set wdt [$itm cget -width]
	    } 
	    1 {
		set wdt [expr {[$itm cget -diameter] * 0.5} ]
	    } 
	    2 {
		set wdt [$itm cget -topWidth]
	    }
	}
	puts $fp "$strg\t\t\t$wdt"
	set strg [format {%s_LW} $nme]
	if { $opt == 2 } {
	    set wdt [$itm cget -bottomWidth]
	} else {
	    set wdt 0.0
	}
	puts $fp "$strg\t\t\t$wdt"
	set strg [format {%s_H} $nme]
	if { $opt == 1 } {
	    set hgt [expr {[$itm cget -diameter] * 0.5} ]
	} else {
	    set hgt [$itm cget -height]
	}
	puts $fp "$strg\t\t\t$hgt"
    }
}

##########################################################
# Set the default length units.
##########################################################
proc ::gui::_setDefaultUnits {} {

    set ::units::default(Length) $::Stackup::defaultLengthUnits
    puts "Default units: $::Stackup::defaultLengthUnits"
}

proc ::gui::_getWidthHeight {} {

    set structureList $Stackup::structureList
    set condWidth 0.0
    set condHeight 0.0
    foreach struct $structureList {
        #--------------------------------------------------------------
        # Add the heights of the ground-planes and dielectrics.
        #--------------------------------------------------------------
        if {[$struct isa GroundPlane] || [$struct isa DielectricLayer]} {
	    continue
        }

        #--------------------------------------------------------------
        # Get the largest conductor width and the shortest conductor
        # height.
        #--------------------------------------------------------------
	set width [$struct width]
	if { $width > $condWidth } {
	    set condWidth $width
	    set conductivity [$struct cget -conductivity]
	    set condHeight [$struct height]
	}
    }
    return "$condWidth $condHeight $conductivity"
}
