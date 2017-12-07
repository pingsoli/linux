CC = gcc
CFLAGS = -Wall -g -O0 -D_GNU_SOURCE


# Specify include source code directory, output object file directory
# and final executable file directory
BIN_PATH := .
OBJ_PATH := .
SRC_PATH := .

# Recursively search in source code directory and generate 
# corresponding object file name
SRC = $(foreach x,$(SRC_PATH),$(wildcard $(addprefix $(x)/*,.c .cpp)))
OBJ = $(addprefix $(OBJ_PATH)/,$(addsuffix .o,$(notdir $(basename $(SRC)))))

# Final executable file name
TARGET = main

$(BIN_PATH)/$(TARGET): $(OBJ)
	$(CC) -o $@ $^

$(OBJ_PATH)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

run: $(BIN_PATH)/$(TARGET)
	./$(TARGET) $(args) 

.PHONY: clean
clean:
	rm -f $(TARGET) *.o
