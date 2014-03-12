#----------------------------------------------------------------
#
#  gui_help.tcl
#
#  Basic help functions for TNT gui.
#
#  Based on a topic, this will display the help.
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved
#  $Id: gui_help.tcl,v 1.7 2004/07/26 13:37:52 techenti Exp $
#
#----------------------------------------------------------------

package provide gui 2.0


#------------------------------------------------------------
#
#  ::gui::tnt_help
#
#  Online help functions
#
#------------------------------------------------------------
proc ::gui::tnt_help { helpTopic } {

    switch -exact $helpTopic {

	userguide {
	    display_online_help user-guide/index.html
	}

    }
}


#------------------------------------------------------------
#
#  ::gui::display_online_help
#
#  Display online help in a web browser.
#
#------------------------------------------------------------
proc ::gui::display_online_help { helpTopic } {

    #  Compute a full path spec, which isn't really a url.
    set url [file normalize [file join $::docDirPath $helpTopic]]

    switch $::tcl_platform(platform) {

	windows {
	    #  must convert to short form because this won't
	    #  work with spaces in the file names. :-(
	    set url [file attributes $url -shortname]
	    eval exec [auto_execok start] [list $url] &
	}

	default {
	    #  Try firefox, mozilla, opera, then netscape
	    foreach browser {firefox mozilla opera netscape} {
		set prog [auto_execok $browser]
		if {$prog ne ""} {
		    if {[catch {exec $prog $url &}] == 0} {
			# display temporary message window, then exit
			after 2000 {destroy .helpstartmsg}
			tk_dialog .helpstartmsg "Starting Browser" \
			    "Starting $browser for online help . . ." \
			    "" 0 "Close"
			return
		    }
		}
	    }
	    #  else we've failed
	    error "Unable to start HTML browser on '$url'"
	}
    }
}
