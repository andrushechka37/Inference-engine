TARGET = engine
CC = g++

OBJ = ./obj/
SRC = ./src/
INCLUDE = ./include/

.PHONY: compile
compile: $(OBJ)Inference_engine.o
	$(CC) $(OBJ)Inference_engine.o -o $(TARGET) && ./$(TARGET)

$(OBJ)Inference_engine.o: $(INCLUDE)Inference_engine.h $(SRC)Inference_engine.cpp
	$(CC) -g -c $(SRC)Inference_engine.cpp -o $(OBJ)Inference_engine.o

clean:
	rm -rf $(OBJ)*.o $(TARGET)