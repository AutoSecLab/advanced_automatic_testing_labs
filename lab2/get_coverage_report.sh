#!/bin/bash

echo "[START PROFILE COLLECTION]"

mkdir profile

TARGET=xmllint_profile
i=0
for s in `ls out/default/queue/`; do
    echo "seed $i: $s"
    LLVM_PROFILE_FILE="./profile/${TARGET}-${i}.profraw" ./${TARGET} ./out/default/queue/$s > /dev/null 2>&1
    i=$((i + 1))
done

llvm-profdata merge -sparse ./profile/*.profraw -o ${TARGET}.profdata
llvm-cov show ./xmllint_profile -instr-profile=${TARGET}.profdata  -format=text > show

echo "[END PROFILE COLLECTION]"