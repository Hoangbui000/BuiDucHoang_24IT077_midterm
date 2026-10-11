#include "ls.h"

extern void set_cmp_flags(unsigned int flags);

int
main(int argc, char *argv[])
{
    unsigned int flags = 0;
    int ch;

    /* Mc nh terminal bt -q */
    if (isatty(STDOUT_FILENO)) {
        flags |= FLAG_q;
    } else {
        flags |= FLAG_w;
    }

    /* Super-user mc nh bt -A theo man page */
    if (geteuid() == 0) {
        flags |= FLAG_A;
    }

    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
        case 'A':
            flags |= FLAG_A;
            break;
        case 'a':
            flags |= FLAG_a;
            break;
        case 'c':
            flags &= ~FLAG_u;
            flags |= FLAG_c;
            break;
        case 'd':
            flags &= ~FLAG_R;
            flags |= FLAG_d;
            break;
        case 'F':
            flags |= FLAG_F;
            break;
        case 'f':
            flags |= FLAG_f;
            flags |= FLAG_a;
            flags &= ~(FLAG_t | FLAG_S | FLAG_r);
            break;
        case 'h':
            flags &= ~FLAG_k;
            flags |= FLAG_h;
            break;
        case 'i':
            flags |= FLAG_i;
            break;
        case 'k':
            flags &= ~FLAG_h;
            flags |= FLAG_k;
            break;
        case 'l':
            flags &= ~FLAG_n;
            flags |= FLAG_l;
            break;
        case 'n':
            flags &= ~FLAG_l;
            flags |= FLAG_n;
            flags |= FLAG_l; /* -n k tha nh dng di ca -l */
            break;
        case 'q':
            flags &= ~FLAG_w;
            flags |= FLAG_q;
            break;
        case 'R':
            flags &= ~FLAG_d;
            flags |= FLAG_R;
            break;
        case 'r':
            flags |= FLAG_r;
            break;
        case 'S':
            flags &= ~FLAG_t;
            flags |= FLAG_S;
            break;
        case 's':
            flags |= FLAG_s;
            break;
        case 't':
            flags &= ~FLAG_S;
            flags |= FLAG_t;
            break;
        case 'u':
            flags &= ~FLAG_c;
            flags |= FLAG_u;
            break;
        case 'w':
            flags &= ~FLAG_q;
            flags |= FLAG_w;
            break;
        default:
            fprintf(stderr, "usage: ls [-AacdFfhiklnqRrSstuw] [file ...]\n");
            exit(EXIT_FAILURE);
        }
    }

    argc -= optind;
    argv += optind;

    /* Khng truyn tham s -> lit k th mc hin ti */
    if (argc == 0) {
        if (flags & FLAG_d) {
            FileEntry ent;
            ent.name = ".";
            ent.path = ".";
            if (lstat(".", &ent.sb) == -1) {
                perror("ls: .");
                return EXIT_FAILURE;
            }
            ent.error = 0;
            print_entries(&ent, 1, flags, 0);
        } else {
            traverse_directory(".", flags, 0);
        }
        return EXIT_SUCCESS;
    }

    /* Thu thp cc i s c truyn vo */
    FileEntry *files = malloc(argc * sizeof(FileEntry));
    FileEntry *dirs = malloc(argc * sizeof(FileEntry));
    size_t file_count = 0;
    size_t dir_count = 0;

    for (int i = 0; i < argc; i++) {
        struct stat sb;
        if (lstat(argv[i], &sb) == -1) {
            fprintf(stderr, "ls: %s: %s\n", argv[i], strerror(errno));
            continue;
        }

        if ((flags & FLAG_d) || !S_ISDIR(sb.st_mode)) {
            files[file_count].name = argv[i];
            files[file_count].path = argv[i];
            files[file_count].sb = sb;
            files[file_count].error = 0;
            file_count++;
        } else {
            dirs[dir_count].name = argv[i];
            dirs[dir_count].path = argv[i];
            dirs[dir_count].sb = sb;
            dirs[dir_count].error = 0;
            dir_count++;
        }
    }

    /* Sp xp file v th mc ring bit nu khng bt -f */
    if (!(flags & FLAG_f)) {
        set_cmp_flags(flags);
        if (file_count > 0)
            qsort(files, file_count, sizeof(FileEntry), cmp_entries);
        if (dir_count > 0)
            qsort(dirs, dir_count, sizeof(FileEntry), cmp_entries);
    }

    /* In cc non-directory operand trc */
    if (file_count > 0) {
        print_entries(files, file_count, flags, 0);
    }

    /* In cc directory operand sau */
    for (size_t i = 0; i < dir_count; i++) {
        if (file_count > 0 || dir_count > 1 || (flags & FLAG_R)) {
            if (i > 0 || file_count > 0)
                putchar('\n');
            traverse_directory(dirs[i].path, flags, 1);
        } else {
            traverse_directory(dirs[i].path, flags, 0);
        }
    }

    free(files);
    free(dirs);
    return EXIT_SUCCESS;
}
