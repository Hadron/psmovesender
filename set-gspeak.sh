#!/bin/sh

set -e

case "$1" in
3.[0-9]*) ;;
*)
    echo "Usage: $0 G_SPEAK_VERSION"
    echo "example: $0 3.8"
    exit 1
    ;;
esac

sed -i \
    -e "s/g-speak3.[0-9][0-9]*/g-speak$1/g" \
    -e "s/gs3.[0-9][0-9]*x/gs${1}x/g" \
    debian/control debian/changelog CMakeLists.txt
