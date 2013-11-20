
#include "memprof.h"

#include "mlvalues.h"
#include "alloc.h"
#include "minor_gc.h"
#include "roots.h"

#include "intext.h"
#include "instruct.h"

#ifndef NATIVE_CODE
#include "stacks.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int caml_memprof_do_dump = 0;

int next_location_id = 1024;

char **location_tables = NULL;
mlsize_t *location_table_sizes = NULL;
int location_tables_next = 0;
int location_tables_max = 0;
char location_tables_filename[256];

void really_dump_heap(char *dumpname); // forward-declaration

void caml_may_dump_heap (char *dumpname)
{
  if(caml_memprof_do_dump) really_dump_heap(dumpname);
}

CAMLprim value caml_dump_heap (char *dumpname)
{
  caml_minor_collection();
  really_dump_heap(dumpname);
  return Val_unit;
}

/* Signal handler to dump heap image */
static void handle_signal(int signal)
{
  switch (signal) {
  case SIGHUP:
    really_dump_heap("");
    break;
  }
}

void caml_memprof_init()
{
  struct sigaction sa;

  /* Initialiaze the location table */
  location_tables_max = 128;
  location_tables = caml_stat_alloc(location_tables_max * sizeof(value));
  location_table_sizes = caml_stat_alloc(location_tables_max * sizeof(mlsize_t));

  /* Register signal handler */
  sa.sa_handler = &handle_signal;
  sa.sa_flags = SA_RESTART;
  signal(SIGHUP, handle_signal);
  sigfillset(&sa.sa_mask);
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    perror("Error: cannot handle SIGHUP");
  }

  /* Remove locations files if present */
  sprintf(location_tables_filename, "memprof.%d.locations", getpid());
  unlink(location_tables_filename);
}


#ifndef ARCH_SIXTYFOUR

value caml_register_location_table (value table, value elems)
{
  return Val_int(0);
}

void really_dump_heap(char *dumpname) {
  return;
}

#else

// FIXME in asmcomp/cmmgen.ml

int caml_register_loc_table (char* table, mlsize_t len, int elems)
{
  int offset;

  if(location_tables_next >= location_tables_max) {
    location_tables_max += 1;
    location_tables_max *= 2;
    location_tables = caml_stat_resize(location_tables,
                                       location_tables_max * sizeof(value));
    location_table_sizes = caml_stat_resize(location_table_sizes,
                                            location_tables_max * sizeof(mlsize_t));
  }

  // FIXME: test next_location_id < (2^21)...
  offset = next_location_id;
  next_location_id += elems;

  location_tables[location_tables_next] = table;
  location_table_sizes[location_tables_next] = len;
  location_tables_next++;

  return offset;
}

CAMLprim value caml_register_location_table (value table, value elems)
{
  int offset =
    caml_register_loc_table(String_val(table), caml_string_length(table),
                            Int_val(elems));
  return (Val_int(offset));
}


void dump_location_tables(void)
{
  FILE *locations_file;
  int i;
  locations_file = fopen(location_tables_filename, "a");
  for(i = 0; i < location_tables_next; i++) 
    fwrite(location_tables[i], 1, location_table_sizes[i], locations_file);
  fclose(locations_file);
  location_tables_next = 0;
}



#define Next(hp) ((hp) + Bhsize_hp (hp))

static FILE *file_oc;

static void store_value(value v)
{
  if(sizeof(value) == 4){
    fputc( (v & 0xff), file_oc);
    fputc( ( (v >> 8) & 0xff), file_oc);
    fputc( ( (v >> 16) & 0xff), file_oc);
    fputc( ( (v >> 24) & 0xff), file_oc);
  } else {
    fputc( (v & 0xff), file_oc);
    fputc( ( (v >> 8) & 0xff), file_oc);
    fputc( ( (v >> 16) & 0xff), file_oc);
    fputc( ( (v >> 24) & 0xff), file_oc);
    fputc( ( (v >> 32) & 0xff), file_oc);
    fputc( ( (v >> 40) & 0xff), file_oc);
    fputc( ( (v >> 48) & 0xff), file_oc);
    fputc( ( (v >> 56) & 0xff), file_oc);
  }
}

