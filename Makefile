CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lFLAC -lpng

TARGET = waveform
SRCS = waveform.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean
