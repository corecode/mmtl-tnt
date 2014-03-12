#------------------------------------------------------------
#                                 This is a -*-Tcl-*- file
#  getFilePreview.tcl
#
#  File->open dialog component including a text preview.
#
#  This GUI component is designed to be used in dialogs
#  that implement functionality similar to tk_getOpenFile,
#  but need a text preview of the files.
#
#  This component is implemented as a BWidget mega-widget,
#  because we want custom options for directories, filetypes
#  and file names.
#
#  Commands in this file:
#    getFilePreview::create
#    getFilePreview::configure
#    getFilePreview::cget
#    getFilePreview::_path_command
#    getFilePreview::_browseDir
#    getFilePreview::_setFileList
#    getFilePreview::_getSelectedFilePath
#    getFilePreview::_previewFile
#    getFilePreview::_arrowKey
#    getFilePreview::_setFileType
#    getFilePreview::getFilePreviewDialog
#
#  Bob Techentin
#  June 10, 2003
#  Copyright (C) Mayo Foundation.  All Rights Reserved.
#
#  $Id: getFilePreview.tcl,v 1.3 2004/01/05 22:06:39 techenti Exp $
#
#------------------------------------------------------------
package provide getFilePreview 1.0

package require BWidget

#------------------------------------------------------------
#  Create a getFilePreview megawidget namespace
#------------------------------------------------------------
namespace eval getFilePreview {

    #  Declare the widget with a special option
    Widget::declare getFilePreview {
	{-filetypes String {{{All Files} {*}}} 0}
	{-filename String "" 0}
	{-directory String "" 0}
    }

    #  Use these kinds of BWidgets
    LabelEntry::use
    ComboBox::use
    Button::use

    ::bind BWgetFilePreview <Destroy> {Widget::destroy %W; rename %W {}}

    proc getFilePreview { path args } {return [eval getFilePreview::create $path $args]}
    proc use {} {}
}