static void check_block (char *hp)
{
  mlsize_t nfields = Wosize_hp (hp);
  mlsize_t i;
  value v = Val_hp (hp);
  value f;
  /* mlsize_t lastbyte; */
  int tag = Tag_hp (hp);
  profiling_t locid = Prof_hp(hp);

  fputc(1, file_oc); /* 1, a block */
  store_value( v); /* the pointer */
  fputc(tag, file_oc); /* the tag */
  store_value(nfields); /* the size */
  store_value(locid);  /* id which will allow us to find the location ! */
  // printf(" ## dumping LOCID: %d  -- tag %d\n", locid, tag);

  /* if tag < No_scan_tag only, the contents of the block */
  switch(tag){
    case String_tag:
    case Double_tag:
    case Double_array_tag:
    case Custom_tag: break;

    default:
    if(Tag_hp (hp) < No_scan_tag){
      for (i = 0; i < Wosize_hp (hp); i++){
        f = Field (v, i);
#ifndef NATIVE_CODE
        if ((char *) f >= code_area_start && (char *) f < code_area_end) {
#ifdef THREADED_CODE
        if ( *(code_t)f == (opcode_t)(caml_instr_table[RESTART] - caml_instr_base) ){
#else
        if ( *(code_t)f == RESTART ){
#endif
            store_value( (value)code_area_end);
          } else {
            store_value( (value)code_area_start);
          }
        } else
#endif
        {
          store_value( f);
        }
/*
        if (Is_block (f) && Is_in_heap (f)) {
          fprintf( " %x", f);
        }
*/
      }
    }
  }
}

void store_root(value v, value *useless)
{
  if(Is_block(v) && Is_in_heap(v)) store_value(v);
}

extern char *caml_exe_name;

#ifdef NATIVE_CODE
extern char globals_map[];
extern value caml_globals[];
extern value caml_globals_info[]; /* CAGO FIXME */
#endif

void really_dump_heap (char *dumpname)
{
  char *chunk = caml_heap_start, *chunk_end;
  char *cur_hp;
  header_t cur_hd;
  char filename[256];
  static int heap_number = 0;
  if (strcmp(dumpname, "") != 0)
    sprintf(filename, "memprof.%d.%s.%d.dump", getpid(), dumpname, heap_number++);
  else
    sprintf(filename, "memprof.%d.%d.dump", getpid(), heap_number++);

  file_oc = fopen(filename, "w");

  fputc(sizeof(value), file_oc);

  if (location_tables_next) dump_location_tables();

  {
    int size = strlen(caml_exe_name);
    store_value(size);
    fwrite(caml_exe_name, 1, size, file_oc);
  }

  while (chunk != NULL){
    chunk_end = chunk + Chunk_size (chunk);

    fputc(0, file_oc); /* 0: a chunk */
    store_value( (value) chunk); /* chunk start */
    store_value( (value) chunk_end); /* chunk end */

    cur_hp = chunk;
    while (cur_hp < chunk_end){
      cur_hd = Hd_hp (cur_hp);
                                           Assert (Next (cur_hp) <= chunk_end);
      switch (Color_hd (cur_hd)){
      case Caml_white:
        if ((Wosize_hd (cur_hd) == 0)
            || (caml_gc_phase == Phase_sweep && cur_hp >= caml_gc_sweep_hp)){
            /* free block */
          }else{
            check_block ( cur_hp);
        }
        break;
      case Caml_gray: case Caml_black:
        check_block ( cur_hp);
        break;
      case Caml_blue:
        /* free block */
        break;
      }
      cur_hp = Next (cur_hp);
    }                                          Assert (cur_hp == chunk_end);
    chunk = Chunk_next (chunk);
  }
  fputc(255, file_oc); /* 255: end of the file */

/* All CLOSURE Codepointers have this value */
  store_value( (value)code_area_start);
/* All RESTART Codepointers have this value */
  store_value( (value)code_area_end);

#ifdef NATIVE_CODE
  store_value(0); /* We are in native code */

/* /\* We need to store the globals_map *\/ */
/* { */
/*     value s = globals_map; */
/*     int len = caml_string_length(s); */
/*     store_value( len ); */
/*     fwrite(s, 1, len, file_oc);     */
/* } */

/* We need to store the caml_globals, and their corresponding pointers */
{
  value* s = caml_globals;
  int pos = 0;
  while(s[pos] != 0) {
    value m = s[pos];
    pos++;

    if(!(Is_block(m))){
      store_value(1); /* Another module */
      store_value(0);
    } else {
      int size = Wosize_val(m);
      int i;

      store_value(1); /* Another module */
      store_value(m);
      store_value(size);
      for(i=0; i<size; i++)
        store_value(Field(m,i));
    }
  }
  store_value(0); /* End of the table */
}

/* We need to store the caml_globals, and their corresponding pointers */
{
  value* s = caml_globals_info;
  int pos = 0;
  while(s[pos] != 0) {
    value m = s[pos];
    pos++;

    store_value(1); /* Another module */
    int len = caml_string_length(m);
    store_value( len );
    fwrite((value*)m, 1, len, file_oc);
  }
  store_value(0); /* End of the table */
}


#else  /* ####### BYTECODE ####### */
  store_value( Val_int(1));
  store_value( caml_global_data); /* global table */
{
  register value * sp;
  int values = 0;
  for (sp = caml_extern_sp; sp < caml_stack_high; sp++) {
    value v = *sp;
    if(Is_block(v) && Is_in_heap(v)) values++;
  }
  store_value( Val_int(values));  /* number of entries in the stack */
  for (sp = caml_extern_sp; sp < caml_stack_high; sp++) {
    value v = *sp;
    if(Is_block(v) && Is_in_heap(v)) { store_value(1); store_value (v);}
  }
  store_value(0);  /* End */
}
#endif

  caml_do_roots(store_root);

  store_value(Val_int(0)); /* a 0 value at the end */
  fclose(file_oc);
}

#endif // ARCH_SIXTYFOUR
