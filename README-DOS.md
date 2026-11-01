# DJGPP / DOS build

The DJGPP target builds one statically linked `ocp.exe`. SDL3 supplies both
the Sound Blaster audio backend and the screen output; set `BLASTER` to match
the installed card. The default configuration selects SDL3 audio and an
80-column by 25-row text interface.

## Build requirements

Use the DJGPP cross compiler (normally `i586-pc-msdosdjgpp-*`) and a matching
static SDL3 build. SDL3 must be built with its DOS backend and available
through the cross `pkg-config` setup. The target dependency libraries used by
the existing build are also required: FreeType, zlib, bzip2, libjpeg, and
libpng. No shared libraries or runtime plugin directory are used. The DOS
build does not depend on libiconv: its interface uses built-in UTF-8/CP437
conversion, ZIP filenames use CP437, and other archive filenames are used as
stored without charset conversion. The archive charset override selector is
disabled.
The reduced DOS build does not include Ogg/Vorbis playback and does not check
for libogg, libvorbis, or libvorbisfile.

The DOS player sources for AdPlug and SID are Git submodules. Initialize them
before configuring:

```sh
git submodule update --init --recursive \
  playopl/adplug-git playopl/libbinio-git \
  playsid/libsidplayfp-git playsid/libresidfp-git
```

The SID build also needs `xa` (the 6502 assembler), `hexdump`, and Perl on the
build host. AdPlug's RetroWave external hardware interface is not built for DOS.

## GitHub Actions

`.github/workflows/dos.yml` cross-builds the DOS dependencies and `ocp.exe`,
then runs a startup smoke test under DOSEMU and uploads the executable, config,
and test log as an Actions artifact. The smoke test confirms file-selector and
Sound Blaster initialization. It currently reports the known SDL DOS video
`SIGFPE` as a warning, so a green workflow is not yet proof that the interface
or sustained playback works.

## Dependency sources and DJGPP builds

These are the upstream Git repositories used for the DOS dependencies. Clone
them under `~/dev` (or update an existing checkout to the revision you intend
to build):

```sh
git clone https://github.com/libsdl-org/SDL.git ~/dev/SDL
git clone https://github.com/madler/zlib.git ~/dev/zlib
git clone https://github.com/libarchive/bzip2.git ~/dev/bzip2
git clone https://github.com/libjpeg-turbo/libjpeg-turbo.git ~/dev/libjpeg
git clone https://github.com/pnggroup/libpng.git ~/dev/libpng
git clone https://github.com/freetype/freetype.git ~/dev/freetype
git clone https://github.com/roboticslibrary/libiconv.git ~/dev/libiconv
```

The DOS build does not use libiconv. The checkout at
`https://github.com/roboticslibrary/libiconv` was not built; OpenCP's DOS path
uses its built-in UTF-8/CP437 conversions instead.

The libraries below were cross-built as static archives with CMake and the
DJGPP toolchain supplied by the SDL checkout. Build/install into one prefix in
dependency order so later CMake projects can find earlier libraries:

