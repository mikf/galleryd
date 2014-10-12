CC       := g++
SRCDIR   := src
BUILDDIR := obj
# CFLAGS   := -g -std=c++11 -Wall -Wextra -pedantic -fdiagnostics-color=auto -Wno-unused-parameter
# LDFLAGS  := -g
CFLAGS   := -O3 -march=native -std=c++11 -Wall -Wextra -pedantic -fdiagnostics-color=auto -Wno-unused-parameter
LDFLAGS  := -O3 -march=native
TARGET   := galleryd

SRCEXT   := cpp
SOURCES  := $(shell find $(SRCDIR) -name *.$(SRCEXT))
OBJECTS  := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS     := $(OBJECTS:.o=.deps)
LIBS     := libmicrohttpd yajl sqlite3

CFLAGS   := $(CFLAGS) $(shell pkg-config --cflags $(LIBS))
LDFLAGS  := $(LDFLAGS) $(shell pkg-config --libs $(LIBS))


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR)/* $(TARGET)

-include $(DEPS)

.PHONY: clean
