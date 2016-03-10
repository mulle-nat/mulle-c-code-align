#! /bin/sh -x
#
# Generate a formula formulle-xcode-settings stand alone
#
PROJECT=MulleCCodeAlign
TARGET=mulle-c-code-align
HOMEPAGE="https://github.com/mulle-nat/mulle-c-code-align"
DESC="a unix tool to align c code"

VERSION="${1:-$AGVTAG}"
shift
ARCHIVEURL="${1:-https://github.com/mulle-nat/mulle-c-code-align/archive/${VERSION}.tar.gz}"
shift

set -e

fail()
{
   echo "$@" >&2
   exit 1
}

[ ! -z "$VERSION"  ]   || fail "no version"
[ ! -z "$ARCHIVEURL" ] || fail "no archive url"


git rev-parse "${VERSION}" >/dev/null 2>&1
if [ $? -ne 0 ]
then
   fail "No tag ${VERSION} found"
   # could tag and push
fi


TMPARCHIVE="/tmp/${PROJECT}-${VERSION}-archive"

if [ ! -f  "${TMPARCHIVE}" ]
then
   curl -L -o "${TMPARCHIVE}" "${ARCHIVEURL}"
   if [ $? -ne 0 -o ! -f "${TMPARCHIVE}" ]
   then
      echo "Download failed" >&2
      exit 1
   fi
else
   echo "using cached file ${TMPARCHIVE} instead of downloading again" >&2
fi

#
# anything less than 5 KB is wrong
#
size="`du -k "${TMPARCHIVE}" | awk '{ print $ 1}'`"
if [ $size -lt 4 ]
then
   echo "Archive truncated or missing" >&2
   cat "${TMPARCHIVE}" >&2
   rm "${TMPARCHIVE}"
   exit 1
fi

HASH="`shasum -p -a 256 "${TMPARCHIVE}" | awk '{ print $1 }'`"

cat <<EOF
class ${PROJECT} < Formula
  homepage "${HOMEPAGE}"
  desc "${DESC}"
  url "${ARCHIVEURL}"
  version "${VERSION}"
  sha256 "${HASH}"

  def install
    system "make", "install", "INSTALL_PATH=#{bin}"
  end

  test do
  end
end
# FORMULA ${TARGET}.rb
EOF
