#!/bin/bash

# as per http://gendev.spritesmind.net/forum/viewtopic.php?p=12798
# (mirrored in doc/instrucitons.txt)

# any arguments are passed to make in the sub-projcets
# sadly, -j2 is a bad idea, because it makes the builds fail :(

DGCC=8.2.0
GMP=6.1.2
MPFR=3.1.2
MPC=1.0.2
BINUTILS=2.28
NEWLIB=2.5.0

export GCC_VERSION=$DGCC
export BINUTILS_VERSION=$BINUTILS
export NEWLIB_VERSION=$NEWLIB
export EXTRA_MAKE_FLAGS="$@"

function Download
{
	echo -n "  * $1 ..."
	wget -c -q $1
	echo " done"
}

function Unpack
{
	echo -n "  * $1 ..."
	tar xf $1
	echo " done"
}

set -e

echo "Making output directories"
if [ ! -d /opt/toolchains ]; then
	sudo mkdir -p /opt/toolchains
fi
sudo chmod 777 /opt/toolchains

if [ ! -d /opt/toolchains/sega/ldscripts ]; then
	mkdir -p /opt/toolchains/sega/ldscripts
fi

mkdir -p build
mkdir -p download

cd download

echo "Downloading gcc"
#Download http://ftp.gnu.org/gnu/gcc/gcc-$DGCC/gcc-core-$DGCC.tar.bz2
#Download http://ftp.gnu.org/gnu/gcc/gcc-$DGCC/gcc-g++-$DGCC.tar.bz2
Download http://ftp.gnu.org/gnu/gcc/gcc-$DGCC/gcc-$DGCC.tar.xz

echo ""
echo "Downloading dependencies"
Download https://ftp.gnu.org/gnu/mpc/mpc-$MPC.tar.gz
Download https://gmplib.org/download/gmp/gmp-$GMP.tar.bz2
Download http://www.mpfr.org/mpfr-$MPFR/mpfr-$MPFR.tar.bz2 

Download http://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS.tar.bz2
Download ftp://sources.redhat.com/pub/newlib/newlib-$NEWLIB.tar.gz

cd ../build

echo ""
echo "Unpacking"
#Unpack ../download/gcc-core-$DGCC.tar.bz2
#Unpack ../download/gcc-g++-$DGCC.tar.bz2
Unpack ../download/gcc-$DGCC.tar.xz
Unpack ../download/mpc-$MPC.tar.gz
Unpack ../download/gmp-$GMP.tar.bz2
Unpack ../download/mpfr-$MPFR.tar.bz2
Unpack ../download/binutils-$BINUTILS.tar.bz2
Unpack ../download/newlib-$NEWLIB.tar.gz

echo ""
echo "Moving and renaming"
mv mpfr-$MPFR gcc-$DGCC/mpfr
mv mpc-$MPC gcc-$DGCC/mpc
mv gmp-$GMP gcc-$DGCC/gmp

echo ""
echo "Copying ldscripts and makefiles"
cp ../ldscripts/* .

#echo "Truncating documentation files (texinfo version incompatibility workaround)"
#for f in gcc-$DGCC/gcc/doc/*.texi; do echo "" > "$f"; done

echo ""
echo "Building (see build/build.log)"
echo "Note: the script says when it finished successfully."
echo "If it terminates without a message, the build failed and you should check the log file."
date > build.log
CFLAGS="-w" make -f makefile-gen &>> build.log

echo ""
echo "Installing megadrive ldscripts etc."
cp *.ld /opt/toolchains/sega/ldscripts
cp ../sourceme.sh /opt/toolchains/sega

echo ""
echo "Done!"
echo "to import the toolchain environment use"
echo "  source /opt/toolchains/sega/sourceme.sh"
echo ""
echo "after that you can eg. try and compile the examples in examples/" 
