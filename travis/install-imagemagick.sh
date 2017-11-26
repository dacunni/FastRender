#!/bin/sh
#VERSION=6.9.3-1
#BASENAME=ImageMagick-${VERSION}.tar.gz
# Latest
BASENAME=ImageMagick
TARBALL=${BASENAME}.tar.gz
set -ex
wget http://www.imagemagick.org/download/${TARBALL}
tar -xzvf ${TARBALL}
cd ${BASENAME} && ./configure --prefix=/usr && make && sudo make install

