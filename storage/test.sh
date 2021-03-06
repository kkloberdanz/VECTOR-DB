make clean || exit 1
make debug EXTRA="-DKT_TEST_MMAP" || exit 1
valgrind --track-origins=yes ./a.out
if [[ $? -eq 0 ]]; then
    printf "file: \033[92mSUCCESS\033[0m\n"
    hd worksheetsdb/*.db
    exit 0
else
    printf "file: \033[91mFAILURE\033[0m\n"
    exit 1
fi
