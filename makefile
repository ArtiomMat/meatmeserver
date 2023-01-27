cc = gcc
libs = -lpng -ljpeg
flags = 

out = meatme
src_c = mle.c main.c
src_h = mle.h

${out}: ${src_c} ${src_h}
	${cc} -o ${out} ${libs} ${src_c}
