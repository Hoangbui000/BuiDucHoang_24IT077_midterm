#include "ls.h"

/* Bin ton cc ni b lu c phc v so snh trong qsort */
static unsigned int g_sort_flags = 0;

void
set_cmp_flags(unsigned int flags)
{
    g_sort_flags = flags;
}

int
cmp_entries(const void *a, const void *b)
{
    const FileEntry *fa = (const FileEntry *)a;
    const FileEntry *fb = (const FileEntry *)b;
    int res = 0;

    /* Sp xp theo kch thc (-S) */
    if (g_sort_flags & FLAG_S) {
        if (fa->sb.st_size < fb->sb.st_size)
            res = 1;
        else if (fa->sb.st_size > fb->sb.st_size)
            res = -1;
        else
            res = strcmp(fa->name, fb->name);
    }
    /* Sp xp theo mc thi gian (-t) */
    else if (g_sort_flags & FLAG_t) {
        time_t ta = get_entry_time(&fa->sb, g_sort_flags);
        time_t tb = get_entry_time(&fb->sb, g_sort_flags);
        if (ta < tb)
            res = 1;
        else if (ta > tb)
            res = -1;
        else
            res = strcmp(fa->name, fb->name);
    }
    /* Mc nh: sp xp t in theo tn */
    else {
        res = strcmp(fa->name, fb->name);
    }

    /* o chiu nu bt c -r */
    if (g_sort_flags & FLAG_r) {
        res = -res;
    }

    return res;
}
