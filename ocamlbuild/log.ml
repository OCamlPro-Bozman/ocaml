(***********************************************************************)
(*                             ocamlbuild                              *)
(*                                                                     *)
(*  Nicolas Pouillard, Berke Durak, projet Gallium, INRIA Rocquencourt *)
(*                                                                     *)
(*  Copyright 2007 Institut National de Recherche en Informatique et   *)
(*  en Automatique.  All rights reserved.  This file is distributed    *)
(*  under the terms of the Q Public License version 1.0.               *)
(*                                                                     *)
(***********************************************************************)


(* Original author: Nicolas Pouillard *)
open My_std

module Debug = struct
let mode _ = true
end
include Debug

let level = ref 1

let classic_display = ref false
let internal_display = ref None
let failsafe_display = lazy (Display.create ~mode:`Classic ~log_level:!level ())

let ( !- ) r =
  match !r with
  | None -> !*failsafe_display
  | Some x -> x

let init log_file =
  let mode =
    if !classic_display || !*My_unix.is_degraded || !level <= 0 || not (My_unix.stdout_isatty ()) then
      `Classic
    else
      `Sophisticated
  in
  internal_display := Some (Display.create ~mode ?log_file ~log_level:!level ())

let raw_dprintf log_level = Display.dprintf ~log_level !-internal_display

let dprintf log_level fmt = raw_dprintf log_level ("@[<2>"^^fmt^^"@]@.")
let eprintf fmt = dprintf (-1) fmt

let update () = Display.update !-internal_display
let event ?pretend x = Display.event !-internal_display ?pretend x
let display x = Display.display !-internal_display x

let finish ?how () =
  match !internal_display with
  | None -> ()
  | Some d -> Display.finish ?how d

(*let () = My_unix.at_exit_once finish*)
