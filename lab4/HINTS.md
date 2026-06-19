# Some hints

These are some hints on how to compile the library and the driver.

For debug (with AddressSanitizer):
```bash
cd ${WORKDIR}/libmsg
clang++ -g -O0 -fsanitize=address -fno-omit-frame-pointer \
    driver.cc msgparser.c -I. -o msg_debug
```

For fuzzing:
```bash
cd ${WORKDIR}/libmsg
clang++ -g -O1 -fsanitize=address,fuzzer driver.cc msgparser.c -I. -o msg_fuzz
```

For LLVM-Cov:
```bash
cd ${WORKDIR}/libmsg
clang++ -g -O0 -fprofile-instr-generate -fcoverage-mapping -fsanitize=fuzzer \
    driver.cc msgparser.c -I. -o msg_coverage
```

## Coverage from a crashing seed

The standard `msg_coverage` binary (built with `-fsanitize=fuzzer`) cannot capture
coverage when the input triggers a crash: libFuzzer calls `_exit()` before LLVM's
atexit-registered `__llvm_profile_write_file` runs.

The workaround is a standalone driver that:
1. uses AddressSanitizer (which intercepts the overflow cleanly before the heap
   metadata is corrupted), and
2. registers `__asan_set_death_callback` so the profile is flushed just before
   ASan's own `_exit()`.

### 1. Build the standalone coverage driver

```bash
cd ${WORKDIR}/libmsg
clang++ -g -O0 -fsanitize=address \
    -fprofile-instr-generate -fcoverage-mapping \
    cov_driver.cc msgparser.c -I. -o msg_cov_asan
```

`cov_driver.cc` is a minimal `main()` that reads a seed file, calls the parser,
and registers `__asan_set_death_callback` to flush the profile on crash.

### 2. Run against the crash seed

```bash
LLVM_PROFILE_FILE="crash.profraw" \
ASAN_OPTIONS="detect_leaks=0" \
    ./msg_cov_asan crash_seed
```

The run will print an ASan heap-buffer-overflow report and end with
`==...==ABORTING`, but `crash.profraw` will be non-empty.

### 3. Generate the coverage report

```bash
llvm-profdata merge -sparse crash.profraw -o crash.profdata

# Line-level annotated source
llvm-cov show ./msg_cov_asan -instr-profile=crash.profdata --sources=msgparser.c

# Summary table
llvm-cov report ./msg_cov_asan -instr-profile=crash.profdata --sources=msgparser.c
```
