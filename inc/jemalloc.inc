jemalloc_printstats() {
  # https://github.com/jemalloc/jemalloc/wiki/Use-Case:-Basic-Allocator-Statistics
  if [[ ! -f /usr/bin/jemalloc-stats && -f /usr/bin/jemalloc.sh ]]; then

mkdir -p /root/tools
JEMSTATS_CFILE='/root/tools/ex_stats_print.c'
JEMSTATS_FILE='/root/tools/ex_stats_print'

cat >"$JEMSTATS_CFILE" <<JEM
#include <stdlib.h>
#include <jemalloc/jemalloc.h>

void
do_something(size_t i)
{

        // Leak some memory.
        malloc(i * 100);
}

int
main(int argc, char **argv)
{
        size_t i;

        for (i = 0; i < 1000; i++) {
                do_something(i);
        }

        // Dump allocator statistics to stderr.
        malloc_stats_print(NULL, NULL, NULL);

        return (0);
}
JEM

  if [ -f "$JEMSTATS_CFILE" ]; then
    gcc "$JEMSTATS_CFILE" -o ex_stats_print -I${JEMALLOC_PATH}/include -L${JEMALLOC_PATH}/lib -Wl,-rpath,${JEMALLOC_PATH}/lib -ljemalloc
  fi
  if [ -f "$JEMSTATS_FILE" ]; then
    ln -sf "$JEMSTATS_FILE" /usr/bin/jemalloc-stats
    chmod 0700 /usr/bin/jemalloc-stats
  fi

  fi
}