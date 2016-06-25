cd gc-7.4.4

PATH=/home/win64/bin:$PATH
export PATH

./configure --prefix=/home/win64 --host=x86_64-w64-mingw32 \
  --enable-shared --disable-static --enable-munmap --enable-parallel-mark \
  CPPFLAGS="-I/home/win64/include" LDFLAGS="-L/home/win64/bin -L/home/win64/lib"

make
make install
