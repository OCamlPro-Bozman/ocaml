/***********************************************************************/
/*                                                                     */
/*                                OCaml                                */
/*                                                                     */
/*            Xavier Leroy, projet Cristal, INRIA Rocquencourt         */
/*                                                                     */
/*  Copyright 2000 Institut National de Recherche en Informatique et   */
/*  en Automatique.  All rights reserved.  This file is distributed    */
/*  under the terms of the GNU Library General Public License, with    */
/*  the special exception on linking described in file ../LICENSE.     */
/*                                                                     */
/***********************************************************************/

/* $Id$ */

/* Stack backtrace for uncaught exceptions */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "config.h"
#ifdef HAS_UNISTD
#include <unistd.h>
#endif
#include "mlvalues.h"
#include "alloc.h"
#include "io.h"
#include "instruct.h"
#include "intext.h"
#include "exec.h"
#include "fix_code.h"
#include "memory.h"
#include "startup.h"
#include "stacks.h"
#include "sys.h"
#include "backtrace.h"

CAMLexport int caml_backtrace_active = 0;
CAMLexport int caml_backtrace_pos = 0;
CAMLexport code_t * caml_backtrace_buffer = NULL;
CAMLexport value caml_backtrace_last_exn = Val_unit;
CAMLexport char * caml_cds_file = NULL;
#define BACKTRACE_BUFFER_SIZE 1024

/* Location of fields in the Instruct.debug_event record */
enum { EV_POS = 0,
       EV_MODULE = 1,
       EV_LOC = 2,
       EV_KIND = 3 };

/* Location of fields in the Location.t record. */
enum { LOC_START = 0,
       LOC_END = 1,
       LOC_GHOST = 2 };

/* Location of fields in the Lexing.position record. */
enum {
  POS_FNAME = 0,
  POS_LNUM = 1,
  POS_BOL = 2,
  POS_CNUM = 3
};

/* Start or stop the backtrace machinery */

CAMLprim value caml_record_backtrace(value vflag)
{
  int flag = Int_val(vflag);

  if (flag != caml_backtrace_active) {
    caml_backtrace_active = flag;
    caml_backtrace_pos = 0;
    if (flag) {
      caml_register_global_root(&caml_backtrace_last_exn);
    } else {
      caml_remove_global_root(&caml_backtrace_last_exn);
    }
    /* Note: lazy initialization of caml_backtrace_buffer in
       caml_stash_backtrace to simplify the interface with the thread
       libraries */
  }
  return Val_unit;
}

/* Return the status of the backtrace machinery */

CAMLprim value caml_backtrace_status(value vunit)
{
  return Val_bool(caml_backtrace_active);
}

/* Store the return addresses contained in the given stack fragment
   into the backtrace array */

void caml_stash_backtrace(value exn, code_t pc, value * sp)
{
  code_t end_code = (code_t) ((char *) caml_start_code + caml_code_size);
  if (pc != NULL) pc = pc - 1;
  if (exn != caml_backtrace_last_exn) {
    caml_backtrace_pos = 0;
    caml_backtrace_last_exn = exn;
  }
  if (caml_backtrace_buffer == NULL) {
    caml_backtrace_buffer = malloc(BACKTRACE_BUFFER_SIZE * sizeof(code_t));
    if (caml_backtrace_buffer == NULL) return;
  }
  if (caml_backtrace_pos >= BACKTRACE_BUFFER_SIZE) return;
  if (pc >= caml_start_code && pc < end_code){
    caml_backtrace_buffer[caml_backtrace_pos++] = pc;
  }
  for (/*nothing*/; sp < caml_trapsp; sp++) {
    code_t p = (code_t) *sp;
    if (p >= caml_start_code && p < end_code) {
      if (caml_backtrace_pos >= BACKTRACE_BUFFER_SIZE) break;
      caml_backtrace_buffer[caml_backtrace_pos++] = p;
    }
  }
}

/* Read the debugging info contained in the current bytecode executable.
   Return an OCaml array of OCaml lists of debug_event records in "events",
   or Val_false on failure. */

#ifndef O_BINARY
#define O_BINARY 0
#endif

static value read_debug_info(void)
{
  CAMLparam0();
  CAMLlocal1(events);
  char * exec_name;
  int fd;
  struct exec_trailer trail;
  struct channel * chan;
  uint32 num_events, orig, i;
  value evl, l;

  if (caml_cds_file != NULL) {
    exec_name = caml_cds_file;
  } else {
    exec_name = caml_exe_name;
  }
  fd = caml_attempt_open(&exec_name, &trail, 1);
  if (fd < 0) CAMLreturn(Val_false);
  caml_read_section_descriptors(fd, &trail);
  if (caml_seek_optional_section(fd, &trail, "DBUG") == -1) {
    close(fd);
    CAMLreturn(Val_false);
  }
  chan = caml_open_descriptor_in(fd);
  num_events = caml_getword(chan);
  events = caml_alloc_loc(num_events, 0, PROF_BACKTRACE);
  for (i = 0; i < num_events; i++) {
    orig = caml_getword(chan);
    evl = caml_input_val(chan);
    /* Relocate events in event list */
    for (l = evl; l != Val_int(0); l = Field(l, 1)) {
      value ev = Field(l, 0);
      Field(ev, EV_POS) = Val_long(Long_val(Field(ev, EV_POS)) + orig);
    }
    /* Record event list */
    Store_field(events, i, evl);
  }
  caml_close_channel(chan);
  CAMLreturn(events);
}

