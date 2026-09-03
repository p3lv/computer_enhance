# Linux build setup for part3_moving_data

Context: this project is Windows-first (built via `build.bat` with `cl`/`clang++`)
and written single-TU style — most `listing_NNNN_*.cpp` files have no `main()`
of their own; they get `#include`d by a later `listing_MMMM_..._main.cpp`.
This doc records what was set up to build and view assembly for individual
listings on Linux, so it can be redone or extended by hand later.

## 1. `./build <N>` script

New file: `build` (executable, `chmod +x`), at the project root next to
`build.bat`. Usage:

```
./build 110            # -> out/listing_0110.s
./build 110 -O0         # extra args are forwarded to clang++
```

What it does, given a listing number `N` (zero-padded to 4 digits):

1. If `linux_drivers/listing_000N_main.cpp` exists, build that (see §2).
2. Else, if `listing_000N_*.cpp` itself contains `int main`, build it directly.
3. Else, search `*_main.cpp` (project root + `linux_drivers/`) for the
   lowest-numbered one containing `#include "listing_000N_...cpp"`, and
   build that instead — that's the file that actually pulls the fragment in.

Compiles with:

```
clang++ -mavx2 -O3 -g -Wall -masm=intel -S <resolved source> -o out/listing_000N.s
```

`-S` emits textual assembly (Intel syntax) instead of an executable, which is
what you open directly to read.

To redo this by hand for a listing that isn't Windows-only: just run the
`clang++` command above yourself against whichever `_main.cpp` includes it.

## 2. `linux_drivers/` override folder

Some listings' natural `_main.cpp` can't build on Linux at all because a
fragment they include is unconditionally Windows-only (no `#if _WIN32` guard).
`linux_drivers/listing_NNNN_main.cpp` is a hand-written substitute main that
takes priority over the real one for that listing number.

Currently contains:

- **`linux_drivers/listing_0110_main.cpp`** — stand-in for listing 110
  (`WriteToAllBytes`, in `listing_0110_pagefault_overhead_test.cpp`). Its
  natural home, `listing_0111_pagefault_overhead_main.cpp`, also includes
  `listing_0106_mallocread_overhead_test.cpp`, which unconditionally
  `#include <windows.h>` and exercises Win32 `ReadFile` — no `#else` branch
  exists for it, so it flat-out can't build outside Windows.

  The driver includes the same real dependencies (`listing_0068_buffer.cpp`,
  `listing_0108_platform_metrics.cpp`, `listing_0109_pagefault_repetition_tester.cpp`,
  `listing_0110_pagefault_overhead_test.cpp`) but locally re-declares just the
  small bit of plumbing `WriteToAllBytes` actually needs (`read_parameters`,
  `allocation_type`, `HandleAllocation`/`HandleDeallocation`) instead of pulling
  in the Windows-only file. It never calls `WriteToAllBytes` (the real `main`
  never does either — it's driven by a repetition-tester loop), so it keeps
  the function alive by taking its address through a `volatile` function
  pointer (otherwise `-O3` dead-code-eliminates an unused static function).

To add another override for a different Windows-only listing later: copy this
pattern — same includes as the real chain up to (not including) the
Windows-only file, then hand-write the minimal missing types/functions.

## 3. Portability fixes applied to the actual course files

While testing the script against other listings (e.g. `./build 142`), found
and fixed genuine Linux/macOS portability bugs in the shared platform files —
these are things the course's own `#else` (non-Windows) branches were
missing, unrelated to the build script itself. Fixed directly in the source
(not via drivers, since these files are otherwise fine on Linux):

**Files:** `listing_0108_platform_metrics.cpp`, `listing_0137_os_platform.cpp`,
`listing_0163_os_platform.cpp`, `listing_0169_os_platform.cpp`

**a) Missing includes in the `#else` (non-`_WIN32`) branch:**
- `<sys/resource.h>` — for `struct rusage` / `getrusage` (all four files)
- `<fcntl.h>`, `<unistd.h>` — for `open`/`O_RDONLY`/`read`/`close`
  (0137, 0163 only — 0169 already had these)
- `<limits.h>` — for `SSIZE_MAX` (0137, 0163, 0169)

**b) Bug in `ReadOSRandomBytes`** (0137, 0163, 0169): the POSIX
implementation is declared `ReadOSRandomBytes(u64 Count, void *Dest)` but the
body called `read(DevRandom, Dest.Data, Dest.Count)` — `.Data`/`.Count` don't
exist on `void *`. Changed to `read(DevRandom, Dest, Count)` (using the
parameters directly), with the result compared against `(int64_t)Count`.

**c) Missing `LargePageSize` field** (0137, 0163, 0169): the shared
(platform-independent) function `GetLargePageSize()` reads
`GlobalOSPlatform.LargePageSize`, but that field only existed in the `_WIN32`
version of the `os_platform` struct. Added
`u64 LargePageSize; // ... 0 when large pages are not supported` to the
non-Windows struct too (zero-initialized by default via static storage
duration, which is correct since large pages aren't implemented there).

Verified fixed by rebuilding listings 142, 166 (no more errors from these
particular issues — see §4 for what's still broken beyond this).

## 4. Known remaining gap (not fixed, out of scope here)

Listings from ~163 onward that do background-thread I/O
(`listing_0163_os_platform.cpp`'s `CreateThread`/`thread_handle`/
`THREAD_ENTRY_POINT` usage, pulled in by `listing_0165_osread_revisited.cpp`,
`listing_0167_osread_sum.cpp`, `listing_0170_memory_mapped_sum.cpp`, and their
mains 166/168/171) use Win32 threading APIs with no POSIX/pthreads
equivalent implemented in the `#else` branch at all. That's a real porting
job (implement thread creation with pthreads), not a small include/typo fix,
so it was left alone. `./build 166`, `./build 168`, `./build 171` will still
fail until that's done.
