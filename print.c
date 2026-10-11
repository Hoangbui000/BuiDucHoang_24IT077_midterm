#include "ls.h"

/* In nh dng chi tit (-l / -n) cho mt mc */
static void
print_long_entry(const FileEntry *ent, unsigned int flags, int bsize)
{
    char mode_str[12];
    format_mode_string(ent->sb.st_mode, mode_str);

    /* C -i: In s inode */
    if (flags & FLAG_i) {
        printf("%ju ", (uintmax_t)ent->sb.st_ino);
    }

    /* C -s: In s block */
    if (flags & FLAG_s) {
        long blocks = (ent->sb.st_blocks * 512 + bsize - 1) / bsize;
        printf("%ld ", blocks);
    }

    printf("%s %ju ", mode_str, (uintmax_t)ent->sb.st_nlink);

    /* Owner name hoc numeric UID (-n) */
    struct passwd *pwd = getpwuid(ent->sb.st_uid);
    if ((flags & FLAG_n) || pwd == NULL) {
        printf("%u ", (unsigned int)ent->sb.st_uid);
    } else {
        printf("%s ", pwd->pw_name);
    }

    /* Group name hoc numeric GID (-n) */
    struct group *grp = getgrgid(ent->sb.st_gid);
    if ((flags & FLAG_n) || grp == NULL) {
        printf("%u ", (unsigned int)ent->sb.st_gid);
    } else {
        printf("%s ", grp->gr_name);
    }

    /* Kch thc tp hoc s device major/minor */
    if (S_ISCHR(ent->sb.st_mode) || S_ISBLK(ent->sb.st_mode)) {
        printf("%3d, %3d ", major(ent->sb.st_rdev), minor(ent->sb.st_rdev));
    } else if (flags & FLAG_h) {
        char hbuf[10];
        humanize_number(hbuf, sizeof(hbuf), (int64_t)ent->sb.st_size, "",
                        HN_AUTOSCALE, HN_DECIMAL | HN_NOSPACE);
        printf("%5s ", hbuf);
    } else if (flags & FLAG_k) {
        long ksize = (ent->sb.st_size + 1023) / 1024;
        printf("%jd ", (intmax_t)ksize);
    } else {
        printf("%jd ", (intmax_t)ent->sb.st_size);
    }

    /* nh dng mc thi gian */
    time_t t = get_entry_time(&ent->sb, flags);
    struct tm *lt = localtime(&t);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%b %e %H:%M", lt);
    printf("%s ", time_str);

    /* In tn tp */
    print_safe_name(ent->name, flags);

    /* C -F: In hu t */
    if (flags & FLAG_F) {
        char sfx = get_suffix_char(ent->sb.st_mode);
        if (sfx != '\0')
            putchar(sfx);
    }

    /* Nu l symlink: hin th mi tn v ng dn tr n */
    if (S_ISLNK(ent->sb.st_mode)) {
        char sym_target[1024];
        ssize_t len = readlink(ent->path, sym_target, sizeof(sym_target) - 1);
        if (len != -1) {
            sym_target[len] = '\0';
            printf(" -> %s", sym_target);
        }
    }

    putchar('\n');
}

/* In cc mc danh sch */
void
print_entries(FileEntry *entries, size_t count, unsigned int flags, int is_dir_content)
{
    int bsize = get_block_size();

    /* In dng tng s blocks nu l ni dung th mc theo quy nh */
    if (is_dir_content && (flags & (FLAG_l | FLAG_s))) {
        long total_blocks = 0;
        for (size_t i = 0; i < count; i++) {
            if (entries[i].error == 0) {
                total_blocks += (entries[i].sb.st_blocks * 512 + bsize - 1) / bsize;
            }
        }
        printf("total %ld\n", total_blocks);
    }

    for (size_t i = 0; i < count; i++) {
        if (entries[i].error != 0) {
            fprintf(stderr, "ls: %s: %s\n", entries[i].name, strerror(entries[i].error));
            continue;
        }

        if (flags & FLAG_l) {
            print_long_entry(&entries[i], flags, bsize);
        } else {
            /* In dng mc nh: 1 file / dng */
            if (flags & FLAG_i) {
                printf("%ju ", (uintmax_t)entries[i].sb.st_ino);
            }
            if (flags & FLAG_s) {
                long blocks = (entries[i].sb.st_blocks * 512 + bsize - 1) / bsize;
                printf("%ld ", blocks);
            }

            print_safe_name(entries[i].name, flags);

            if (flags & FLAG_F) {
                char sfx = get_suffix_char(entries[i].sb.st_mode);
                if (sfx != '\0')
                    putchar(sfx);
            }
            putchar('\n');
        }
    }
}
