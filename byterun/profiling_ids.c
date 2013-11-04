#include <stdlib.h>
#include "profiling_ids.h"

int get_prof_id(const char *filename, const char *funname, int count) {
  int i = 0;
  while (all_ids[i].filename != NULL) {
    if (all_ids[i].filename == filename &&
        all_ids[i].funname == funname &&
        all_ids[i].count == count)
      return all_ids[i].id_val;
    i ++;
  }
  return -1;
}
