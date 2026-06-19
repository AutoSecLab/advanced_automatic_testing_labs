/*
 * driver.cc -- libFuzzer harness for msgparser
 *
 * Compile (fuzzing):
 *   clang++ -g -O1 -fsanitize=address,fuzzer \
 *       driver.cc msgparser.c -I. -o msg_fuzz
 *
 * Compile (coverage):
 *   clang++ -g -O0 -fprofile-instr-generate -fcoverage-mapping \
 *       -fsanitize=fuzzer \
 *       driver.cc msgparser.c -I. -o msg_coverage
 *
 * Run a single seed without fuzzing:
 *   ./msg_fuzz path/to/seed
 */

#include "msgparser.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    msg_t msg = {};
    if (msg_parse(data, size, &msg) == 0) {
        msg_process(&msg);
        msg_free(&msg);
    }
    return 0;
}
