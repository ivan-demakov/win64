cd libiconv-1.14

PATH=/home/win64/bin:$PATH
export PATH

export CPPFLAGS="-Wall -I/home/win64/include"
export LDFLAGS="-L/home/win64/bin"

./configure --prefix=/home/win64 --host=x86_64-w64-mingw32

make
make install