/* Search the event for the given PC.  Return Val_false if not found. */

static value event_for_location(value events, code_t pc)
{
  mlsize_t i;
  value pos, l, ev, ev_pos, best_ev;

  best_ev = 0;
  Assert(pc >= caml_start_code && pc < caml_start_code + caml_code_size);
  pos = Val_long((char *) pc - (char *) caml_start_code);
  for (i = 0; i < Wosize_val(events); i++) {
    for (l = Field(events, i); l != Val_int(0); l = Field(l, 1)) {
      ev = Field(l, 0);
      ev_pos = Field(ev, EV_POS);
      if (ev_pos == pos) return ev;
      /* ocamlc sometimes moves an event past a following PUSH instruction;
         allow mismatch by 1 instruction. */
      if (ev_pos == pos + 8) best_ev = ev;
    }
  }
  if (best_ev != 0) return best_ev;
  return Val_false;
}

/* Extract location information for the given PC */

struct loc_info {
  int loc_valid;
  int loc_is_raise;
  char * loc_filename;
  int loc_lnum;
  int loc_startchr;
  int loc_endchr;
};

static void extract_location_info(value events, code_t pc,
                                  /*out*/ struct loc_info * li)
{
  value ev, ev_start;

  ev = event_for_location(events, pc);
  li->loc_is_raise = caml_is_instruction(*pc, RAISE);
  if (ev == Val_false) {
    li->loc_valid = 0;
    return;
  }
  li->loc_valid = 1;
  ev_start = Field (Field (ev, EV_LOC), LOC_START);
  li->loc_filename = String_val (Field (ev_start, POS_FNAME));
  li->loc_lnum = Int_val (Field (ev_start, POS_LNUM));
  li->loc_startchr =
    Int_val (Field (ev_start, POS_CNUM))
    - Int_val (Field (ev_start, POS_BOL));
  li->loc_endchr =
    Int_val (Field (Field (Field (ev, EV_LOC), LOC_END), POS_CNUM))
    - Int_val (Field (ev_start, POS_BOL));
}

/* Print location information */

static void print_location(struct loc_info * li, int index)
{
  char * info;

  /* Ignore compiler-inserted raise */
  if (!li->loc_valid && li->loc_is_raise) return;

  if (li->loc_is_raise) {
    /* Initial raise if index == 0, re-raise otherwise */
    if (index == 0)
      info = "Raised at";
    else
      info = "Re-raised at";
  } else {
    if (index == 0)
      info = "Raised by primitive operation at";
    else
      info = "Called from";
  }
  if (! li->loc_valid) {
    fprintf(stderr, "%s unknown location\n", info);
  } else {
    fprintf (stderr, "%s file \"%s\", line %d, characters %d-%d\n",
             info, li->loc_filename, li->loc_lnum,
             li->loc_startchr, li->loc_endchr);
  }
}

/* Print a backtrace */

CAMLexport void caml_print_exception_backtrace(void)
{
  value events;
  int i;
  struct loc_info li;

  events = read_debug_info();
  if (events == Val_false) {
    fprintf(stderr,
            "(Program not linked with -g, cannot print stack backtrace)\n");
    return;
  }
  for (i = 0; i < caml_backtrace_pos; i++) {
    extract_location_info(events, caml_backtrace_buffer[i], &li);
    print_location(&li, i);
  }
}

/* Convert the backtrace to a data structure usable from OCaml */

CAMLprim value caml_get_exception_backtrace(value unit)
{
  CAMLparam0();
  CAMLlocal5(events, res, arr, p, fname);
  int i;
  struct loc_info li;

  events = read_debug_info();
  if (events == Val_false) {
    res = Val_int(0);           /* None */
  } else {
    arr = caml_alloc_loc(caml_backtrace_pos, 0, PROF_BACKTRACE);
    for (i = 0; i < caml_backtrace_pos; i++) {
      extract_location_info(events, caml_backtrace_buffer[i], &li);
      if (li.loc_valid) {
        fname = caml_copy_string_loc(li.loc_filename, PROF_BACKTRACE);
        p = caml_alloc_small_loc(5, 0, PROF_BACKTRACE);
        Field(p, 0) = Val_bool(li.loc_is_raise);
        Field(p, 1) = fname;
        Field(p, 2) = Val_int(li.loc_lnum);
        Field(p, 3) = Val_int(li.loc_startchr);
        Field(p, 4) = Val_int(li.loc_endchr);
      } else {
        p = caml_alloc_small_loc(1, 1, PROF_BACKTRACE);
        Field(p, 0) = Val_bool(li.loc_is_raise);
      }
      caml_modify(&Field(arr, i), p);
    }
    res = caml_alloc_small_loc(1, 0, PROF_BACKTRACE); Field(res, 0) = arr; /* Some */
  }
  CAMLreturn(res);
}
