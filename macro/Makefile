TARGET = main
CC = gcc
CFLAGS = -Wall -Wextra -g

all : $(TARGET)
pre_only : $(TARGET).i
	
$(TARGET).i: $(TARGET).c
	$(CC) -E $(TARGET).c -o $(TARGET).i

$(TARGET) : $(TARGET).o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o

$(TARGET).o : $(TARGET).c
	$(CC) $(CFLAGS) -c $(TARGET).c -o $(TARGET).o 

clean:
	rm *.o $(TARGET)

