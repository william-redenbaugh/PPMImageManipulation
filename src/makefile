CC=g++
CFLAGS= -I. -g
LDFLAGS+= -lrt -lm -lpthread
DEPS = ppm_management.h io_handler.h

%.o: %.c $(DEPS)
	$(CC) -I $(CFLAGS) -c -o $@ $<

%.o: %.cpp $(DEPS)
	$(CC) -I $(CFLAGS) -c -o $@ $<

PPMImageManipulation: PPMImageManipulation ppm_management.o io_handler.o main.o
	$(CC) $(CFLAGS) -o PPMImageManipulation ppm_management.o main.o  $(LDFLAGS)

clean:
	rm -f PPMImageManipulation ppm_management.o main.o
FORCE:
	.PHONY: FORCE