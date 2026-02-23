CC=gcc
CFLAGS=-O2 -Wall -Wextra -std=gnu11
LDFLAGS=-pthread

TARGET=rtsched

all: $(TARGET)

$(TARGET): rtsched.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o gantt.dat gantt.png edf.png

# Run under EDF via rtexec
run: $(TARGET)
	mkdir -p /tmp/$$USER
	cp $(TARGET) /tmp/$$USER/
	sudo /usr/local/bin/rtexec /tmp/$$USER/$(TARGET) -edf input.txt > gantt.dat

plot:
	gnuplot gantt.plot
	mv gantt.png edf.png
