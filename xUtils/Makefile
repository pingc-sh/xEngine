DIR      = $(shell pwd)
INC_DIR  = $(DIR)/include
SRC_DIR  = $(DIR)/src
OBJ_DIR  = $(DIR)/lib
LIB_DIR  = $(DIR)/lib
CC       = g++
CPPFLAGS = -std=c++14 -g -Wall -I$(INC_DIR)
SRC      = $(wildcard $(SRC_DIR)/*.cpp) 
OBJ      = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TARGET   = libxUtils.a
LIB      = $(LIB_DIR)/$(TARGET)

$(LIB) : $(OBJ)
	ar -cr $(LIB) $(OBJ)
	rm -rf $(OBJ_DIR)/*.o

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(LIB) $(OBJ_DIR)/*.o
