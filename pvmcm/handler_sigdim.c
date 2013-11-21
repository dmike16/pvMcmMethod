/* Name: handler_sigdim.c
 * Pourpose: handler for SIGDIM signal
 * Author: dmike
 */

#include <stdio.h>
#include <stdlib.h>
#include "signal_dim.h"

void handler_sigdim(int signal)
{
  fprintf(stderr,"Error incorrect instaction maybe wrong dimension\n Chech the manual to use this program\n");
  abort();
}
