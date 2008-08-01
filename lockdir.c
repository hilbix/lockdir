/* Create a directory for locking
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "lockdir_version.h"

void
relax(void)
{
  struct timespec ts;

  ts.tv_sec	= 0;
  ts.tv_nsec	= 100000000 | rand()%10000000;
  nanosleep(&ts, NULL);
}

int
main(int argc, char **argv)
{
  const char *mill="|/-\\";
  int startcount, wait, turn, delta;
  const char *dir;
  time_t mtime;

  if (argc<2 || argc>3)
    {
      fputs("Usage: lockdir dir [seconds]\n\t\t\tVersion " LOCKDIR_VERSION " compiled " __DATE__ "\n", stderr);
      return 1;
    }
  dir		= argv[1];
  startcount	= 0;
  if (argc>2)
    startcount	= 10*strtoul(argv[2], NULL, 0);

  nice(99);
  srand(getpid());

  mtime	= 0;
  delta	= 1;
  turn	= 0;
  wait	= 0;
  while (mkdir(dir, 0700))
    {
      struct stat st;

      if (errno!=EEXIST)
        {
	  perror(dir);
	  return 1;
	}
      for (;;)
        {
	  turn	+= delta;
          putchar(mill[turn&3]);
          putchar(8);
          if (fflush(stdout))
	    {
	      perror("stdin went away");
	      return 1;
	    }
          relax();

          if (stat(dir, &st) && errno==ENOENT)
            break;

          if (st.st_mtime!=mtime)
            {
	      wait	= 0;
	      delta	= -delta;
	      mtime	= st.st_mtime;
	      continue;
            }

          if (startcount && ++wait>startcount && rmdir(dir))
            perror(dir);
	}
    }
  return 0;
}

