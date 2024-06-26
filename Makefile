CXXFLAGS =  -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
   -Wc++14-compat -Wmissing-declarations -Wcast-qual -Wchar-subscripts                \
   -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal           \
   -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline                   \
   -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked                     \
   -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo           \
   -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn                         \
   -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default               \
   -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast                    \
   -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing            \
   -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation             \
   -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192                  \
   -Wstack-usage=8192 -fsanitize=address -fsanitize=undefined -fPIE -Werror=vla

TARGET = engine
CC = g++

OBJ = ./obj/
SRC = ./src/
INCLUDE = ./include/

rules: $(OBJ)Inference_engine.o $(OBJ)generate_rules.o
	$(CC) $(OBJ)Inference_engine.o $(OBJ)generate_rules.o $(CXXFLAGS) -o $(TARGET) && ./$(TARGET)

$(OBJ)Inference_engine.o: $(INCLUDE)Inference_engine.h $(SRC)Inference_engine.cpp
	$(CC) -g -c $(SRC)Inference_engine.cpp -o $(OBJ)Inference_engine.o

$(OBJ)generate_rules.o: $(INCLUDE)Inference_engine.h $(SRC)generate_rules.cpp
	$(CC) -g -c $(SRC)generate_rules.cpp -o $(OBJ)generate_rules.o




engine: $(OBJ)Inference_engine.o $(OBJ)engine.o
	$(CC) $(OBJ)Inference_engine.o $(OBJ)engine.o $(CXXFLAGS) -o $(TARGET) && ./$(TARGET)

$(OBJ)Inference_engine.o: $(INCLUDE)Inference_engine.h $(SRC)Inference_engine.cpp
	$(CC) -g -c $(SRC)Inference_engine.cpp -o $(OBJ)Inference_engine.o

$(OBJ)engine.o: $(INCLUDE)Inference_engine.h $(SRC)engine.cpp
	$(CC) -g -c $(SRC)engine.cpp -o $(OBJ)engine.o

clean:
	rm -rf $(OBJ)*.o $(TARGET) engine log.txt report.txt