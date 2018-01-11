src_files=vis.c
static_libs="G:/.minlib/SDL2-2.0.7/i686-w64-mingw32/lib/libSDL2.a" "G:/.minlib/SDL2-2.0.7/i686-w64-mingw32/lib/libSDL2main.a"
incl_dirs=-I"G:/.libraries/stb" -I"G:\.minlib\SDL2-2.0.7\i686-w64-mingw32\include"
lib_dirs=-L"G:/.minlib/SDL2-2.0.7/i686-w64-mingw32/lib"
sdl2_static_opts=-static -lmingw32 -lSDL2main -lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc
sdl2_dyn_opts=-lmingw32 -lSDL2main -lSDL2
other_opts=-lgomp -fopenmp -O2 -w -mconsole -o bin/mandlebrot

static:
	gcc $(src_files) $(static_libs) $(incl_dirs) $(lib_dirs) $(sdl2_static_opts) $(other_opts)

dynamic:
	gcc $(src_files) $(incl_dirs) $(lib_dirs) $(sdl2_dyn_opts) $(other_opts)

gstatic:
	gcc -g $(src_files) $(static_libs) $(incl_dirs) $(lib_dirs) $(sdl2_static_opts) $(other_opts)

gdynamic:
	gcc -g $(src_files) $(incl_dirs) $(lib_dirs) $(sdl2_dyn_opts) $(other_opts)

pgstatic:
	gcc -pg $(src_files) $(static_libs) $(incl_dirs) $(lib_dirs) $(sdl2_static_opts) $(other_opts)

pgdynamic:
	gcc -pg $(src_files) $(incl_dirs) $(lib_dirs) $(sdl2_dyn_opts) $(other_opts)
