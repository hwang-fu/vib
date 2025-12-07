#include <stdio.h>
#include <string.h>

#include "vib.h"
#include "vib_term.h"
#include "vib_keys.h"
#include "common.h"
#include "cstr.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * CLI Helpers
 * ───────────────────────────────────────────────────────────────────────────── */

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


/* ─────────────────────────────────────────────────────────────────────────────
 * Main Loop
 * ───────────────────────────────────────────────────────────────────────────── */

static void draw_tui()
{
    vib_terminal_clear();
    vib_terminal_cursor_home();
    vib_terminal_writef("vib %s | %lux%lu | Key Test Mode\r\n",
                        VIB_VERSION_STRING,
                        vib_terminal_get_columns(),
                        vib_terminal_get_rows());
    vib_terminal_writef("Press keys to see their codes. Press Ctrl+Q to quit.\r\n");
    vib_terminal_writef("─────────────────────────────────────────────────────\r\n");

}

static void loop()
{
    vib_terminal_cursor_hide();
    draw_tui();

    uint64_t line = 4;
    uint64_t max_lines = vib_terminal_get_rows() - 2;

    for (;;)
    {
        /* Handle resize */
        if (vib_terminal_was_resized())
        {
            draw_tui();
            line = 4;
            max_lines = vib_terminal_get_rows() - 2;
        }

        vib_key_t key = vib_keys_read();
        if (key == VIB_KEY_NONE)
        {
            continue;
        }
        if (key == (VIB_CTRL | 'q'))
        {
            /* Quit on Ctrl+Q */
            break;
        }
        if (line >= max_lines)
        {
            /* Clear screen if full */
            draw_tui();
            line = 4;
        }

        /* Display key info */
        vib_terminal_cursor_move(line++, 1);

        vib_key_t base = vib_key_base(key);
        const char * name = vib_key_name_get(key);

        if (base >= 0 && base < 256)
        {
            vib_terminal_writef("Key: %-15s | base: %3d (0x%02X) | raw: 0x%04X\r\n",
                                name, base, base, key);
        }
        else
        {
            vib_terminal_writef("Key: %-15s | base: %3d        | raw: 0x%04X\r\n",
                                name, base, key);
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Entry Point
 * ───────────────────────────────────────────────────────────────────────────── */

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

    COPIED result_t init = vib_terminal_init();
    if (RESULT_IS_ERR(init))
    {
        fprintf(stderr, "error: failed to initialize terminal (code %lu)\n", init.err);
        return 1;
    }

    loop();

    return 0;
}
