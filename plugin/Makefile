LDFLAGS = -shared -lc
TARGET = libplugin.so
OBJS = plugin.o
COMPILE  = $(CC) $(CFLAGS) -MD -fpic -c -o $@ $<
LINK = $(CC) $< $(LDFLAGS) -o $@

ALL:$(TARGET)

$(TARGET):$(OBJS)
	$(LINK)

%.o:%.c
	$(COMPILE)

-include $(OBJS:.o=.d)

clean:
	rm -f $(OBJS) *~ *.d *.o $(TARGET)
