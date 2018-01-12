# Independent
src=src/render.c
other_opts=-lgomp -fopenmp -O2 -w -o bin/mandible

# Windows
win_incl_dirs=-I"G:/.libraries/stb" -I"G:\.minlib\SDL2-2.0.7\i686-w64-mingw32\include"
win_other_opts=-mconsole

# Unix
nix_incl_dirs=
nix_other_opts=

# Filled options
win_full=$(src) $(win_incl_dirs) $(win_other_opts) $(other_opts)
nix_full=$(src) $(nix_incl_dirs) $(nix_other_opts) $(other_opts)

win:
	gcc $(win_full)

wing:
	gcc -g $(win_full)

nix:
	gcc $(nix_full)

nixg:
	gcc -g $(nix_full)
