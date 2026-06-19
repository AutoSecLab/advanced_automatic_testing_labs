#include "msgparser.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" void __asan_set_death_callback(void (*callback)(void));
extern "C" int __llvm_profile_write_file(void);

static void death_callback() {
    __llvm_profile_write_file();
}

int main(int argc, char **argv) {
    __asan_set_death_callback(death_callback);

    if (argc < 2) { fprintf(stderr, "usage: %s <seed>\n", argv[0]); return 1; }

    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen"); return 1; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    uint8_t *buf = (uint8_t *)malloc(sz);
    fread(buf, 1, sz, f);
    fclose(f);

    // this is the original driver
    msg_t msg = {};
    if (msg_parse(buf, sz, &msg) == 0) {
        msg_process(&msg);
        msg_free(&msg);
    }
    
    free(buf);
    __llvm_profile_write_file();
    return 0;
}