#------------------------------------------------------------
#  getFilePreview::create
#
#  Create the getFilePreview megawidget
#------------------------------------------------------------
proc getFilePreview::create { path args } {

    #------------------------------------------------------------
    #  Standard BWidget creation code
    #------------------------------------------------------------
    array set maps [list getFilePreview {} :cmd {} .dirent {} .ftype {}]
    array set maps [Widget::parseArgs getFilePreview $args]

    eval frame $path $maps(:cmd) -class getFilePreview \
	-relief flat -bd 0 -highlightthickness 0 -takefocus 0
    Widget::initFromODB getFilePreview $path $maps(getFilePreview)

    bindtags $path [list $path BwgetFilePreview [winfo toplevel $path] all]

    
    #------------------------------------------------------------
    #  Paned window contains file names and preview
    #------------------------------------------------------------
    set pw [PanedWindow::create $path.pw -side top]
    pack $pw -side top -expand yes -fill both 

    #------------------------------------------------------------
    #  Create Directory, File List and File Extensions on the left
    #------------------------------------------------------------
    set pane [$pw add]
    set title [TitleFrame $pane.lf -text "File Selection"]
    set titleframe [$title getframe]

    #  Directory selection
    set dent [LabelEntry $path.dirent -label "Directory:"]

    #  Set the initial directory
    foreach {option value} $args {
	switch -exact -- $option {
	    "-directory" {
		configure $path -directory $value
	    }
	}
    }

    set dbut [Button::create $path.dirbtn \
		  -text "Browse..." \
		  -command [list getFilePreview::_browseDir $path]]
    #  File list
    set sw [ScrolledWindow $titleframe.sw \
		-relief sunken -borderwidth 2]
    set flist [listbox $path.listbox]
    $sw setwidget $flist

    #  File type selector
    set flab [label $titleframe.ftypelab -text "File Types:"]    
    set ftypes [ComboBox $path.ftype \
		    -modifycmd [list getFilePreview::_setFileType $path]]

    set pad 4
    grid $dent - $dbut \
		  -in $titleframe -sticky news -padx $pad -pady $pad
    grid $sw -columnspan 3 \
		  -in $titleframe -sticky news -padx $pad -pady $pad
    grid $flab $ftypes - \
		  -in $titleframe -sticky news -padx $pad -pady $pad
    grid rowconfigure $titleframe 1 -weight 1
    grid columnconfigure $titleframe 1 -weight 1
    pack $title -side top -expand yes -fill both 
    pack $pane -side top -expand yes -fill both


    #------------------------------------------------------------
    #  Create a Text Previewer on the right
    #------------------------------------------------------------
    set pane [$pw add]
    set title [TitleFrame $pane.lf -text "Preview Text"]
    set titleframe [$title getframe]
    set sw [ScrolledWindow $titleframe.sw \
		-relief sunken -borderwidth 2]
    set txt [text $path.text -font {Courier 10} -width 40 \
		 -wrap none -state disabled]
    $sw setwidget $txt
    pack $sw -side top -expand yes -fill both -padx $pad -pady $pad 
    pack $title -side top -expand yes -fill both 
    pack $pane -side top -expand yes -fill both


    #------------------------------------------------------------
    #  Set up bindings 
    #------------------------------------------------------------
    #   Listbox selection invokes _previewFile
    #------------------------------------------------------------
    bind $flist <<ListboxSelect>>  [list getFilePreview::_previewFile $path]
    #------------------------------------------------------------
    #   Up/Down Arrows invoke _arrowKey to change selection
    #  Set bindings for directory and file types entries.
    #  Set focus on the listbox so that its default bindings work.
    #------------------------------------------------------------
    $dent   bind <Up> [list getFilePreview::_arrowKey $path -1]
    $dent   bind <Down> [list getFilePreview::_arrowKey $path +1]
    $ftypes bind <Up> [list getFilePreview::_arrowKey $path -1]
    $ftypes bind <Down> [list getFilePreview::_arrowKey $path +1]
    bind $flist <Button-1> [list + focus $flist]
    focus -force $flist
    #------------------------------------------------------------
    #   KeyPress events in either directory entry or file
    #     types entry invoke _setFileList.
    #------------------------------------------------------------
    $ftypes bind <KeyPress> [list after idle \
				 [list getFilePreview::_setFileList $path]]
    $dent   bind <KeyPress> [list after idle \
			       [list getFilePreview::_setFileList $path]]

    #------------------------------------------------------------
    #  Catch the <Return> in the directory entry, which could
    #  surprise us by invoking OK on the dialog.
    #------------------------------------------------------------
    $dent   bind <Return> {break}


    #------------------------------------------------------------
    # Initialize directory spec
    #------------------------------------------------------------
    set directory [cget $path -directory]
    if { "$directory" == "" } {
	configure $path -directory [pwd]
    }

    #------------------------------------------------------------
    # Initialize the file types and file list
    #------------------------------------------------------------
    set filetypevalues [list]
    set f [cget $path -filetypes]
    foreach line $f {
	set name [lindex $line 0]
	set type [lindex $line 1]
	lappend filetypevalues [format "%s (%s)" $name $type]
    }
    $path.ftype configure -values $filetypevalues
    $path.ftype setvalue first
    after idle [list getFilePreview::_setFileType $path]


    #------------------------------------------------------------
    #  Create widget command procedure, ala BWidget standard
    #------------------------------------------------------------
    rename $path ::$path:cmd
    proc ::$path { cmd args } "return \[getFilePreview::_path_command $path \$cmd \$args\]"

    return $path
}


#------------------------------------------------------------
#  Command getFilePreview::configure
#------------------------------------------------------------
proc getFilePreview::configure { path args } {
    foreach {option value} $args {
	switch -exact -- $option {
	    "-directory" {
		#  Copy value to directory entry
		$path.dirent configure -text $value
	    }
	}
    }
    return [Widget::configure $path $args]
}


