src_files=vis.c

win_libs="G:/.minlib/SDL2-2.0.7/i686-w64-mingw32/lib/libSDL2.a" "G:/.minlib/SDL2-2.0.7/i686-w64-mingw32/lib/libSDL2main.a"
win_incl_dirs=-I"G:/.libraries/stb" -I"G:\.minlib\SDL2-2.0.7\i686-w64-mingw32\include"
win_lib_dirs=-L"G:/.minlib/SDL2-2.0.7/i686-w64-mingw32/lib"
win_sdl2_opts=-static -lmingw32 -lSDL2main -lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc
win_other_opts=-mconsole

nix_libs="/usr/lib/libSDL2.a" "/usr/lib/libSDL2main.a"
nix_incl_dirs=
nix_lib_dirs=
nix_sdl2_opts=$(shell sdl2-config --static-libs)

other_opts=-lgomp -fopenmp -O2 -w -o bin/mandlebrot

# # #

win:
	gcc $(src_files) $(win_libs) $(win_incl_dirs) $(win_lib_dirs) $(win_sdl2_opts) $(other_opts) $(win_other_opts)

wing:
	gcc -g $(src_files) $(win_libs) $(win_incl_dirs) $(win_lib_dirs) $(win_sdl2_opts) $(other_opts) $(win_other_opts)

# # #

nix:
	gcc $(src_files) $(nix_libs) $(nix_incl_dirs) $(nix_lib_dirs) $(nix_sdl2_opts) $(other_opts)

nixg:
	gcc -g $(src_files) $(nix_libs) $(nix_incl_dirs) $(nix_lib_dirs) $(nix_sdl2_opts) $(other_opts)
