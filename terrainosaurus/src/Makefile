INCA=/home/jediry/Documents/Projects/inca

CC=g++
CFLAGS=-I$(INCA)/src -I$(INCA)/3rd_party
LDFLAGS=-L/usr/X11R6/lib -L$(INCA)/src -linca -lglut -lGLU -lGL

EXE=map_editor
SRC=GLUTMapEditor.cpp MapEditorWidget.cpp Map.cpp

$(EXE) : $(SRC)
	$(CC) $(CFLAGS) -o $(EXE) $(SRC) $(LDFLAGS)
