CXXFLAGS = -g3 -Wall -fmessage-length=0

OBJS = wordshooter.o util.o

LIBS = -L/usr/X11R6/lib -lm -lpthread -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11 -lfreeimage -lSDL2 -lSDL2_mixer

TARGET = word-shooter

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
