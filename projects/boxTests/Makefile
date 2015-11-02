include ../include.mk

# calls:
CC         = g++
CFLAGS     = ${CFLAGS_COMMON} 
LDFLAGS    = ${LDFLAGS_COMMON}
EXECUTABLE = ../../bin/boxTests

SOURCES    = boxTests.cpp \
						 ../../src/linearalgebra/MATRIX3.cpp \
						 ../../src/geometry/BOX.cpp \

OBJECTS    = $(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f ../../bin/boxTests
