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

/* Cu trc mt n bit i din cho cc c ty chn theo man page */
#define FLAG_A      (1U << 0)   /* -A: Lit k tt c tr . v .. */
#define FLAG_a      (1U << 1)   /* -a: Lit k tt c bao gm . v .. */
#define FLAG_c      (1U << 2)   /* -c: Dng thi gian i trng thi status (ctime) */
#define FLAG_d      (1U << 3)   /* -d: Xem th mc nh file thng thng */
#define FLAG_F      (1U << 4)   /* -F: Hin th k t phn loi hu t */
#define FLAG_f      (1U << 5)   /* -f: Khng sp xp kt qu (bt km -a) */
#define FLAG_h      (1U << 6)   /* -h: nh dng kch thc cho ngi c (human-readable) */
#define FLAG_i      (1U << 7)   /* -i: In s inode (serial number) */
#define FLAG_k      (1U << 8)   /* -k: Kch thc tnh theo kilobyte */
#define FLAG_l      (1U << 9)   /* -l: nh dng chi tit (long listing) */
#define FLAG_n      (1U << 10)  /* -n: Dng UID/GID s thay v tn */
#define FLAG_q      (1U << 11)  /* -q: i k t khng in c thnh '?' */
#define FLAG_R      (1U << 12)  /* -R: Duyt  quy th mc con */
#define FLAG_r      (1U << 13)  /* -r: o ngc th t sp xp */
#define FLAG_S      (1U << 14)  /* -S: Sp xp theo dung lng gim dn */
#define FLAG_s      (1U << 15)  /* -s: Hin th s block h thng tp tin */
#define FLAG_t      (1U << 16)  /* -t: Sp xp theo thi gian mi nht */
#define FLAG_u      (1U << 17)  /* -u: Dng thi gian truy cp (atime) */
#define FLAG_w      (1U << 18)  /* -w: In raw cc k t khng in c */

typedef struct {
    char        *name;      /* Tn hin th */
    char        *path;      /* ng dn y   gi lstat */
    struct stat  sb;        /* D liu ly t lstat */
    int          error;     /* M li errno nu lstat tht bi */
} FileEntry;

/* Hm so snh (cmp.c) */
int cmp_entries(const void *a, const void *b);

/* Hm tin ch nh dng (util.c) */
char get_suffix_char(mode_t mode);
void format_mode_string(mode_t mode, char *out);
void print_safe_name(const char *name, unsigned int flags);
time_t get_entry_time(const struct stat *sb, unsigned int flags);
int get_block_size(void);

/* Hm hin th (print.c) */
void print_entries(FileEntry *entries, size_t count, unsigned int flags, int is_dir_content);

/* Hm duyt v x l th mc (traverse.c) */
void traverse_directory(const char *dirpath, unsigned int flags, int print_header);

#endif /* LS_H */
