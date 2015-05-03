#!/bin/zsh

TOP_LEVEL=../

CONTRIB=$PWD
PACKAGE_VERSION=4.2.0
PACKAGE_FILE=papi-${PACKAGE_VERSION}.tar.gz
PACKAGE_URL=http://icl.cs.utk.edu/projects/papi/downloads/${PACKAGE_FILE}
PACKAGE_SRC=build/papi-${PACKAGE_VERSION}

PACKAGE_CONFIGURE=( --with-pcl=yes --with-pcl-incdir=/lib/modules/$(uname -r)/build/include/linux )
PACKAGE_OPTS=( -j8 )

echo @@@ Build papi
set -e
install -dv $TOP_LEVEL build download

if [ ! -r download/${PACKAGE_FILE} ] ; then
  wget ${PACKAGE_URL} -P download
fi

tar -xzf download/${PACKAGE_FILE} -C build
( cd ${PACKAGE_SRC}/src ; ./configure --prefix=${CONTRIB} ${PACKAGE_CONFIGURE}  ; make ${PACKAGE_OPTS} && make install )

rm -rf ${PACKAGE_SRC}
touch build/papi.stamp

echo @@@ Done papi