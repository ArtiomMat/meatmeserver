cc = gcc
libs =  -lpng -ljpeg -lm
flags = -Imle -Wall -Werror

o = mle_map.o main.o
h = *.h

server: ${o}
	${cc} -o $@ ${flags} ${o} ${libs}

%.o: %.c ${h}
	$(cc) ${flags} -c -o $@ $< ${libs}

clean:
	rm -rf *.o
	rm server

.PHONY: makelib