set -e
set -x

MAKEOBJDIRPREFIX=/home/pizlo/Programs/pizlonated-freebsd-obj make -j 32 SRCCONF=$PWD/src.conf XCC=/home/pizlo/Programs/llvm-project-deluge/build/bin/clang XCXX=/home/pizlo/Programs/llvm-project-deluge/build/bin/clang++ XCPP=/home/pizlo/Programs/llvm-project-deluge/build/bin/clang-cpp buildworld | tee build_filbsd_log.txt 2>&1


