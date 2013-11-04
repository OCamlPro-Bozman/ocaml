#ifndef __MEMPROF__
#define __MEMPROF__

#include "misc.h"
#include "mlvalues.h"

CAMLextern int caml_memprof_do_dump;

void caml_memprof_init(void);
void caml_may_dump_heap (char *dumpname);
value caml_dump_heap (char *dumpname);
value caml_register_location_table (value table, value elems);
int caml_register_loc_table (char* table, mlsize_t len, int elems);

#endif // __MEMPROF__
