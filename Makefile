RM := rm -f

GCC := gcc
GCCFLAGS := -Wall -std=c99 -g -lrt -pthread
SOURCES_MAIN := lib.c slave.c
SOURCES_VISTA := lib.c

all:app vista

app: main.c
	$(GCC) main.c $(GCCFLAGS) $(SOURCES_MAIN) -o md5

vista: vista.c
	$(GCC) vista.c $(GCCFLAGS) $(SOURCES_VISTA) -o vista

clean:
	$(RM)	md5
	$(RM)	vista
	$(RM)	respuesta.txt

.PHONY: all app vista clean
