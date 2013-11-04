#ifndef __PROFILING_IDS__
#define __PROFILING_IDS__

typedef struct {
  const char *filename;
  const char *funname;
  const int count;
  const int id_val;
} id_desc;

static const id_desc all_ids[] = {
  /* alloc.c */
  { "alloc.c", "caml_alloc", 0, 1 },  
  { "alloc.c", "caml_alloc", 1, 2 },  
  { "alloc.c", "caml_alloc_small", 2, 3 },  
  { "alloc.c", "caml_alloc_tuple", 3, 4 },  
  { "alloc.c", "caml_alloc_string", 4, 5 },  
  { "alloc.c", "caml_alloc_string", 5, 6 },  
  { "alloc.c", "caml_alloc_final", 6, 7 },
  { "alloc.c", "caml_copy_string", 7, 8 },
  { "alloc.c", "caml_alloc_array", 8, 9 },
  { "alloc.c", "caml_alloc_dummy", 9, 10 },
  { "alloc.c", "caml_alloc_dummy_float", 10, 11 },

  /* array.c */
  { "array.c", "caml_array_get_float", 0, 12 },  
  { "array.c", "caml_array_unsafe_get_float", 1, 13 },  
  { "array.c", "caml_make_vect", 2, 14 },  
  { "array.c", "caml_make_vect", 3, 15 },  
  { "array.c", "caml_make_vect", 4, 16 },  
  { "array.c", "caml_make_vect", 5, 17 },  
  { "array.c", "caml_make_array", 6, 18 },  
  { "array.c", "caml_array_gather", 7, 19 },  
  { "array.c", "caml_array_gather", 8, 20 },  
  { "array.c", "caml_array_gather", 9, 21 },  

  /* backtrace.c */
  { "backtrace.c", "read_debug_info", 0, 22 },  
  { "backtrace.c", "caml_get_exception_backtrace", 1, 23 },  
  { "backtrace.c", "caml_get_exception_backtrace", 2, 24 },  
  { "backtrace.c", "caml_get_exception_backtrace", 3, 25 },  
  { "backtrace.c", "caml_get_exception_backtrace", 4, 26 },  
  { "backtrace.c", "caml_get_exception_backtrace", 5, 27 },  

  /* callback.c */
  
  /* compact.c */
  { "compact.c", "do_compaction", 0, 28 },  
  { "compact.c", "caml_compact_heap", 1, 29 },  

  /* compare.c */

  /* custom.c */
  { "custom.c", "caml_alloc_custom", 0, 30 },  
  { "custom.c", "caml_alloc_custom", 1, 31 },  

  /* debugger.c */
  { "debugger.c", "caml_debugger_init", 0, 32 },  
  
  /* dynlink.c */
  { "dynlink.c", "caml_dynlink_open_lib", 0, 33 },  
  { "dynlink.c", "caml_dynlink_lookup_symbol", 1, 34 },  
  { "dynlink.c", "caml_dynlink_get_current_libs", 2, 35 },  
  { "dynlink.c", "caml_dynlink_get_current_libs", 3, 36 },  
  
  /* extern.c */
  { "extern.c", "caml_output_value_to_string", 0, 37 },  
  
  /* fail.c */
  { "fail.c", "caml_raise_with_string", 0, 38 },  
  { "fail.c", "caml_init_exceptions", 1, 39 },  

  
  /* finalise.c */
  /* fix_code.c */

  /* floats.c */
  { "floats.c", "caml_copy_double", 0, 40 },  
  { "floats.c", "caml_format_float", 1, 41 },  
  { "floats.c", "caml_frexp_float", 2, 42 },  
  { "floats.c", "caml_modf_float", 3, 43 },  

  /* freelist.c */
  { "freelist.c", "allocate_block", 0, 44 },  
  { "freelist.c", "allocate_block", 1, 45 },  
  { "freelist.c", "caml_fl_merge_block", 2, 46 },  
  { "freelist.c", "caml_fl_merge_block", 3, 47 },  
  { "freelist.c", "caml_fl_merge_block", 4, 48 },  
  { "freelist.c", "caml_make_free_block", 5, 49 },  

  /* gc_ctrl.c */
  /* globroots.c */
  /* hash.c */
  /* instrtrace.c */

  /* intern.c */
  { "intern.c", "read_prof", 0, 50 },  
  { "intern.c", "intern_add_to_heap", 1, 51 },  
  { "intern.c", "caml_input_val", 2, 52 },  
  { "intern.c", "caml_input_value_from_string", 3, 53 },  
  { "intern.c", "caml_input_value_from_malloc", 4, 54 },  
  { "intern.c", "caml_input_value_from_block", 5, 55 },  

  /* interp.c */
  { "interp.c", "caml_interprete", 0, 56 },  
  { "interp.c", "caml_interprete", 1, 57 },  
  { "interp.c", "caml_interprete", 2, 58 },  
  { "interp.c", "caml_interprete", 3, 59 },  
  { "interp.c", "caml_interprete", 4, 60 },  
  { "interp.c", "caml_interprete", 5, 61 },  
  { "interp.c", "caml_interprete", 6, 62 },  
  { "interp.c", "caml_interprete", 7, 63 },  
  { "interp.c", "caml_interprete", 8, 64 },  
  { "interp.c", "caml_interprete", 9, 65 },  
  { "interp.c", "caml_interprete", 10, 66 },  
  { "interp.c", "caml_interprete", 11, 67 },  

  /* ints.c  */
  { "ints.c", "caml_format_int", 0, 68 },  
  { "ints.c", "caml_copy_int32", 1, 69 },  
  { "ints.c", "caml_int32_format", 2, 70 },  
  { "ints.c", "caml_copy_int64", 3, 71 },  
  { "ints.c", "caml_int64_format", 4, 72 },  
  { "ints.c", "caml_copy_nativeint", 5, 73 },  
  { "ints.c", "caml_nativeint_format", 6, 74 },  
  
  /* io.c */
  { "io.c", "caml_alloc_channel", 0, 75 },  
  { "io.c", "caml_ml_open_descriptor_in", 1, 76 },  
  { "io.c", "caml_ml_open_descriptor_out", 2, 77 },  
  { "io.c", "caml_ml_out_channels_list", 3, 78 },  
  { "io.c", "caml_ml_out_channels_list", 4, 79 },  

  /* lexing.c */
  /* main.c */

  /* major_gc.c */
  { "major_gc.c", "sweep_slice", 0, 80 },  
  { "major_gc.c", "caml_init_major_heap", 1, 81 },  
  
  /* md5.c */
  { "md5.c", "caml_md5_string", 0, 82 },  
  { "md5.c", "caml_md5_chan", 1, 83 },  

  /* memory.c */

  /* meta.c */
  { "meta.c", "caml_reify_bytecode", 0, 84 },  
  { "meta.c", "caml_realloc_global", 1, 85 },  

  /* minor_gc.c */
  /* misc.c */

  /* obj.c */
  { "obj.c", "caml_obj_block", 0, 86 },  
  { "obj.c", "caml_obj_truncate", 1, 87 },  
  { "obj.c", "caml_obj_truncate", 2, 88 }, 
  { "obj.c", "caml_lazy_make_forward", 3, 89 },  

  /* parsing.c */
  /* prims.c */
  /* printexc.c */
  /* roots.c */
  /* signals_byt.c */
  /* signals.c */
  /* stacks.c */

  /* startup.c */
  { "startup.c", "init_atoms", 0, 90 },  

  /* str.c */
  { "str.c", "caml_create_string", 0, 91 },  

  /* sys.c */
  { "sys.c", "caml_sys_error", 0, 92 },  
  { "sys.c", "caml_sys_error", 1, 93 },  
  { "sys.c", "caml_sys_getcwd", 2, 94 },  
  { "sys.c", "caml_sys_getenv", 3, 95 },  
  { "sys.c", "caml_sys_get_argv", 4, 96 },  
  { "sys.c", "caml_sys_get_config", 5, 97 },  

  /* terminfo.c */
  /* unix.c */

  /* weak.c */
  { "weak.c", "caml_weak_create", 0, 98 },  
  { "weak.c", "caml_weak_get", 1, 99 },  

  /* win32.c */
  
  { NULL, NULL, 0, 0 }
};

