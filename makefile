OBJS = main.o
CC = gcc
CFLAGS = -c

psnr : $(OBJS)
	$(CC) -o psnr $(OBJS) -lm -L/lib -L/usr/lib
main.o : main.c
	$(CC) $(CFLAGS) main.c

clean:  
	rm *.o
	@echo "Clean done!"  



