# Executable targets
TARGETS = lab5 UDP_reciever Video_processor

# Compiler and flags
CC = g++
CFLAGS = -Ofast -Wall -std=c++11 `pkg-config --cflags opencv`
LDFLAGS = -Ofast -Wall -std=c++11 `pkg-config --libs opencv` -lpthread

# Default rule
all: $(TARGETS)

# Rules for each target
lab5: lab5.o
	$(CC) $(LDFLAGS) -o $@ $^ `pkg-config --libs opencv`

UDP_reciever: UDP_reciever.o
	$(CC) $(LDFLAGS) -o $@ $^ `pkg-config --libs opencv`
	
Video_processor: Video_processor.o	
	$(CC) $(LDFLAGS) -o $@ $^ `pkg-config --libs opencv`

# Pattern rule for object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean rule
clean:
	rm -f $(TARGETS) *.o *~