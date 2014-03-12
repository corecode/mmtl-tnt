#----------------------------------------------------------------
#
#  Makefile.am
#
#  Automake source file TNT documentation
#
#  This file, along with aclocal, configure.ac autoconf,
#  and automake can produce a highly functional configuration
#  script and Makefile.
#
#  This makefile is separate from the toplevel makefile, as
#  we want to capture this functionality separately from
#  the main program's build procedure.
#
#  This makefile uses pdflatex and latex2html to generate
#  both PDF and HTML versions of the docs.
#  "Local" rules are used to build and cleanup.
#
#
#  Bob Techentin
#  June 30, 2004
#
#  Copyright 2004 (C) Mayo Foundation.  All Rights Reserved
#
#  $Id: Makefile.am,v 1.1 2004/07/21 15:45:26 techenti Exp $
#
#----------------------------------------------------------------

#----------------------------------------------------------------
#  Just for the record, this isn't a GNU project, and 
#  doesn't have all the support files like LICENSE and
#  ChangeLog.  We're also compiling in a subdirectory, so
#  we supply a couple of options to automake.
#----------------------------------------------------------------
AUTOMAKE_OPTIONS = foreign

#---------------------------------------------------------------
#  Documentation Targets
#
#  Note that automake doesn't support Latex documentation
#  generation, so we're calling it "data", but installed
#  in a special 'doc' dir.
#
#---------------------------------------------------------------

docdir = $(prefix)/doc
doc_DATA = \
	user-guide.pdf 


#---------------------------------------------------------------
#  Compile both PDF and HTML documents
#
#  Support is added for latex documents using bibtex,
#  but the '.bib' file must have the same name as
#  the .tex file.
#
#  Also note that we've got both PDF and HTML targets.
#  But the autotools aren't happy with the idea of 
#  an indeterminate number of html files, so we use the
#  PDF file as the target, supply a rule to compile both
#  the PDF and the HTML, then an installation procedure
#  which copies both the PDF and HTML to the
#  destination "doc" directory.
#---------------------------------------------------------------
%.pdf : %.tex
	pdflatex $*
	if [ -f $*.bib ]; then \
	  echo Generating bibliography for $*; \
	  bibtex $*; \
	fi
	echo Run latex again to get references right $*
	pdflatex $*
	latex2html -split 4 -show_section_numbers -local_icons $<

#  Custom installation rule
install-data-local:
	for f in $(doc_DATA); do \
	  $(mkinstalldirs) $(docdir); \
	  $(INSTALL_DATA) $$f $(docdir)/$$f; \
	  d=`basename $$f .pdf`; \
	  $(mkinstalldirs) $(docdir)/$$d; \
	  $(INSTALL_DATA) $$d/* $(docdir)/$$d; \
	done

#---------------------------------------------------------------
#  clean
#
#  On a good day, we would use a "clean-local" target to
#  describe cleanup after both latex2html and pdflatex.
#  But as of this writing, we don't have a good latex,
#  pdflatex, or latex2html set for the Windows (cygwin)
#  environment.  Without a standard autotools clean
#  target, 'make clean' and 'make distclean' will
#  leave the generated PDF and HTML intact.
#
#---------------------------------------------------------------
#clean-local:
very-clean:
	for f in $(doc_DATA); do \
	  rm -f $$f; \
	  d=`basename $$f .pdf`; \
	  rm -f $$d.aux $$d.bbl $$d.blg $$d.dvi; \
	  rm -f $$d.lof $$d.log $$d.out $$d.toc; \
	  rm -rf $$d; \
	done

