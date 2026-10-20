# Alternative Thread-Local Storage (TLS)

## Executive Summary

This repository implements an educational alternative to compiler-provided thread-local storage (TLS). It demonstrates how each POSIX thread can be given a separate memory area and access selected variables through an explicit interface modeled after the Linux kernel's per-CPU memory concept.

The implementation uses C, `pthread`, anonymous memory mappings, the x86-64 GS segment base, and GCC-style inline assembly. It is architecture- and operating-system-specific and should be treated as a low-level systems programming demonstration rather than a portable TLS library.

## Technical Scope & Key Findings

### Macro interface

The code exposes four macros that define the storage layout and access operations:

- `PER_THREAD_MEMORY_START` — begins the declaration of the structure used to lay out thread-local data.
- `PER_THREAD_MEMORY_END` — closes that structure declaration.
- `READ_THREAD_VARIABLE(name)` — reads a named field using its offset relative to the thread's GS base.
- `WRITE_THREAD_VARIABLE(name, value)` — writes a value to a named field using the same GS-relative addressing scheme.

The example declares fields `i` and `j`, creates two POSIX threads, maps an area for each thread with `mmap`, and sets each thread's GS base with `arch_prctl(ARCH_SET_GS, ...)`. Each thread writes and prints its own values to demonstrate isolation between the two areas.

### Implementation mechanism

The macro interface relies on:

- `pthread_create` for thread creation.
- `mmap` with `MAP_ANONYMOUS | MAP_PRIVATE` for per-thread storage.
- `arch_prctl` and `ARCH_SET_GS` to set the GS base on Linux x86-64.
- `offsetof` to calculate each field's byte offset.
- GCC statement expressions, `typeof`, and inline x86-64 assembly to access memory through GS-relative addresses.

## Repository Contents

| Path | Role |
|---|---|
| `main.c` | Macro definitions, example thread storage structure, per-thread routine, and `main`. |
| `CMakeLists.txt` | CMake target definition for the `TLS` executable. It currently lists `TLS.h` as a source input. |
| `README.md` | Original assignment description. |

**Build note:** the current repository file listing does not include `TLS.h`, although `CMakeLists.txt` references it. The direct GCC command below avoids that stale or missing CMake source entry.

## Requirements & Environment

- Linux on an x86-64 architecture.
- GCC or a compatible compiler supporting GNU C extensions and x86-64 inline assembly.
- POSIX threads and Linux `mmap` / `arch_prctl` interfaces.

This code cannot be compiled or run as-is on Windows, macOS, or non-x86-64 architectures. The compiler extensions used by the macros are not standard portable C11 features.

## Build & Run

From the repository root, compile the source directly with GCC:

```bash
gcc -std=gnu11 -Wall -Wextra -pthread main.c -o TLS
```

Run the executable:

```bash
./TLS
```

The program prints the values read by each thread. The current `main` calls `pause()` after creating the threads, so the process remains suspended after the example threads finish; terminate it manually with `Ctrl+C` when you have collected the output.

To use CMake instead, restore the missing `TLS.h` if it is intended to be part of the project, or update `CMakeLists.txt` so the target lists only files that exist in the checkout. A CMake configure/build command is not provided because the current source list is inconsistent with the checked-in files.

## Validation & Expected Behavior

The sample assigns different values to `i` and `j` in the two worker threads. The expected demonstration is that each thread reads back values from its own GS-relative memory area rather than from a single shared set of variables. Thread output order is nondeterministic, so the lines may appear in either order.

## Academic Context

The original assignment describes the goal as providing an alternative per-thread memory interface, with semantics analogous to the Linux kernel's per-CPU memory facilities.
