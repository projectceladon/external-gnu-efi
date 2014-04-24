#
# This script is used to generate gnu_efi prebuilts for both ia32 and x86_64.
# The resulting binaries will be copied into prebuilts/{ia32, x86_64}.
#
# Please make sure you have Android's build system setup first, and lunch
# target defined.
#
# Specify "-a" in command line to add these prebuilt binaries for
# git commit.
#
# Note:
# 1. ARCH ia32 and x86 are interchangable here.
#    Android uses x86, but EFI uses ia32.
#

PREBUILT_TOP=$ANDROID_BUILD_TOP/hardware/intel/efi_prebuilts/

set -e

pushd gnu-efi-3.0

copy_to_prebuilts()
{
    DEST_DIR=$PREBUILT_TOP/gnu-efi/linux-$2/

    # Sanity check
    if [ ! -s "gnuefi/crt0-efi-$1.o" ] ; then
        echo "[ERROR] *** $1: gnuefi/crt0-efi-$1.o does not exist or has size 0. aborting..."
        exit 1
    fi
    if [ ! -s "gnuefi/libgnuefi.a" ] ; then
        echo "[ERROR] *** $1: gnuefi/libgnuefi.a does not exist or has size 0. aborting..."
        exit 1
    fi
    if [ ! -s "lib/libefi.a" ] ; then
        echo "[ERROR] *** $1: lib/libefi.a does not exist or has size 0. aborting..."
        exit 1
    fi

    cp -v gnuefi/crt0-efi-$1.o $DEST_DIR/lib/crt0-efi-$1.o
    cp -v gnuefi/libgnuefi.a $DEST_DIR/lib/libgnuefi.a
    cp -v gnuefi/elf_$1_efi.lds $DEST_DIR/lib/elf_$1_efi.lds
    cp -v lib/libefi.a $DEST_DIR/lib/libefi.a

    cp -v inc/*.h $DEST_DIR/include/efi/
    cp -v inc/$1/*.h $DEST_DIR/include/efi/$1/
    cp -v inc/protocol/*.h $DEST_DIR/include/efi/protocol/
}

add_prebuilts=0
while getopts "a" opt; do
    case "$opt" in
        a) add_prebuilts=1;;
    esac
done

# Create prebuilts directory (if not already exists)
mkdir -p $PREBUILT_TOP/gnu-efi/linux-x86/include/efi/
mkdir -p $PREBUILT_TOP/gnu-efi/linux-x86/include/efi/ia32
mkdir -p $PREBUILT_TOP/gnu-efi/linux-x86/include/efi/protocol
mkdir -p $PREBUILT_TOP/gnu-efi/linux-x86/lib
mkdir -p $PREBUILT_TOP/gnu-efi/linux-x86_64/include/efi/
mkdir -p $PREBUILT_TOP/gnu-efi/linux-x86_64/include/efi/x86_64
mkdir -p $PREBUILT_TOP/gnu-efi/linux-x86_64/include/efi/protocol
mkdir -p $PREBUILT_TOP/gnu-efi/linux-x86_64/lib

make ARCH=x86_64 clean
make ARCH=ia32 clean

# Generate prebuilts for x86_64
make ARCH=x86_64
copy_to_prebuilts x86_64 x86_64
make ARCH=x86_64 clean

# Generate prebuilts for ia32
make ARCH=ia32
copy_to_prebuilts ia32 x86
make ARCH=ia32 clean

if [ "$add_prebuilts" == "1" ]; then
    export GIT_DIR=$PREBUILT_TOP/gnu-efi/.git
    export GIT_WORK_TREE=$PREBUILT_TOP/gnu-efi

    git add -- linux-x86/*
    git add -- linux-x86_64/*

    unset GIT_DIR
    unset GIT_WORK_TREE

    echo "[NOTICE] Please remember to commit the prebuilts under $PREBUILT_TOP"
fi

popd
