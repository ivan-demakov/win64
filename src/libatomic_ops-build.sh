cd libatomic_ops-7.4.4

PATH=/home/win64/bin:$PATH
export PATH

./configure --prefix=/home/win64 --host=x86_64-w64-mingw32 \
  CPPFLAGS="-I/home/win64/include" LDFLAGS="-L/home/win64/bin -L/home/win64/lib"

make
make install