#------------------------------------------------------------
#  Command getFilePreview::cget
#------------------------------------------------------------
proc getFilePreview::cget { path option } {
    switch -exact -- $option {
	"-directory" {
	    #  Copy directory entry to widget option
	    set directory [$path.dirent cget -text]
	    Widget::configure $path [list -directory $directory]
	}
    }
    return [Widget::cget $path $option]
}


#------------------------------------------------------------
#  Command getFilePreview::_path_command
#------------------------------------------------------------
proc getFilePreview::_path_command { path cmd larg } {
    if { ![string compare $cmd "configure"] ||
         ![string compare $cmd "cget"] ||
         ![string compare $cmd "bind"] } {
        return [eval getFilePreview::$cmd $path $larg]
    } else {
        return 
    }
}


#------------------------------------------------------------
#  getFilePreview::_browseDir
#
#  User clicks "Browse..." button for the directory
#  entry.  Call tk_chooseDirectory to get a new
#  directory entry.  Then update the files list.
#------------------------------------------------------------
proc getFilePreview::_browseDir { path } {
    set directory [cget $path -directory]
    set directory [tk_chooseDirectory -initialdir $directory -parent $path]
    if { "$directory" != "" } {
	configure $path -directory $directory
    }
    after idle [list getFilePreview::_setFileList $path]
}


#------------------------------------------------------------
#  getFilePreview::_setFileList
#
#  Creates a list of files based on the directory entry
#  and the file types filter.  Put the list of files
#  into the listbox.
#------------------------------------------------------------
proc getFilePreview::_setFileList { path } {
    $path.listbox delete 0 end
    set directory [cget $path -directory]
    set filetypes [$path.ftype cget -text]
    set fileList [list]
    foreach filter $filetypes {
	if { [string range $filter 0 0] != "*"} {
	    set filter "*$filter"
	}
	set files [glob -nocomplain [file join $directory $filter]]
	set fileList [concat $fileList $files]
    }
    foreach f [lsort -dictionary -unique $fileList] {
	if { ([file type $f] == "file") &&  ([file readable $f]) } {
	    $path.listbox insert end [file tail $f]
	}
    }
}


#------------------------------------------------------------
#  getFilePreview::_getSelectedFilePath
#
#  Appends directory spec and listfile selection
#  to produce a complete path to the selected file.
#  Used by _previewFile and returned by cget -filename.
#------------------------------------------------------------
proc getFilePreview::_getSelectedFilePath { path } {

    # get directory and file name
    set directory [cget $path -directory]
    if { [catch {$path.listbox get [$path.listbox curselection]} filename] } {
	# no valid selection or empty listbox
	set filespec ""
    } else {
	#  Construct the file path, and make sure it exists
	set filespec [file join $directory $filename]
	if { [catch {file type $filespec}] } {
	    set filespec ""
	}
    }
    return $filespec
}

#------------------------------------------------------------
#  getFilePreview::_previewFile
#
#  Display the contents of the currently selected
#  file in the text widget.
#------------------------------------------------------------
proc getFilePreview::_previewFile { path } {

    #  Clear the text preview
    #  (but keep a copy of the text)
    set oldText [split [$path.text get 1.0 end] "\n"]
    $path.text configure -state normal
    $path.text delete 1.0 end

    set filespec [_getSelectedFilePath $path]

    # display file preview, catching errors.
    catch {file type $filespec} filetype 
    if { "$filetype" == "file" } {
	if { [catch {
	    set fp [open $filespec "r"]
	    set data [read $fp 100000]
	    close $fp
	    if { [string is ascii $data] } {
		$path.text insert end $data
	    } else {
		$path.text insert end "<binary data>"
	    }
	    #  Since we've successfully previewed the file,
	    #  save the file spec in the megawidget
	    configure $path -filename $filespec
	} ] } {
	    #  Preview failed
	    configure $path -filename ""
	}
    } else {
	#  No preview - not a "file"
	configure $path -filename ""
    }

    #  Add highlighting, or more correctly, de-emphasize
    #  identical lines by graying them out slightly.
    set i 1
    set newText [split [$path.text get 1.0 end] "\n"]
    foreach oldline $oldText newline $newText {
	if { [string equal $oldline $newline]} {
	    $path.text tag add grayout $i.0 $i.end
	}
	incr i
    }
    $path.text tag configure grayout -foreground gray40

    $path.text configure -state disabled
}


