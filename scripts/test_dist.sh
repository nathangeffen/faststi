#!/bin/sh
set -v
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$SCRIPT_DIR"/..
ROOT=`pwd`
cd $ROOT
pwd
rm -rf debug
meson debug
cd debug
ninja dist
cd meson-dist
sha256sum faststi-0.?.?.tar.xz | awk '{print $1}' >t1.txt;awk '{print $1}' faststi-0.?.?.tar.xz.sha256sum > t2.txt; diff t1.txt t2.txt; rm t1.txt t2.txt
cd "$ROOT"
rm -rf tmp/
mkdir tmp
cd tmp
cp "$ROOT"/debug/meson-dist/faststi-0.?.?.tar.xz .
tar xf faststi-0.?.?.tar.xz
cd faststi-0.?.?
meson debug
cd debug
ninja test
sudo ninja install
cd "$SCRIPT_DIR"
gcc testfsti.c `pkg-config --cflags --libs faststi gsl glib-2.0` -o testfsti
valgrind --leak-check=full ./testfsti
export LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu/
./faststi.py -c ~/workspace/C/faststi/simulations/examples/eg1.ini
cd "$ROOT"/tmp/faststi-0.?.?/debug
sudo ninja uninstall
echo Success!