```bash
export PATH="/usr/local/djgpp/bin:$PATH"
export DJDIR=/usr/local/djgpp/i586-pc-msdosdjgpp
export SDL_TOOLCHAIN="$HOME/dev/SDL/build-scripts/i586-pc-msdosdjgpp.cmake"
export PREFIX=/tmp/djgpp-deps
mkdir -p "$PREFIX"
printf 'include("%s")\nset(CMAKE_SYSTEM_PROCESSOR i586 CACHE STRING "" FORCE)\n' \
  "$SDL_TOOLCHAIN" > "$PREFIX/djgpp-toolchain.cmake"
export TOOLCHAIN="$PREFIX/djgpp-toolchain.cmake"

cmake_djgpp() {
  local src="$1"
  shift
  local build="$PREFIX/build/$(basename "$src")"
  cmake -S "$src" -B "$build" \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_PREFIX_PATH="$PREFIX" "$@" &&
  cmake --build "$build" --parallel &&
  cmake --install "$build"
}

cmake_djgpp "$HOME/dev/zlib" \
  -DZLIB_BUILD_SHARED=OFF -DZLIB_BUILD_STATIC=ON \
  -DZLIB_BUILD_TESTING=OFF
cmake_djgpp "$HOME/dev/bzip2" \
  -DENABLE_STATIC_LIB=ON -DENABLE_SHARED_LIB=OFF \
  -DENABLE_APP=OFF -DENABLE_DOCS=OFF -DENABLE_EXAMPLES=OFF
cp "$PREFIX/lib/libbz2_static.a" "$PREFIX/lib/libbz2.a"
cmake_djgpp "$HOME/dev/libjpeg" \
  -DENABLE_SHARED=OFF -DENABLE_STATIC=ON \
  -DWITH_SIMD=OFF -DWITH_TURBOJPEG=OFF -DWITH_TOOLS=OFF -DWITH_TESTS=OFF
cmake_djgpp "$HOME/dev/libpng" \
  -DZLIB_ROOT="$PREFIX" \
  -DZLIB_LIBRARY="$PREFIX/lib/libz.a" \
  -DZLIB_INCLUDE_DIR="$PREFIX/include" \
  -DPNG_SHARED=OFF -DPNG_STATIC=ON \
  -DPNG_TESTS=OFF -DPNG_TOOLS=OFF
cp "$PREFIX/lib/libpng.a" "$PREFIX/lib/libpng18.a"
cmake_djgpp "$HOME/dev/freetype" \
  -DZLIB_ROOT="$PREFIX" -DBZip2_ROOT="$PREFIX" \
  -DZLIB_LIBRARY="$PREFIX/lib/libz.a" \
  -DZLIB_INCLUDE_DIR="$PREFIX/include" \
  -DBZIP2_LIBRARY_RELEASE="$PREFIX/lib/libbz2.a" \
  -DBZIP2_INCLUDE_DIR="$PREFIX/include" \
  -DPNG_LIBRARY="$PREFIX/lib/libpng.a" \
  -DPNG_PNG_INCLUDE_DIR="$PREFIX/include" \
  -DFT_REQUIRE_ZLIB=ON -DFT_REQUIRE_BZIP2=ON -DFT_REQUIRE_PNG=ON \
  -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_BROTLI=ON -DFT_DISABLE_HVF=ON \
  -DBUILD_SHARED_LIBS=OFF
cmake_djgpp "$HOME/dev/SDL" \
  -DSDL_SHARED=OFF -DSDL_STATIC=ON \
  -DSDL_TESTS=OFF -DSDL_TEST_LIBRARY=OFF -DSDL_EXAMPLES=OFF
```

SDL's CMake toolchain also overrides CMake's DOS `.lib` naming defaults with
the `.a` archive convention used by DJGPP. The bzip2 CMake target installs as
`libbz2_static.a`; OpenCP's `-lbz2` link expects `libbz2.a`, so provide that
name immediately after building bzip2, as shown above.

Use the prefix for OpenCP's configure checks and final link:

```sh
export CC=i586-pc-msdosdjgpp-gcc
export CXX=i586-pc-msdosdjgpp-g++
export CC_FOR_BUILD=gcc
export CXX_FOR_BUILD=g++
export PKG_CONFIG_LIBDIR="$PREFIX/lib/pkgconfig"
export CPPFLAGS="-I$PREFIX/include -I$PREFIX/include/freetype2 -I$PREFIX/include/libpng18"
export LDFLAGS="-L$PREFIX/lib"
```

`/tmp` is temporary; choose a persistent `PREFIX` if the libraries should
survive cleanup or a reboot.

Configure and build from the repository root:

```sh
./configure --build="$(gcc -dumpmachine)" \
  --host=i586-pc-msdosdjgpp --with-sdl3 --with-builtin=core
make
```

Prepend the directory containing the prefixed DJGPP tools, rather than
sourcing DJGPP's `setenv` script: its unprefixed `gcc` can be selected as the
build-machine compiler by configure.

The resulting `ocp.exe` and generated `ocp.ini` are in the build directory.
Keep `ocp.ini` next to the executable; the DOS launcher uses the executable's
directory for configuration and data files. DOS drive paths with backslashes
are accepted by the file selector. Temporary files use `TEMP` or `TMP` when
set, and otherwise use the executable directory.
The DOS configuration defaults to a 500 ms playback buffer, and SDL3 uses
2048-frame Sound Blaster chunks to improve tolerance of DOS scheduling delays.
This increases playback latency; adjust `plrbufsize` in `ocp.ini` if needed.

## Run

Run under DOS with a DPMI host available (for example `CWSDPMI.EXE`) and a
correct `BLASTER` setting. SDL3's DOS audio driver reads the Sound Blaster
configuration from that environment variable. The SDL3 video backend uses
VGA graphics and can use VESA modes when available; the 80x25 interface is
rendered by SDL rather than written to the BIOS text screen, so VESA is not a
runtime requirement for the basic display.

The target includes AdPlug OPL, AY, GMD, HVL/AHX, IT, SID, WAV, XM, and YM
playback. It does not include dynamic plugins, CD audio, external process
execution, MusicBrainz lookups, or Ancient-based compressed-module decoding.
Other codec players require libraries not present in the DOS dependency
prefix, while several additional bundled players need uninitialized or
incomplete Git submodules.
