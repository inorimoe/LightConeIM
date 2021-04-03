CC = g++
CFLAGS = -g
BIN = app
RUN = server.cpp
SRC = $(wildcard ./lib/*.c ./lib/*.cpp $(RUN))
OBJS = $(patsubst %.c %.cpp, %.o, $(SRC))
LIBS = -I./ -I./lib/  -L./ -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -pthread -fpermissive

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)
%.o:%.c 
	$(CC) $(CFLAGS) -c $< -o $@ -fpermissive
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -fpermissive

.PHONY:clean
clean:
	rm -f *.o ./lib/*.o $(BIN)