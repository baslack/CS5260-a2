all:
	mpicc -g -std=c99 -D_XOPEN_SOURCE=600 -D_POSIX_C_SOURCE=199309L -Wall -o a2 main.c -lm