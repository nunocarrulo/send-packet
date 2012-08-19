TARGET := sendpacket
CC := gcc
AR := ar
OBJ_DIR := objs
OBJS := $(OBJ_DIR)/sendpacket.o
#objs in current directory
TOP := $(PWD)
CFLAGS_GLOBAL= -g -Wall -Ishare
COMPILE  =  $(CC) $(CFLAGS_GLOBAL) $(CFLAGS_LOCAL) -MD -c -o $@ $<
MAKELIB  = $(AR) -cr $@ $^
CFLAGS_LOCAL = -Iraw -Iudp -Itcp -Itest1 -Iparser_cfg

#the directory where the objs put

all: $(TARGET)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o : %.c
	$(COMPILE)

-include $(OBJS:.o=.d)

dir := parser_cfg
include $(dir)/parser_cfg.mk

dir := test1
include $(dir)/tt.mk

dir := raw
include $(dir)/raw.mk

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
