#########################################################################################
##  Filename: addressDialog.tcl
##
##  Proc ShowAddressDlg $oldAdr
##  This proceedure creates and displays a model dialog that blocks until the user applys
##  a new address or presses cancel. The new address (integer 1..30) is returned to the calling
##  proceedure or in the case of cancel, 'false' is returned. It takes one argument, the
##  old instrument address, just to display to the user what the old address was. If no argument
##  is given, the address of '0' is used as the old intrument address.
##
##  Example usage:
## 
##  set newAdr [ShowAddressDlg $oldAdr];		Blocks until user presses 'Apply' of 'Cancel'
##
##  ## newAdr is either an integer (1..30) or false
##
##
##
##  Last Modified 05/13/04, djs
##  Removed the line 'wm transient $addressDlg .'
##
##  Last Modified 05/10/04, djs
##  Initial Development
##
##  Created 05/2004
##  Author Dan Schraufnagel
##  Copyright (C) Mayo Foundation.  All Rights Reserved.
##
#########################################################################################

package require BWidget
package provide addressDialog 1.0
proc QuitAddressDlg {} {
	global addrDlgFlag
	global addressDlg
	global instrAddress
	destroy $addressDlg
	set instrAddress false
	set addrDlgFlag true
}

proc ApplyValue {} {
	global addrDlgFlag
	global addressDlg
	destroy $addressDlg
	set addrDlgFlag true
}

proc ShowAddressDlg { {oldAdr 1} } {
	global addressDlg
	global instrAddress
	global addrDlgFlag
	set addrDlgFlag false
	
	# Address Dialog
	set addressDlg [toplevel .addressDlg]
	wm title $addressDlg "Instrument Address"
	# Size and place the Main Window on the screen
	wm geometry $addressDlg +400+300
	# Re-Direct all Window Close events
	wm protocol $addressDlg WM_DELETE_WINDOW QuitAddressDlg
	set instrAddress 1
	# Create a frame, fields and labels for the meter values
	# -relief sunken -text "Magnet Temperature"
	set readOut [frame $addressDlg.lf -padx 10 -pady 10]
	
	# Create the Dialog fields
	frame $readOut.f0 -padx 10 -pady 10

	set lf [labelframe $readOut.lf -text "GPIB Address Failed" -padx 15 -pady 15]
	label $lf.lbl -text "The Instrument failed to open at GPIB address: $oldAdr\n\
	To try again, select an address and press Apply."
	pack $lf.lbl
	pack $lf
	
	label $readOut.f0.addressLabel -text "Instrument Address:" -width 20 -anchor e
	set adrCombo [ComboBox $readOut.f0.adrCombo -editable false -values "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15\
	 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30" -width 7 \
	 -justify right -modifycmd {set instrAddress [.addressDlg.lf.f0.adrCombo cget -text] }]

	$adrCombo setvalue @[expr {$oldAdr - 1}]

	grid $readOut.f0.addressLabel $adrCombo -sticky e
	pack $readOut.f0
	pack $readOut
	
	# Create a Frame and buttons
	set btnFrame [frame $addressDlg.btnFrame]
	button $btnFrame.applyBtn -text "Apply" -command { ApplyValue }
	button $btnFrame.quitBtn -text "Cancel" -command { QuitAddressDlg }
	pack $btnFrame -side bottom -padx 5 -pady 5
	pack $btnFrame.applyBtn -side left
	pack $btnFrame.quitBtn -side right

	# Display as a model dialog
	wm deiconify $addressDlg
	wm state $addressDlg normal
	tkwait visibility $addressDlg
	focus -force $addressDlg
#	wm transient $addressDlg .
	grab set $addressDlg
	
	# Block until user secles 'Apply' or 'Cancel'
	vwait addrDlgFlag
	
	# Return the new instrument address or false
	return $instrAddress
}

