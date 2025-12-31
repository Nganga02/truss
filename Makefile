CC := gcc
CFLAGS = -Wall -std=c17 -pedantic -O2 $(shell sdl2-config --cflags) -I$(INCDIR)
LDFLAGS:= $(shell sdl2-config --libs) -lm

SRCDIR:=./src
INCDIR:=./include
OBJDIR:=./obj
BINDIR:=./bin

interlude_sources:= truss-io.c truss-render.c linked-list.c truss.c fetch_line.c xmalloc.c render-beta.c

interlude_src=$(foreach file,$(interlude_sources),$(SRCDIR)/$(file))
interlude_obj=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(interlude_src))

headers:=$(wildcard $(INCDIR)/*.h)

TARGET01:= $(BINDIR)/render

interlude: $(TARGET01)

$(OBJDIR):
	@mkdir -p $@

$(BINDIR):
	@mkdir -p $@

$(TARGET01):$(interlude_obj)|$(BINDIR)
	$(CC) $(interlude_obj) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -g -c $< -o $@

# Clean build
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean