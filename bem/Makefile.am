#----------------------------------------------------------------
#
#  Makefile.am
#
#  Automake source file for Boundary Element MMTL (bem).
#  This file, along with aclocal, configure.ac autoconf,
#  and automake can produce a highly functional configuration
#  script and Makefile.
#
#  Sharon Zahn, Bob Techentin
#  October 3, 2002
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: Makefile.am,v 1.5 2004/04/22 20:07:12 techenti Exp $
#
#----------------------------------------------------------------

#----------------------------------------------------------------
#  Just for the record, this isn't a GNU project, and 
#  doesn't have all the support files like LICENSE and
#  ChangeLog.  We're also compiling in a subdirectory, so
#  we supply a couple of options to automake.
#
#  Note that this package _does_ require some of the support
#  files like install-sh, so if you're running aclocal,
#  autoconf, and automake for the first time, you'll need to
#  supply command line switches --add-missing --copy.
#----------------------------------------------------------------
AUTOMAKE_OPTIONS = foreign subdir-objects

#---------------------------------------------------------------
# tcl/tk scripts that may need to be installed.
#---------------------------------------------------------------
pkglib_SCRIPTS = \
        lib/bem.tcl \
	lib/bem_gpge.itcl \
        lib/bem_graphs.itcl \
        lib/bem_iterate.tcl \
        lib/bem_parameters.tcl \
        lib/bem_sweep.tcl \
        lib/bem_view.tcl \
        lib/bem_welement.itcl \
        lib/pkgIndex.tcl

#---------------------------------------------------------------
#  Define an installation directory that includes
#  the version number (from AC_INIT) like "bltmath1.0"
#---------------------------------------------------------------
pkglibdir = $(libdir)/$(PACKAGE)$(PACKAGE_VERSION)

#----------------------------------------------------------------
#  nmmtl is our main target
#----------------------------------------------------------------
bin_PROGRAMS = bem

#----------------------------------------------------------------
#  The old "New MMTL", named nmmtl, reads old-fashioned MagiCAD 
#  GPGE graphics files instead of the newer cross section 
#  descriptions.  
#  Build nmmtl just for testing - don't install it.
#----------------------------------------------------------------
noinst_PROGRAMS = nmmtl

#----------------------------------------------------------------
#  This package has its own test suite
#----------------------------------------------------------------
bem_TESTS = tests/test_bem.tcl

#----------------------------------------------------------------
#  Each program depends on (mostly the same) source files
#----------------------------------------------------------------
bem_SOURCES = $(cpp_SOURCES) $(fortran_SOURCES) \
		src/nmmtl_parse_xsctn.cpp

nmmtl_SOURCES = $(cpp_SOURCES) $(fortran_SOURCES) \
		src/nmmtl_parse_graphic.cpp


#----------------------------------------------------------------
#  Most of nmmtl is written in C++
#----------------------------------------------------------------
cpp_SOURCES = \
	src/assemble.cpp \
	src/assemble_free_space.cpp \
	src/dim2.cpp \
	src/electro_read_icon_section.cpp \
	src/error_location.cpp \
	src/free2.cpp \
	src/in_string.cpp \
	src/math_library.cpp \
	src/nmmtl_add_to_sorted_list.cpp \
	src/nmmtl_angle_of_intersection.cpp \
	src/nmmtl_build_gnd_die_list.cpp \
	src/nmmtl_charge.cpp \
	src/nmmtl_circle_segments.cpp \
	src/nmmtl_charimp_propvel_calculate.cpp \
	src/nmmtl_combine_die.cpp \
	src/nmmtl_containment.cpp \
	src/nmmtl_det_arc_intersections.cpp \
	src/nmmtl_det_intersections.cpp \
	src/nmmtl_dump.cpp \
	src/nmmtl_dump_geometry.cpp \
	src/nmmtl_eval_circles.cpp \
	src/nmmtl_eval_conductors.cpp \
	src/nmmtl_eval_polygons.cpp \
	src/nmmtl_eval_rectangles.cpp \
	src/nmmtl_fill_die_gaps.cpp \
	src/nmmtl_find_ground_planes.cpp \
	src/nmmtl_find_nu.cpp \
	src/nmmtl_form_die_subseg.cpp \
	src/nmmtl_gen_netlist_from_icon.cpp \
	src/nmmtl_genel.cpp \
	src/nmmtl_genel_ccs.cpp \
	src/nmmtl_genel_cls.cpp \
	src/nmmtl_genel_die.cpp \
	src/nmmtl_genel_gnd.cpp \
	src/nmmtl_intersections.cpp \
	src/nmmtl_interval.cpp \
	src/nmmtl_jacobian.cpp \
	src/nmmtl_load.cpp \
	src/nmmtl_merge_die_subseg.cpp \
	src/nmmtl_new_die_seg.cpp \
	src/nmmtl_nl_expand.cpp \
	src/nmmtl_orphans.cpp \
	src/nmmtl_output_headers.cpp \
	src/nmmtl_output_matrices.cpp \
	src/nmmtl_overlap_parallel_seg.cpp \
	src/nmmtl_dc_resistance.cpp \
	src/nmmtl_qsp_calculate.cpp \
	src/nmmtl_qsp_kernel.cpp \
	src/nmmtl_retrieve.cpp \
	src/nmmtl_set_offset.cpp \
	src/nmmtl_shape.cpp \
	src/nmmtl_spout_off.cpp \
	src/nmmtl_sort_gnd_die_list.cpp \
	src/nmmtl_unload.cpp \
	src/nmmtl_write_plot_data.cpp \
	src/nmmtl_xtk_calculate.cpp \
	src/output_charimp_propvel.cpp \
	src/output_crosstalk.cpp \
	src/plotFileInitialization.cpp \
	src/remove_all_spaces.cpp \
	src/units.cpp \
	src/nmmtl.cpp

#----------------------------------------------------------------
#  The math library (math_library.cpp) makes calls to the NSWC
#  math library.  But since we only use a few of the hundreds
#  of modules in NSWC, we just include them here.  It saves
#  having a big library hanging around.
#----------------------------------------------------------------
fortran_SOURCES = \
	src/fft.F \
	src/cmtms.F \
	src/mtms.F \
	src/cmslv1.F \
	src/mslv.F \
	src/dmslv.F \
	src/dcmslv.F \
	src/sgefa.F \
	src/sgeco.F \
	src/dgefa.F \
	src/sgesl.F \
	src/dgesl.F \
	src/fmin.F \
	src/daxpy.F \
	src/ddot.F \
	src/saxpy.F \
	src/sdot.F \
	src/idamax.F \
	src/dscal.F \
	src/sasum.F \
	src/sscal.F \
	src/isamax.F \
	src/dcfact.F \
	src/dcsol.F \
	src/dcminv.F \
	src/dgeco.F \
	src/dgedi.F \
	src/cbal.F \
	src/sgedi.F \
	src/cgefa.F \
	src/cgesl.F \
	src/comqr2.F \
	src/cbabk2.F \
	src/sfft.F \
	src/spmpar.F \
	src/cdivid.F \
	src/dswap.F \
	src/dasum.F \
	src/cgedi.F \
	src/sswap.F \
	src/caxpy.F \
	src/cdotc.F \
	src/icamax.F \
	src/cscal.F \
	src/ipmpar.F \
	src/cswap.F \
	src/dpmpar.F














