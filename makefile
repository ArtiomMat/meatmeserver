cc = gcc
libs = -lpng -ljpeg -lm -lz -lpng

out = meatme
src_c = mle_map.c main.c
src_h = mle.h

${out}: ${src_c} ${src_h}
	${cc} -o ${out} ${src_c} ${libs}
