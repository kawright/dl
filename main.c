/* Directory listing */

#include <arr.h>
#include <argvp.h>
#include <core.h>
#include <fileio.h>
#include <mem.h>
#include <print.h>
#include <str.h>

/* ----- CONSTANTS ----- */

#define HELP_TXT_PATH           "/usr/local/etc/dl/HELP.txt"
#define OPTSTR                  "dfhlrv"
#define VER_TXT_PATH            "/usr/local/etc/dl/VERSION.txt"

/* ----- STATIC FUNCTIONS ----- */

static I16 _comp_cb_asc(Void *left, Void *right) {
    FileEntry *fe_left      = (FileEntry *) left;
    FileEntry *fe_right     = (FileEntry *) right;
    return comp_str(fe_left->name, fe_right->name);
}

static I16 _comp_cb_desc(Void *left, Void *right) {
    FileEntry *fe_left      = (FileEntry *) left;
    FileEntry *fe_right     = (FileEntry *) right;
    return comp_str(fe_right->name, fe_left->name);
}

/* ----- MAIN FUNCTION ----- */

I16 main(I16 argc, Ch **argv) {

    // Initialization
    Argvp                   argvp;
    Err                     err;
    Arr                     entries;
    Ch                      *ver_file_txt;
    Ch                      *help_file_txt;
    ver_file_txt            = NIL;
    help_file_txt           = NIL;
    init_argvp(&argvp);
    init_err(&err);
    init_arr(&entries);

    // Parse arguments and handle static output options (-h, -v, ...)
    ld_argvp(&argvp, argc, argv, OPTSTR, &err);
    if (is_err(&err))
        goto CLEANUP;
    if (argvp.pos_ct > 1) {
        THROW(&err, ErrCode_ARGV, "Expected 0 or 1 positionals, %d given", 
            argvp.pos_ct);
        goto CLEANUP;
    }
    if (get_argv_flag(&argvp, 'h')) {
        read_file_to_str(HELP_TXT_PATH, &help_file_txt, &err);
        if (is_err(&err))
            goto CLEANUP;
        print_fmt("\n%s\n", help_file_txt);
        goto CLEANUP;
    }
    if (get_argv_flag(&argvp, 'v')) {
        read_file_to_str(VER_TXT_PATH, &ver_file_txt, &err);
        if (is_err(&err))
            goto CLEANUP;
        print_fmt("\n%s\n", ver_file_txt);
        goto CLEANUP;
    }

    if (argvp.pos_ct == 1) {
        ld_file_entry_arr(&entries, get_argv_pos(&argvp, 0, NIL), &err);
        print_fmt("\nListing for Directory '%s'\n\n", 
            get_argv_pos(&argvp, 0, NIL)); 
    } else {
        ld_file_entry_arr(&entries, ".", &err);
        print_fmt("\nListing for Directory '.'\n\n"); 
    }
    if (is_err(&err))
        goto CLEANUP;
    if (get_argv_flag(&argvp, 'r')) {
        sort_arr(&entries, _comp_cb_desc);
    } else {
        sort_arr(&entries, _comp_cb_asc);
    }

    FileEntry       *curr_entry         = NIL;
    Ch              *file_type_str      = NIL;
    U16             entry_ct            = 0;
    print_ln("NAME                             TYPE     SIZE");
    print_ln("==================================================");
    for (U16 i; i < entries.sz; i++) {
        curr_entry = (FileEntry *) get_arr_elem(&entries, i, NIL);
        switch (curr_entry->type) {
            case FileEntryType_FILE:
            file_type_str = "FILE";
            break;

            case FileEntryType_DIR:
            file_type_str = "DIR";
            break;

            case FileEntryType_LINK:
            file_type_str = "LINK";
            break;

            default:
            file_type_str = "OTHER";
        }
        if (get_argv_flag(&argvp, 'd') && 
                curr_entry->type != FileEntryType_DIR)
            continue;
        if (get_argv_flag(&argvp, 'f') &&
                curr_entry->type != FileEntryType_FILE)
            continue;
        if (get_argv_flag(&argvp, 'l') &&
                curr_entry->type != FileEntryType_LINK)
            continue;
        if (curr_entry->type == FileEntryType_FILE) {
            print_fmt("%-32.32s %-8s %08llx\n", curr_entry->name, file_type_str, 
                curr_entry->sz);
        } else {
            print_fmt("%-32.32s %-8s\n", curr_entry->name, file_type_str);
        }
        entry_ct++;
    }
    print_ln("==================================================");
    print_fmt("Total Entries: %d\n\n", entry_ct);

    CLEANUP:
    if (is_err(&err))
        warn(&err);
    free_mem(ver_file_txt);
    free_mem(help_file_txt);
    clean_arr(&entries, free_mem);

    return err.code;
}
