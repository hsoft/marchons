TARGET = marchons
PKGDEPS = pango cairo pangocairo
MYCFLAGS = $(CFLAGS) -std=c99 `pkg-config --cflags $(PKGDEPS)`
MYLDFLAGS = $(LDFLAGS) `pkg-config --libs $(PKGDEPS)`

.PHONY: all
all: $(TARGET)

$(TARGET): main.c
	$(CC) $(MYCFLAGS) -o $@ main.c -lm $(MYLDFLAGS)
