bin_PROGRAMS = xnmmtl gnmmtl

AM_CXXFLAGS = -DFORTRAN_UNDERBARS

F77 = f90

FFLAGS=+ppu -O2

EXEEXT = .exe

LDFLAGS = -L/sppdg/software/libs/rwav/workspaces/HPUX1020/HP102/0s/lib

LDADD = -lzip0s -lzap0s -lzhp0s -ltls0s -lSM -lICE -lm

cpp_SOURCES = \
	assemble.cpp \
	assemble_free_space.cpp \
	dim2.cpp \
	electro_read_icon_section.cpp \
	error_location.cpp \
	free2.cpp \
	in_string.cpp \
	math_library.cpp \
	nmmtl_add_to_sorted_list.cpp \
	nmmtl_angle_of_intersection.cpp \
	nmmtl_build_gnd_die_list.cpp \
	nmmtl_charge.cpp \
	nmmtl_circle_segments.cpp \
	nmmtl_charimp_propvel_calculate.cpp \
	nmmtl_combine_die.cpp \
	nmmtl_containment.cpp \
	nmmtl_det_arc_intersections.cpp \
	nmmtl_det_intersections.cpp \
	nmmtl_dump.cpp \
	nmmtl_dump_geometry.cpp \
	nmmtl_eval_circles.cpp \
	nmmtl_eval_conductors.cpp \
	nmmtl_eval_polygons.cpp \
	nmmtl_eval_rectangles.cpp \
	nmmtl_fill_die_gaps.cpp \
	nmmtl_find_ground_planes.cpp \
	nmmtl_find_nu.cpp \
	nmmtl_form_die_subseg.cpp \
	nmmtl_gen_netlist_from_icon.cpp \
	nmmtl_genel.cpp \
	nmmtl_genel_ccs.cpp \
	nmmtl_genel_cls.cpp \
	nmmtl_genel_die.cpp \
	nmmtl_genel_gnd.cpp \
	nmmtl_intersections.cpp \
	nmmtl_interval.cpp \
	nmmtl_jacobian.cpp \
	nmmtl_load.cpp \
	nmmtl_merge_die_subseg.cpp \
	nmmtl_new_die_seg.cpp \
	nmmtl_nl_expand.cpp \
	nmmtl_orphans.cpp \
	nmmtl_output_headers.cpp \
	nmmtl_output_matrices.cpp \
	nmmtl_overlap_parallel_seg.cpp \
	nmmtl_dc_resistance.cpp \
	nmmtl_qsp_calculate.cpp \
	nmmtl_qsp_kernel.cpp \
	nmmtl_retrieve.cpp \
	nmmtl_set_offset.cpp \
	nmmtl_shape.cpp \
	nmmtl_spout_off.cpp \
	nmmtl_sort_gnd_die_list.cpp \
	nmmtl_unload.cpp \
	nmmtl_write_plot_data.cpp \
	nmmtl_xtk_calculate.cpp \
	output_charimp_propvel.cpp \
	output_crosstalk.cpp \
	plotFileInitialization.cpp \
	remove_all_spaces.cpp \
	units.cpp \
	nmmtl.cpp

ftrn_SOURCES = \
        fft.F \
	corth.F \
        ceigv.F \
        cmtms.F \
        mtms.F \
        cmslv1.F \
        mslv.F \
        dmslv.F \
        dcmslv.F \
        sgefa.F \
        sgeco.F \
        dgefa.F \
        sgesl.F \
        dgesl.F \
        fmin.F 	\
	daxpy.F \
	ddot.F \
	saxpy.F \
	sdot.F \
	idamax.F \
	dscal.F \
	sasum.F \
	sscal.F \
	isamax.F \
	dcfact.F \
	dcsol.F \
	dcminv.F \
	dgeco.F \
	dgedi.F \
	cbal.F \
	sgedi.F \
	cgefa.F \
	cgesl.F \
	comqr2.F \
	cbabk2.F \
	sfft.F \
	spmpar.F \
	cdivid.F \
	dswap.F \
	dasum.F \
	cgedi.F \
	sswap.F \
	caxpy.F \
	cdotc.F \
	icamax.F \
	cscal.F \
	ipmpar.F \
	cswap.F \
	dpmpar.F

gnmmtl_SOURCES = nmmtl_parse_graphic.cpp $(cpp_SOURCES) $(ftrn_SOURCES)

xnmmtl_SOURCES = nmmtl_parse_xsctn.cpp $(cpp_SOURCES) $(ftrn_SOURCES)














