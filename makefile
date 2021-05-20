OBJS = main.o psnr.o bmp.o
CC = gcc
CFLAGS = -c -std=c99

psnr : $(OBJS)
	$(CC) -o psnr $(OBJS) -lm -L/lib -L/usr/lib
main.o : main.c
	$(CC) $(CFLAGS) main.c psnr.c bmp.c

clean:  
	rm *.o
	rm psnr
	@echo "Clean done!"  



