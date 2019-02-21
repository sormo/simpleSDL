call "c:\src\emsdk\emsdk_env.bat"

mkdir emscripten
cd emscripten

call emcmake cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -G"Unix Makefiles" "../.." 
make VERBOSE=1

cd ..