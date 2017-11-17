# Hisilicon Hi3519 sample Makefile

include ../Makefile.param

# target source
INC	 := -I./interface/ -I./strategy/
HEADS:=$(wildcard *.h)
SRC  := $(wildcard *.c) 
VDA_SAMPLE_SRC := $(wildcard interface/*.c) $(wildcard strategy/*.c)
OBJ  := $(SRC:%.c=%.o) 
VDA_SAMPLE_OBJ  := $(VDA_SAMPLE_SRC:%.c=%.o)
TARGET := $(OBJ:%.o=%)
.PHONY : clean all

all: $(TARGET)

MPI_LIBS += $(REL_LIB)/libtde.a

$(TARGET):%:%.o $(COMM_OBJ) $(VDA_SAMPLE_OBJ) $(HEADS)
	$(CC) $(CFLAGS) -lpthread -lm -g -o $@ $^ $(INC) $(MPI_LIBS) $(AUDIO_LIBA) $(SENSOR_LIBS) $(HVD_LIBS)

# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)

clean:
	@echo "$(CFLAGS)"
	@echo "$(VDA_SAMPLE_OBJ)"
	@rm -f $(TARGET)
	@echo $(OBJ)
	@rm -f $(OBJ)
	@rm -f $(COMM_OBJ)
	@rm -f $(VDA_SAMPLE_OBJ)

