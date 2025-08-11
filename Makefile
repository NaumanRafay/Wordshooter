CXXFLAGS =	-g3 -Wall -fmessage-length=0 #-Werror

OBJS =		wordshooter.o util.o #Board.o util.o #game.o

<<<<<<< HEAD
LIBS =-L/usr/X11R6/lib -lm -lpthread -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11 -lfreeimage -lSDL2 -lSDL2_mixer
=======
LIBS =-L/usr/X11R6/lib -lm -lpthread -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11 -lfreeimage
>>>>>>> 77d21ec46dc8b4978daaf8bfad93e1dba4879614



TARGET = word-shooter	


$(TARGET):	$(OBJS) 
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
