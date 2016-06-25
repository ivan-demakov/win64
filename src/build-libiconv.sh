cd libiconv-1.14

PATH=/home/win64/bin:$PATH
export PATH

./configure --prefix=/home/win64 --host=x86_64-w64-mingw32 \
  CPPFLAGS="-Wall -I/home/win64/include" LDFLAGS="-L/home/win64/bin"

make
make install
