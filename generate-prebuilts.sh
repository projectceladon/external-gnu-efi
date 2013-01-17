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

copy_to_prebuilts()
{
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

    cp gnuefi/crt0-efi-$1.o prebuilts/$1/crt0-efi-$1.o
    cp gnuefi/libgnuefi.a prebuilts/$1/libgnuefi.a
    cp lib/libefi.a prebuilts/$1/libefi.a
}

add_prebuilts=0
while getopts "a" opt; do
    case "$opt" in
        a) add_prebuilts=1;;
    esac
done

# Clean up everything and create prebuilts directory
rm -rf prebuilts
mkdir -p prebuilts/ia32
mkdir -p prebuilts/x86_64

make ARCH=x86_64 clean
make ARCH=ia32 clean

# Generate prebuilts for x86_64
make ARCH=x86_64 CC=$ANDROID_BUILD_TOP/prebuilts/gcc/linux-x86/host/x86_64-linux-glibc2.7-4.6/bin/x86_64-linux-gcc
copy_to_prebuilts x86_64
make ARCH=x86_64 clean

# Generate prebuilts for ia32
make ARCH=ia32 CC=$ANDROID_BUILD_TOP//prebuilts/gcc/linux-x86/host/i686-linux-glibc2.7-4.6/bin/i686-linux-gcc
copy_to_prebuilts ia32
make ARCH=ia32 clean

if [ "$add_prebuilts" == "1" ]; then
    git add -- prebuilts/
fi