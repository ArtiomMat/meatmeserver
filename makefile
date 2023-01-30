cc = gcc
libs = -lpng -ljpeg -lm -lz -lpng
flags = -Wall -Werror

out = meatme
src_c = mle_map.c main.c
src_h = mle.h

${out}: ${src_c} ${src_h}
	${cc} ${flags} -o ${out} ${src_c} ${libs}
