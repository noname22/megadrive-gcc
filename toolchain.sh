#!/bin/bash

# as per http://gendev.spritesmind.net/forum/viewtopic.php?p=12798
# (mirrored in doc/instrucitons.txt)

DGCC=4.5.2

GMP=5.0.1
MPFR=2.4.2
MPC=0.8.2
BINUTILS=2.21.1
NEWLIB=1.19.0

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

if [ ! -d /opt/toolchains/gen/ldscripts ]; then
	mkdir -p /opt/toolchains/gen/ldscripts
fi

mkdir -p build
mkdir -p download

cd download

echo "Downloading gcc"
Download http://ftp.gnu.org/gnu/gcc/gcc-$DGCC/gcc-core-$DGCC.tar.bz2
Download http://ftp.gnu.org/gnu/gcc/gcc-$DGCC/gcc-g++-$DGCC.tar.bz2

echo ""
echo "Downloading dependencies"
Download http://www.multiprecision.org/mpc/download/mpc-$MPC.tar.gz
Download ftp://ftp.gmplib.org/pub/gmp-5.0.1/gmp-$GMP.tar.bz2
Download http://www.mpfr.org/mpfr-2.4.2/mpfr-$MPFR.tar.bz2 

Download http://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS.tar.bz2
Download ftp://sources.redhat.com/pub/newlib/newlib-$NEWLIB.tar.gz

cd ../build

echo ""
echo "Unpacking"
Unpack ../download/gcc-core-$DGCC.tar.bz2
Unpack ../download/gcc-g++-$DGCC.tar.bz2
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

echo "Truncating documentation files (texinfo version incompatibility workaround)"
for f in gcc-$DGCC/gcc/doc/*.texi; do echo "" > "$f"; done

echo ""
echo "Building (see build/build.log)"
date > build.log
CFLAGS="-w" make -f makefile-gen >> build.log

echo ""
echo "Installing megadrive ldscripts etc."
cp *.ld /opt/toolchains/gen/ldscripts
cp ../sourceme.sh /opt/toolchains/gen

echo ""
echo "Done!"
echo "to import the toolchain environment use"
echo "  source /opt/toolchains/gen/sourceme.sh"
echo ""
echo "after that you can eg. try and compile the examples in examples/" 