#------------------------------------------------------------
#  getFilePreview::_arrowKey
#
#  Adjust the listbox selection for <Up> and <Down>
#  arrow key events.  Refresh the file preview.
#------------------------------------------------------------
proc getFilePreview::_arrowKey { path increment } {
    set indx [$path.listbox curselection]
    if { "$indx" != "" } {
	incr indx $increment
	if { ($indx >= 0) && ($indx < [$path.listbox size]) } {
	    $path.listbox selection clear 0 end
	    $path.listbox selection set $indx
	    $path.listbox see $indx
	    after idle [list getFilePreview::_previewFile $path]
	}
    }
}


#------------------------------------------------------------
#  getFilePreview::_setFileType
#
#  Called right after user selects something from
#  the File Type combobox dropdown list.  Edits the 
#  entry field in place so we see only the file extension.
#  Refresh the file list.
#------------------------------------------------------------
proc getFilePreview::_setFileType { path } {
    set entrytext [$path.ftype cget -text]
    # scan string within (parens)
    scan $entrytext "%*\[^(\](%\[^)\]" fileextension
    $path.ftype configure -text "$fileextension"
    after idle [list getFilePreview::_setFileList $path]
}


#------------------------------------------------------------
#  getFilePreview::getFilePreviewDialog
#
#  Simple modal dialog for getting a single file name
#  using the getFilePreview file browser with preview.
#------------------------------------------------------------
proc getFilePreview::getFilePreviewDialog {args} {

    #------------------------------------------------------------
    #  Set some default parameters.
    #  Parse command line options, just like tk_getOpenFile
    #------------------------------------------------------------
    set directory [pwd]
    set types {
	{{All Files}        *             }
    }
    set parent "."
    set title "Open"
    foreach {name value} $args {
	switch -exact -- $name {
	    -initialdir {set directory $value}
	    -filetypes {set types $value}
	}
    }


    #------------------------------------------------------------
    #  Build dialog, only if necessary.
    #------------------------------------------------------------
    set dlg .getFilePreviewDialog
    if { ! [winfo exist $dlg] } {
	#  Create BWidget Dialog
	set dlg [Dialog $dlg \
		     -parent $parent \
		     -modal local \
		     -title   $title \
		     -side    bottom    \
		     -anchor  e  \
		     -default 0 -cancel 1]
	$dlg add -name Open -text "Open"
	$dlg add -name Cancel -text "Cancel"
	set gf [getFilePreview::getFilePreview $dlg.getfile \
		    -directory $directory -filetypes $types]
	pack $gf -expand yes -fill both
    }


    #------------------------------------------------------------
    #  Now draw the dialog, and return the results
    #------------------------------------------------------------
    set result [$dlg draw]
    if { $result == 0 } {
	return [$dlg.getfile cget -filename]
    } else {
	return ""
    }
}









#------------------------------------------------------------
#  Standalone Testing Code
#------------------------------------------------------------
if { $::argv0 == [info script] } {
    wm geometry . +800+400
    #wm withdraw .
    set types {
	{{All Files}        *                    }
	{{Text Files}       {.txt}               }
	{{TCL Scripts}      {.tcl}               }
	{{C Source Files}   {.c .h}         TEXT }
	{{Graphics Files}   {.gif .jpg .png}     }
    }
    set filespec [getFilePreview::getFilePreviewDialog \
		     -filetypes $types -title "getFilePreview Test"]
    tk_messageBox -message "Selected File = '$filespec'"
    exit
}






