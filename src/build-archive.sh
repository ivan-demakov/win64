cd libarchive-3.2.1

PATH=/home/win64/bin:$PATH
export PATH

export CPPFLAGS="-I/home/win64/include"
export LDFLAGS="-L/home/win64/bin -L/home/win64/lib"
export libarchive_la_LDFLAGS="-Wl,--out-implib"
./configure --prefix=/home/win64 --host=x86_64-w64-mingw32 \
  --enable-shared --disable-static

make
make install
