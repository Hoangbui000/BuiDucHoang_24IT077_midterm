# Midterm Project: Implement ls(1)

- **Student Name:** Bui Duc Hoang
- **Student ID:** 24IT077
- **Course:** Advanced Programming in the UNIX Environment (CS631)
- **Repository:** https://github.com/Hoangbui000/BuiDucHoang_24IT077_midterm

---

## 1. Project Overview
This project implements a simplified version of the UNIX `ls(1)` utility from scratch on NetBSD 10.1, fully conforming to the project specification and manual page requirements.

## 2. Implemented Features & Options
The utility implements the synopsis `ls [-AacdFfhiklnqRrSstuw] [file ...]`:
- `-A`: List all entries except for `.` and `..` (default for root user).
- `-a`: Include directory entries whose names begin with a dot (`.`).
- `-c`: Use status change time (`ctime`) for sorting (`-t`) or printing (`-l`).
- `-d`: List directories as plain files rather than listing their contents.
- `-F`: Append suffixes (`/`, `*`, `@`, `=`, `|`) to entry names based on file type.
- `-f`: Output is unsorted; automatically enables `-a`.
- `-h`: Human-readable size formatting via NetBSD `humanize_number(3)`.
- `-i`: Display file serial numbers (inode numbers).
- `-k`: Report sizes in kilobytes.
- `-l`: Long listing format displaying mode, link count, owner, group, size/devices, timestamp, and target names.
- `-n`: Same as `-l`, but displays owner and group IDs numerically.
- `-q`: Replace non-printable characters with `?` (default output to terminal).
- `-R`: Recursively traverse subdirectories encountered.
- `-r`: Reverse the sorting order.
- `-S`: Sort directory entries by file size descending.
- `-s`: Display the number of file system blocks actually allocated.
- `-t`: Sort entries by timestamp descending.
- `-u`: Use last access time (`atime`) for sorting (`-t`) or printing (`-l`).
- `-w`: Force raw printing of non-printable characters.

## 3. Modular Source Architecture
The implementation is organized into modular `.c` and `.h` translation units:
- `ls.h`: Header file declaring bitmask flags, the `FileEntry` container, and public prototypes.
- `main.c`: Command-line parsing with `getopt(3)`, mutual exclusivity handling, and dispatching non-directory vs directory operands.
- `cmp.c`: Comparison routines passed to `qsort(3)` handling sorting criteria.
- `print.c`: Formatting module generating standard lines, total block sums, and long-format lines.
- `traverse.c`: Directory traversal (`opendir`, `readdir`, `closedir`), hidden file filters, and recursion (`-R`).
- `util.c`: Helper routines for file modes, classification suffixes, environment variable parsing (`BLOCKSIZE`), and timestamps.

## 4. Compilation and Usage
To compile the project:
```sh
make
