# Midterm Project Report: Implementation of UNIX ls(1)

- **Course:** Advanced Programming in the UNIX Environment (System Programming)
- **Instructor:** Dr. Nguyen Nhat An
- **Student Name:** Bui Duc Hoang
- **Student ID:** 24IT077
- **Target Platform:** NetBSD 10.1 (amd64)
- **Repository URL:** [https://github.com/Hoangbui000/BuiDucHoang_24IT077_midterm](https://github.com/Hoangbui000/BuiDucHoang_24IT077_midterm)

---

## 1. Project Overview & Objectives

This project implements a simplified, POSIX-compliant version of the UNIX `ls(1)` command from scratch in C on NetBSD 10.1. The utility strictly conforms to the provided manual page specification:

    ls [-AacdFfhiklnqRrSstuw] [file ...]

### Core Engineering Goals
- **Modular Architecture:** Decouple program logic across multiple `.c` source files and `.h` header files for high cohesion, maintainability, and clean separation of concerns.
- **System-Level Robustness:** Prevent segmentation faults, buffer overflows, file descriptor leaks, and memory leaks across all edge cases (such as broken symbolic links, permission-denied directories, non-existent operands, and deep directory recursion).
- **Strict Compilation Standards:** Compile cleanly using BSD `make` with strict warning flags (`-Wall -Wextra -Werror`) and link against system utilities (`-lutil`).

---

## 2. Modular Architecture & System Design

Instead of a single monolithic source file, the project is structured into 6 distinct modules:

| File | Primary Responsibility & Key System APIs Used |
| :--- | :--- |
| **`ls.h`** | Declares the 19 option bitmask macros (`FLAG_A` through `FLAG_w`), the `FileEntry` data structure, global exit status `g_exit_status`, and module function prototypes. |
| **`main.c`** | Parses command-line options via `getopt(3)`, enforces flag mutual exclusivity and precedence rules, detects terminal output via `isatty(3)` and superuser privileges via `geteuid(2)`, separates plain file operands from directory operands, and dispatches execution. |
| **`cmp.c`** | Implements comparator callbacks for `qsort(3)`. Manages lexicographical sorting (`strcmp`), descending file size sorting (`-S`), timestamp sorting (`-t` combined with `-c` or `-u`), and reverse order (`-r`). |
| **`print.c`** | Renders standard single-column output, calculates `total` allocated filesystem blocks, and formats detailed long listings (`-l`, `-n`) including permissions, hard link counts, UID/GID resolution (`getpwuid`, `getgrgid`), device numbers (`major`/`minor`), human-readable sizes (`humanize_number`), and symlink targets (`readlink`). |
| **`traverse.c`** | Manages POSIX directory streams (`opendir`, `readdir`, `closedir`), filters hidden dotfiles based on `-a`/`-A`, dynamically expands entry arrays via `realloc(3)`, queries metadata using `lstat(2)`, and performs recursive subdirectory traversal (`-R`). |
| **`util.c`** | Provides helper functions to construct the 10-character file mode string (including `setuid`, `setgid`, and `sticky` bits), append file classification suffixes (`-F`), mask non-printable characters (`-q` vs `-w`), and parse the `BLOCKSIZE` environment variable. |

---

## 3. Detailed Feature Implementation & Flag Precedence

The program implements all 19 options specified in the manual page, carefully handling flag interactions and mutual overrides:

### 3.1. Directory Filtering & Traversal Options
- **`-a` and `-A`:** By default, filenames starting with a dot (`.`) are hidden. `-a` includes all dotfiles as well as `.` and `..`. `-A` includes all dotfiles except `.` and `..` (automatically enabled when running as root where `geteuid() == 0`).
- **`-d` vs `-R`:** `-d` lists directories themselves as plain files rather than inspecting their contents, whereas `-R` recursively lists all subdirectories encountered. These two flags mutually override each other based on whichever appears last on the command line.
- **Infinite Recursion Prevention (`-R`):** During recursive directory traversal, the engine explicitly filters out `.` and `..` entries before descending into child directories, preventing infinite loops and call-stack exhaustion.

### 3.2. Sorting & Timestamp Options
- **Default Operand & Entry Ordering:** Entries are sorted alphabetically using `qsort(3)`. When multiple command-line arguments are supplied, non-directory files are sorted and displayed first, followed by directory operands and their contents.
- **`-f` (Unsorted):** Disables sorting to print directory entries in raw filesystem directory-stream order, automatically enabling `-a` and overriding `-t`, `-S`, and `-r`.
- **`-S` vs `-t`:** `-S` sorts entries by file size (`st_size`) from largest to smallest; `-t` sorts entries by timestamp from newest to oldest. Whichever flag appears later on the command line overrides the earlier one. If two files share the exact same size or timestamp, the comparator falls back to alphabetical order (`strcmp`) for deterministic output.
- **`-c` vs `-u`:** Modifies the timestamp field used by `-t` (for sorting) and `-l` (for display). `-c` selects the last status change time (`st_ctime`), while `-u` selects the last access time (`st_atime`).
- **`-r` (Reverse):** Reverses the active sorting order (lexicographical, size, or time).

### 3.3. Output Formatting & Block Calculation Options
- **`-l` vs `-n`:** Both flags enable the detailed long listing format (file type, permissions, hard links, owner, group, size or device numbers, timestamp, and filename). `-l` resolves numeric IDs into user and group names via `getpwuid(3)` and `getgrgid(3)` (falling back to numeric values if an ID is unmapped), whereas `-n` forces numeric UID and GID output.
- **`-h` vs `-k`:** Mutually exclusive size formatting flags. `-h` scales file sizes into human-readable units (Bytes, `K`, `M`, `G`) using NetBSD's `humanize_number(3)` library function (`-lutil`). `-k` reports file sizes in 1024-byte kilobytes.
- **`-s` and `BLOCKSIZE`:** Displays the actual number of filesystem blocks allocated to each file, along with a `total` block summary header at the top of directory listings. It respects the `BLOCKSIZE` environment variable (defaulting to 512-byte blocks if unset or invalid).
- **`-i`:** Prepends each file's filesystem inode serial number (`st_ino`).
- **`-F`:** Appends a file-type classification indicator directly after the filename: `/` for directories, `*` for executable files, `@` for symbolic links, `=` for sockets, and `|` for FIFOs.
- **`-q` vs `-w`:** Controls how non-printable characters in filenames are displayed. When standard output is a terminal (`isatty(STDOUT_FILENO)`), `-q` is enabled by default to mask non-printable bytes with `?` to protect terminal emulators from control-sequence injection. When output is redirected or piped, `-w` (raw printing) is used by default.

---

## 4. Compilation & Usage Guide

### 4.1. Building the Project
Run BSD `make` in the root directory of the repository to compile all `.c` source files into `.o` object files and link the `ls` executable:

    make

To remove all compiled object files (`*.o`), binaries (`ls`, `a.out`), and core dumps (`*.core`):

    make clean

### 4.2. Example Commands

    # 1. Default listing of the current working directory
    ./ls

    # 2. Long listing with hidden files, classification suffixes, and human-readable sizes
    ./ls -laFh

    # 3. Sort files by size in ascending order (size sort combined with reverse flag)
    ./ls -lSr

    # 4. Recursive directory traversal with inode numbers on /tmp
    ./ls -laiR /tmp

    # 5. Long listing with numeric UIDs/GIDs and allocated block counts
    ./ls -ns

---

## 5. Experimental Verification & System Analysis

### Test Case 1: Modular Compilation & Default Directory Streaming (`./ls`)
- **Execution Mechanism:** Running `make` compiles each source module with `-Wall -Wextra -Werror` and links them with `-lutil`. Executing `./ls` without arguments opens `.` via `opendir(".")`, iterates over directory entries via `readdir()`, skips hidden dotfiles, sorts visible filenames alphabetically with `qsort()`, and prints one entry per line.
- **System Significance:** Demonstrates clean modular compilation and standard POSIX directory stream iteration using `struct dirent`.

### Test Case 2: Detailed Metadata, Symlinks & Human-Readable Formatting (`./ls -laFh`)
- **Execution Mechanism:** Displays the `total` allocated disk blocks followed by detailed metadata for every entry (including `.` and `..`), human-scaled byte sizes, and classification suffixes (`./`, `ls*`).
- **System Significance:** Uses `lstat(2)` rather than `stat(2)` so that symbolic links are inspected directly without dereferencing their targets. This prevents broken symbolic links from causing runtime errors and allows `readlink(2)` to display `-> target_path` in long listings.

### Test Case 3: Size Sorting & Reverse Comparator Logic (`./ls -lSr`)
- **Execution Mechanism:** Enables `-S` (sort by `st_size` descending) together with `-r` (reverse order), producing an ascending size listing where the smallest file appears first.
- **System Significance:** Validates custom `qsort(3)` comparator callbacks in `cmp.c` and deterministic alphabetical tie-breaking when multiple files have identical byte sizes.

### Test Case 4: Inode Numbers & Safe Recursive Traversal (`./ls -laiR /tmp`)
- **Execution Mechanism:** Prints each entry's unique inode number (`st_ino`) and recursively traverses child directories while skipping `.` and `..`.
- **System Significance:** Highlights that UNIX directories are mappings of string filenames (hard links) to underlying filesystem inodes, and verifies that directory descriptors are properly closed (`closedir`) and dynamically allocated heap buffers are freed (`free`) at every recursion depth.

### Test Case 5: Error Handling & POSIX Exit Status (`./ls nonexistent_file`)
- **Execution Mechanism:** Passing a non-existent path causes `lstat(2)` to return `-1` and set `errno` to `ENOENT`. The program reports `ls: nonexistent_file: No such file or directory` to `stderr` via `strerror(errno)` and sets `g_exit_status = 1`, which is verified by running `echo $?`.
- **System Significance:** Adheres strictly to POSIX utility standards (exiting with `0` on complete success and `>0` if any error occurs), allowing external shell scripts and pipelines to detect execution failures reliably.

---

## 6. Conclusion
The midterm project fulfills all structural and functional requirements outlined in the course specification:
1. Complete support for all 19 options specified in the NetBSD 10.1 manual page.
2. High-quality modular architecture divided across distinct `.c` and `.h` files.
3. Safe memory allocation and proper descriptor management with zero segmentation faults.
4. An automated `Makefile` with strict flags (`-Wall -Wextra -Werror`) and `clean` targets.
5. Full version control history hosted on GitHub with unwanted build artifacts excluded via `.gitignore`.