int get_prof_id(const char *filename, const char *funname, int count);



#define PROF_TODO 42

#define PROF_DUMMY 1
#define PROF_ATOM 2
#define PROF_CONS 3
#define PROF_CAML_OUTPUT_VAL_TO_STR 4

/* INTS */
#define PROF_INT32 5
#define PROF_INT64 6
#define PROF_INT_NATIVE 7
#define PROF_CAML_FORMAT_INT 8
#define PROF_CAML_FORMAT_INT32 9
#define PROF_CAML_FORMAT_INT64 10
#define PROF_CAML_FORMAT_NATIVEINT 11


/* FLOAT */
#define PROF_FLOAT_ALLOC_DUMMY_FLOAT 12
#define PROF_FLOAT_CP_DOUBLE 13
#define PROF_FLOAT_FREXP_FLOAT 14
#define PROF_FLOAT_MODF_FLOAT 15
#define PROF_FLOAT_GET_FLOAT 16
#define PROF_FLOAT_UNSAFE_GET_FLOAT 17
#define PROF_FORMATFLOAT 18

/* MD5 */
#define PROF_MD5_STRING 19
#define PROF_MD5_CHAN 20

/* TUPLE */
#define PROF_TUPLE_ALLOC_TUPLE 21
#define PROF_TUPLE_DYNLINK_GET_CUR_LIBS 22

