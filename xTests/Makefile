DIR      = $(shell pwd)
INC_DIR  = $(DIR)/../xUtils
SRC_DIR  = $(DIR)
OBJ_DIR  = $(DIR)
LIB      = $(DIR)/../xUtils/lib/libxUtils.a
BIN_DIR  = $(DIR)/bin
CC       = g++
CPPFLAGS = -std=c++14 -g -Wall -I$(INC_DIR) -I$(SRC_DIR)
LDFLAGS  = -pthread
SRC      = $(wildcard $(SRC_DIR)/*.cpp) 
OBJ      = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRC)))
TARGET   = xUtilsTest
BIN      = $(BIN_DIR)/$(TARGET)

$(BIN) : $(OBJ)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ) $(LIB)
	rm -rf *.o
  
%.o : %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@ 

clean:
	rm -rf $(BIN)
