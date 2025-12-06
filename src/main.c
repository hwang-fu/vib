#include <stdio.h>
#include <string.h>

#include "vib.h"
#include "common.h"
#include "cstr.h"

static void vib_print_version()
{
    printf("vib %s\n", VIB_VERSION_STRING);
}

static void vib_print_usage(BORROWED const char * prog)
{
    printf("Usage: %s [OPTIONS] [FILE]\n", prog);
    printf("\n");
    printf("Options:\n");
    printf("  --version    Show version and exit\n");
    printf("  --help       Show this help and exit\n");
}

int main(int argc, const char * const argv[])
{
    BORROWED const char * progname = argv[0];

    for (int i = 1; i < argc; i++)
    {
        BORROWED const char * arg = argv[i];
        if (strcmp_smart(arg, "--version"))
        {
            vib_print_version();
            return 0;
        }
        if (strcmp_smart(arg, "--help"))
        {
            vib_print_usage(progname);
            return 0;
        }
    }

    vib_print_usage(progname);
    return 0;
}