/* ARRAYS */
#define PROF_ARRAYS_ALLOC_ARRAY 23
#define PROF_ARRAYS_MK_VECT_1 24
#define PROF_ARRAYS_MK_VECT_2 25
#define PROF_ARRAYS_MK_VECT_3 26
#define PROF_ARRAYS_GATHER_1 27
#define PROF_ARRAYS_GATHER_2 28
#define PROF_DOUBLE_ARRAYS_MK_VECT 29
#define PROF_DOUBLE_ARRAYS_MK_ARRAY 30
#define PROF_DOUBLE_ARRAYS_GATHER 31
#define PROF_BIG_ARRAY 32

/* OBJ */
#define PROF_OBJ_BLOCK 33
#define PROF_OBJ_TRUNCATE_1 34 
#define PROF_OBJ_TRUNCATE_2 35
#define PROF_OBJ_MK_FORWARD 36

/* IO */
#define PROF_IO_ALLOC_CHANNEL 37
#define PROF_IO_OPEN_DESC_IN 38
#define PROF_IO_OPEN_DESC_OUT 39
#define PROF_IO_OUT_CHANNELS_LIST 40 

/* WEAK */
#define PROF_WEAK_CREATE 41
#define PROF_WEAK_GET 43

#define PROF_RAISE_WITH_STRING 44
#define PROF_FAIL_EXN 45

/* SYS */
#define PROF_CAML_SYS_ERROR_1 46
#define PROF_CAML_SYS_ERROR_2 47
#define PROF_CAML_SYS_GETCWD 48
#define PROF_CAML_SYS_GETARGV 49
#define PROF_CAML_SYS_GETCONFIG 50
#define PROF_CAML_SYS_GETENV 51

/* INTERN.c */
#define PROF_CAML_INP_VAL_FROM_MALLOC 52
#define PROF_CAML_INP_VAL_FROM_BLOCK 53
#define PROF_CAML_INP_VAL_FROM_STRING 54
#define PROF_INTERN_TOHEAP 55

/* META.c */
#define PROF_REALLOC_GLOBAL 56
#define PROF_REIFY_BYTECODE 57

/* major.c */
#define PROF_SWEEP_SLICE 58
#define PROF_INIT_MAJ_HEAP 59

#define PROF_COMPACTION 60

/* ALLOC */
#define PROF_CAML_ALLOC_1 61
#define PROF_CAML_ALLOC_2 62
#define PROF_CAML_ALLOC_SMALL 63


#define PROF_INPUT_VALUES 64

#define PROF_EXTERN_WRITE_PROF  65

/* SIGNALS */
#define PROF_SIGNALS_INSTALL_SIGNAL_HANDLER_1 66
#define PROF_SIGNALS_INSTALL_SIGNAL_HANDLER_2 67

/* BACKTRACE */
#define PROF_BACKTRACE_READ_DEBUG_INFO 68
#define PROF_BACKTRACE_GET_EXN_BACKTRACE_1 69
#define PROF_BACKTRACE_GET_EXN_BACKTRACE_2 70
#define PROF_BACKTRACE_GET_EXN_BACKTRACE_3 71
#define PROF_BACKTRACE_GET_EXN_BACKTRACE_4 72
#define PROF_BACKTRACE_GET_EXN_BACKTRACE_5 73

#define PROF_PAIR 74

/* FREELIST */
#define PROF_FREELIST_ALLOC_BLOCK_1 75
#define PROF_FREELIST_ALLOC_BLOCK_2 76
#define PROF_FREELIST_MERGE_BLOCK_1 77
#define PROF_FREELIST_MERGE_BLOCK_2 78
#define PROF_FREELIST_MERGE_BLOCK_3 79
#define PROF_FREELIST_MK_FREE_BLOCK 80

#define PROF_DYNLINK_OPEN_LIB 81
#define PROF_DYNLINK_LOOKUP_SYM 82
#define PROF_DYNLINK_GET_CUR_LIBS 83

/* STRING */
#define PROF_STRING_CREATE_STRING 84
#define PROF_STRING_ALLOC_STRING_1 85
#define PROF_STRING_ALLOC_STRING_2 86
#define PROF_STRING_CP_STRING 87

#endif
