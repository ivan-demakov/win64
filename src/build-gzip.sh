cd zlib-1.2.8

PATH=/home/win64/bin:$PATH
export PATH

BINARY_PATH=/home/win64/bin INCLUDE_PATH=/home/win64/include LIBRARY_PATH=/home/win64/lib make -f win32/Makefile.gcc clean
BINARY_PATH=/usr/x86_64-w64-mingw32/bin INCLUDE_PATH=/usr/x86_64-w64-mingw32/include LIBRARY_PATH=/usr/x86_64-w64-mingw32/lib make -f win32/Makefile.gcc
BINARY_PATH=/home/win64/bin INCLUDE_PATH=/home/win64/include LIBRARY_PATH=/home/win64/lib make -f win32/Makefile.gcc install
