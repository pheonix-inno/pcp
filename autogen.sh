#!/bin/sh

# clear up the mess
set -x

find . -name Makefile -exec rm {} \;
find . -name Makefile.in -exec rm {} \;
find . -name "*~"     -exec rm {} \;
find . -name config.h -exec rm {} \;
find . -name stamp.h  -exec rm {} \;
find . -name stamp-h1  -exec rm {} \;
find . -name .deps    -exec rm -rf {} \;
find . -name .libs    -exec rm -rf {} \;
find . -name .o    -exec rm -rf {} \;
find . -name .lo    -exec rm -rf {} \;

rm -rf configure config.* config autom4te.cache tests/test* tests/v* tests/stresstest/* \
    libpcp/libpcp1.pc
set +x

# generate the install include file
(echo "#ifndef _HAVE_PCP"; echo "#define _HAVE_PCP"; echo) > include/pcp.h
(echo "#ifdef __cplusplus"; echo "extern \"C\" {"; echo "#endif"; echo) >> include/pcp.h

ls include/pcp/*.h | sed 's#include/##' | while read include; do
  echo "#include \"$include\"" >> include/pcp.h
done

(echo "#ifdef __cplusplus"; echo "}"; echo "#endif"; echo) >> include/pcp.h
(echo; echo "#endif") >> include/pcp.h


# generate the version file
maj=`egrep "#define PCP_VERSION_MAJOR" include/pcp/version.h | awk '{print $3}'`
min=`egrep "#define PCP_VERSION_MINOR" include/pcp/version.h | awk '{print $3}'`
pat=`egrep "#define PCP_VERSION_PATCH" include/pcp/version.h | awk '{print $3}'`
echo -n "$maj.$min.$pat" > VERSION

# generate the manpage
echo "=head1 NAME

Pretty Curved Privacy - File encryption using eliptic curve cryptography.

=head1 SYNOPSIS

" > man/pcp1.pod
cat src/usage.txt | sed "s/^/  /g" >> man/pcp1.pod
cat man/pcp.pod >> man/pcp1.pod
cat man/details.pod >> man/pcp1.pod
cat man/footer.pod >> man/pcp1.pod

pod2man -r "PCP `cat VERSION`" -c "USER CONTRIBUTED DOCUMENTATION" man/pcp1.pod > man/pcp1.1

# generate the top level readme
cat man/pcp.pod man/install.pod man/footer.pod > README.pod
pod2text README.pod > README.txt

# generate usage.h
(cd src && ./usage.sh)

clean=$1

touch README

if test -z "$clean"; then
  mkdir -p ./config
  
  if ! command -v libtool >/dev/null 2>&1; then
      echo "could not find libtool." 1>&2
      exit 1
  fi
  
  if ! command -v autoreconf >/dev/null 2>&1; then
      echo "could not find autoreconf." 1>&2
      exit 1
  fi
  
  autoreconf --install --force --verbose -I config
fi

rm -rf README include/pcp/config.h.in~ libpcp/stamp-h1 autom4te.cache
