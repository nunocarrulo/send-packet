TARGET := sendpacket
CC := gcc
AR := ar
OBJ_DIR := objs
OBJS := $(OBJ_DIR)/sendpacket.o
#objs in current directory
TOP := $(PWD)
CFLAGS_GLOBAL=-g -Wall
COMPILE  =  $(CC) $(CFLAGS_GLOBAL) $(CFLAGS_LOCAL) -MD -c -o $@ $<
MAKELIB  = $(AR) -cr $@ $^
CFLAGS_LOCAL = -Iudp -Itcp -Itest1

#the directory where the objs put

all: $(TARGET)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o : %.c
	$(COMPILE)

-include $(OBJS:.o=.d)

dir := test1
include $(dir)/tt.mk

dir := udp
include $(dir)/udp.mk

dir := tcp
include $(dir)/tcp.mk

dir := share
include $(dir)/share.mk

#dir := xxxxx
#-include $(dir)

$(TARGET): $(OBJ_DIR) $(OBJS) $(LIBS_LIST)
	$(CC) $(OBJS) $(LDFLAGS_PATH) $(LIBS_LIST) $(LDFLAGS_GLOBAL) -o $@

clean:
	rm -rf $(OBJ_DIR) $(CLEAN_LIST) $(TARGET)
