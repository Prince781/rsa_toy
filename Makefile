CFLAGS=-Wall -Werror
LDFLAGS =-lm
OBJDIR=obj
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:%.c=$(OBJDIR)/%.o)

rsa_toy: $(OBJECTS) Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o rsa_toy

$(OBJDIR)/%.o: %.c %.h Makefile
	@if [ ! -d $(shell dirname $@) ]; then mkdir -p $(shell dirname $@); fi
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c Makefile
	@if [ ! -d $(shell dirname $@) ]; then mkdir -p $(shell dirname $@); fi
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@


.PHONY: clean clean_keys

clean:
	rm -f rsa_toy $(OBJECTS)
	rm -rf $(OBJDIR)

clean_keys:
	rm -f key.priv key.pub
