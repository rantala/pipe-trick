all: pipes eventfd signalfd

pipes: pipes.c
	$(CC) -O2 -Wall -g $(CFLAGS) $+ -o $@

eventfd: eventfd.c
	$(CC) -O2 -Wall -g $(CFLAGS) $+ -o $@

signalfd: signalfd.c
	$(CC) -O2 -Wall -g $(CFLAGS) $+ -o $@

clean:
	$(RM) pipes eventfd signalfd
