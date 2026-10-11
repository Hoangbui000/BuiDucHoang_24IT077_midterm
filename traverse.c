#include "ls.h"

void
traverse_directory(const char *dirpath, unsigned int flags, int print_header)
{
    DIR *dir = opendir(dirpath);
    if (dir == NULL) {
        fprintf(stderr, "ls: %s: %s\n", dirpath, strerror(errno));
        g_exit_status = 1;
        return;
    }

    if (print_header) {
        printf("%s:\n", dirpath);
    }

    FileEntry *entries = NULL;
    size_t count = 0;
    size_t capacity = 0;
    struct dirent *dp;

    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_name[0] == '.') {
            if (!(flags & (FLAG_a | FLAG_A)))
                continue;
            if ((flags & FLAG_A) && !(flags & FLAG_a)) {
                if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
                    continue;
            }
        }

        if (count >= capacity) {
            capacity = (capacity == 0) ? 32 : capacity * 2;
            FileEntry *new_entries = realloc(entries, capacity * sizeof(FileEntry));
            if (new_entries == NULL) {
                perror("ls: realloc");
                closedir(dir);
                free(entries);
                g_exit_status = 1;
                return;
            }
            entries = new_entries;
        }

        entries[count].name = strdup(dp->d_name);
        char fullpath[1024];
        if (strcmp(dirpath, "/") == 0)
            snprintf(fullpath, sizeof(fullpath), "/%s", dp->d_name);
        else
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp->d_name);

        entries[count].path = strdup(fullpath);
        if (lstat(entries[count].path, &entries[count].sb) == -1) {
            entries[count].error = errno;
            g_exit_status = 1;
        } else {
            entries[count].error = 0;
        }
        count++;
    }
    closedir(dir);

    if (!(flags & FLAG_f) && count > 0) {
        set_cmp_flags(flags);
        qsort(entries, count, sizeof(FileEntry), cmp_entries);
    }

    print_entries(entries, count, flags, 1);

    if (flags & FLAG_R) {
        for (size_t i = 0; i < count; i++) {
            if (entries[i].error == 0 && S_ISDIR(entries[i].sb.st_mode)) {
                if (strcmp(entries[i].name, ".") != 0 && strcmp(entries[i].name, "..") != 0) {
                    putchar('\n');
                    traverse_directory(entries[i].path, flags, 1);
                }
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(entries[i].name);
        free(entries[i].path);
    }
    free(entries);
}
