#!/bin/bash -ex
TOP_SRCDIR="$(cd "$(dirname "$0")" && pwd)"
AUTOTOOLS_DIRS=(
	"$TOP_SRCDIR"
	"$TOP_SRCDIR"/examples/libopencm3_stm32f4
	"$TOP_SRCDIR"/font_generator
)
for dir in "${AUTOTOOLS_DIRS[@]}"
do
	cd "$dir"
	gnulib-tool --copy-file build-aux/git-version-gen
	cp "$TOP_SRCDIR"/libopencm3.mk build-aux/
	autoreconf --verbose --force --install --warnings=error
	cd -
done