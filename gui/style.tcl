# as_style.tcl --
#
#	This file implements package as::style.
#
# Copyright (c) 2003 ActiveState Corporation, a division of Sophos
# Donated to tklib style package in early 2004
#
# Copied May 2004 by Bob Techentin and modified to add options
# at "userDefault" priority for DTE sessions on HP-UX.
#
#  $Id: style.tcl,v 1.2 2004/07/20 14:51:55 techenti Exp $
#

package provide gui 2.0

namespace eval ::gui {
    if { [tk windowingsystem] == "x11" } {
	set highlightbg "#316AC5" ; # SystemHighlight
	set highlightfg "white"   ; # SystemHighlightText
	set bg          "white"   ; # SystemWindow
	set fg          "black"   ; # SystemWindowText

	## Fonts
	##
	set size	-12
	set family	Helvetica
	set fsize	-12
	set ffamily	Courier


	font create ASfont      -size $size -family $family
	font create ASfontBold  -size $size -family $family -weight bold
	font create ASfontFixed -size $fsize -family $ffamily
	for {set i -2} {$i <= 4} {incr i} {
	    set isize  [expr {$size + ($i * (($size > 0) ? 1 : -1))}]
	    set ifsize [expr {$fsize + ($i * (($fsize > 0) ? 1 : -1))}]
	    font create ASfont$i      -size $isize -family $family
	    font create ASfontBold$i  -size $isize -family $family -weight bold
	    font create ASfontFixed$i -size $ifsize -family $ffamily
	}

	#---------------------------------------------------------------
	#  Check for CDE - which overrides options at userDefault level.
	#  Note that the CDE package at http://wiki.tcl.tk/cde has
	#  a much more complex (and probably more correct) way to
	#  determine if CDE is running - but this works for Mayo SPPDG.
	#---------------------------------------------------------------
	if { [info exists ::env(DTUSERSESSION)] } {
	    set priority userDefault
	    option add *background #d9d9d9 $priority
	} else {
	    set priority widgetDefault
	}

	option add *Text.font		ASfontFixed $priority
	option add *Button.font		ASfont $priority
	option add *Canvas.font		ASfont $priority
	option add *Checkbutton.font	ASfont $priority
	option add *Entry.font		ASfont $priority
	option add *Label.font		ASfont $priority
	option add *Labelframe.font	ASfont $priority
	option add *Listbox.font	ASfont $priority
	option add *Menu.font		ASfont $priority
	option add *Menubutton.font	ASfont $priority
	option add *Message.font	ASfont $priority
	option add *Radiobutton.font	ASfont $priority
	option add *Spinbox.font	ASfont $priority

	option add *Table.font		ASfont $priority
	option add *TreeCtrl*font	ASfont $priority
	## Misc
	##
	option add *ScrolledWindow.ipad		0 $priority

	## Listbox
	##
	option add *Listbox.background		$bg $priority
	option add *Listbox.foreground		$fg $priority
	option add *Listbox.selectBorderWidth	0 $priority
	option add *Listbox.selectForeground	$highlightfg $priority
	option add *Listbox.selectBackground	$highlightbg $priority
	option add *Listbox.activeStyle		dotbox $priority

	## Button
	##
	option add *Button.padX			1 $priority
	option add *Button.padY			2 $priority

	## Entry
	##
	option add *Entry.background		$bg $priority
	option add *Entry.foreground		$fg $priority
	option add *Entry.selectBorderWidth	0 $priority
	option add *Entry.selectForeground	$highlightfg $priority
	option add *Entry.selectBackground	$highlightbg $priority

	## Spinbox
	##
	option add *Spinbox.background		$bg $priority
	option add *Spinbox.foreground		$fg $priority
	option add *Spinbox.selectBorderWidth	0 $priority
	option add *Spinbox.selectForeground	$highlightfg $priority
	option add *Spinbox.selectBackground	$highlightbg $priority

	## Text
	##
	option add *Text.background		$bg $priority
	option add *Text.foreground		$fg $priority
	option add *Text.selectBorderWidth	0 $priority
	option add *Text.selectForeground	$highlightfg $priority
	option add *Text.selectBackground	$highlightbg $priority

	## Menu
	##
	option add *Menu.activeBackground	$highlightbg $priority
	option add *Menu.activeForeground	$highlightfg $priority
	option add *Menu.activeBorderWidth	0 $priority
	option add *Menu.highlightThickness	0 $priority
	option add *Menu.borderWidth		1 $priority

	## Menubutton
	##
	option add *Menubutton.activeBackground	$highlightbg $priority
	option add *Menubutton.activeForeground	$highlightfg $priority
	option add *Menubutton.activeBorderWidth	0 $priority
	option add *Menubutton.highlightThickness	0 $priority
	option add *Menubutton.borderWidth		0 $priority
	option add *Menubutton*padX			4 $priority
	option add *Menubutton*padY			2 $priority

	## Scrollbar
	##
	option add *Scrollbar.width		12 $priority
	option add *Scrollbar.troughColor	#bdb6ad $priority
	option add *Scrollbar.borderWidth		1 $priority
	option add *Scrollbar.highlightThickness	0 $priority

	## PanedWindow

	##
	option add *Panedwindow.borderWidth		0 $priority
	option add *Panedwindow.sashwidth		3 $priority
	option add *Panedwindow.showhandle		0 $priority
	option add *Panedwindow.sashpad		0 $priority
	option add *Panedwindow.sashrelief		flat $priority
	option add *Panedwindow.relief		flat $priority
    }
}; # end of namespace style::as
