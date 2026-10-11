#ifndef LS_H
#define LS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <util.h>
#include <ctype.h>

#define FLAG_A      (1U << 0)
#define FLAG_a      (1U << 1)
#define FLAG_c      (1U << 2)
#define FLAG_d      (1U << 3)
#define FLAG_F      (1U << 4)
#define FLAG_f      (1U << 5)
#define FLAG_h      (1U << 6)
#define FLAG_i      (1U << 7)
#define FLAG_k      (1U << 8)
#define FLAG_l      (1U << 9)
#define FLAG_n      (1U << 10)
#define FLAG_q      (1U << 11)
#define FLAG_R      (1U << 12)
#define FLAG_r      (1U << 13)
#define FLAG_S      (1U << 14)
#define FLAG_s      (1U << 15)
#define FLAG_t      (1U << 16)
#define FLAG_u      (1U << 17)
#define FLAG_w      (1U << 18)

typedef struct {
    char        *name;
    char        *path;
    struct stat  sb;
    int          error;
} FileEntry;

extern int g_exit_status;

int cmp_entries(const void *a, const void *b);
void set_cmp_flags(unsigned int flags);
char get_suffix_char(mode_t mode);
void format_mode_string(mode_t mode, char *out);
void print_safe_name(const char *name, unsigned int flags);
time_t get_entry_time(const struct stat *sb, unsigned int flags);
int get_block_size(void);
void print_entries(FileEntry *entries, size_t count, unsigned int flags, int is_dir_content);
void traverse_directory(const char *dirpath, unsigned int flags, int print_header);

#endif /* LS_H */
