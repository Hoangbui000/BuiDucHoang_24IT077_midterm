#include "ls.h"

/* Ly k t loi file cho nh dng di -l */
char
get_file_type_char(mode_t mode)
{
    if (S_ISDIR(mode))  return 'd';
    if (S_ISLNK(mode))  return 'l';
    if (S_ISCHR(mode))  return 'c';
    if (S_ISBLK(mode))  return 'b';
    if (S_ISFIFO(mode)) return 'p';
    if (S_ISSOCK(mode)) return 's';
    return '-';
}

/* K t hu t cho c -F */
char
get_suffix_char(mode_t mode)
{
    if (S_ISDIR(mode))  return '/';
    if (S_ISLNK(mode))  return '@';
    if (S_ISSOCK(mode)) return '=';
    if (S_ISFIFO(mode)) return '|';
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) return '*';
    return '\0';
}

/* nh dng chui 10 k t quyn hn theo chun man page NetBSD */
void
format_mode_string(mode_t mode, char *out)
{
    out[0] = get_file_type_char(mode);

    /* Quyn ca Owner */
    out[1] = (mode & S_IRUSR) ? 'r' : '-';
    out[2] = (mode & S_IWUSR) ? 'w' : '-';
    if (mode & S_ISUID)
        out[3] = (mode & S_IXUSR) ? 's' : 'S';
    else
        out[3] = (mode & S_IXUSR) ? 'x' : '-';

    /* Quyn ca Group */
    out[4] = (mode & S_IRGRP) ? 'r' : '-';
    out[5] = (mode & S_IWGRP) ? 'w' : '-';
    if (mode & S_ISGID)
        out[6] = (mode & S_IXGRP) ? 's' : 'S';
    else
        out[6] = (mode & S_IXGRP) ? 'x' : '-';

    /* Quyn ca Others */
    out[7] = (mode & S_IROTH) ? 'r' : '-';
    out[8] = (mode & S_IWOTH) ? 'w' : '-';
    if (mode & S_ISVTX)
        out[9] = (mode & S_IXOTH) ? 't' : 'T';
    else
        out[9] = (mode & S_IXOTH) ? 'x' : '-';

    out[10] = '\0';
}

/* In tn file vi c ch x l k t khng in c (-q / -w) */
void
print_safe_name(const char *name, unsigned int flags)
{
    for (const char *p = name; *p != '\0'; p++) {
        if ((flags & FLAG_q) && !isprint((unsigned char)*p)) {
            putchar('?');
        } else {
            putchar(*p);
        }
    }
}

/* Ly thi gian chnh xc ty thuc c -c, -u hoc mc nh mtime */
time_t
get_entry_time(const struct stat *sb, unsigned int flags)
{
    if (flags & FLAG_c)
        return sb->st_ctime;
    if (flags & FLAG_u)
        return sb->st_atime;
    return sb->st_mtime;
}

/* c bin mi trng BLOCKSIZE hoc tr v 512 mc nh */
int
get_block_size(void)
{
    char *env = getenv("BLOCKSIZE");
    if (env != NULL) {
        int bsize = atoi(env);
        if (bsize > 0)
            return bsize;
    }
    return 512;
}
