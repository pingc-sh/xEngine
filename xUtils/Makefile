DIR      = $(shell pwd)
INC_DIR  = $(DIR)
SRC_DIR  = $(DIR)
OBJ_DIR  = $(DIR)
LIB_DIR  = $(DIR)/lib
CC       = g++
CPPFLAGS = -std=c++14 -g -Wall -I$(INC_DIR) -I$(SRC_DIR)
SRC      = $(wildcard $(SRC_DIR)/*.cpp) 
OBJ      = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRC)))
TARGET   = libxUtils.a
LIB      = $(LIB_DIR)/$(TARGET)

$(LIB) : $(OBJ)
	ar -cr $(LIB) $(OBJ)
	rm -rf *.o
  
%.o : %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@ 

clean:
	rm -rf $(LIB) *.o
