cpp := g++
EXE := genCharacter
CXXFLAGS=-fPIC
LIB := -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lfreetype
INC := -I/usr/local/include/opencv -I/usr/local/include/opencv2 -I/usr/local/include -I/usr/include/freetype2
SOURCE = $(wildcard *.cpp)  
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCE))
$(EXE):$(OBJECTS)
	$(cpp) $(CXXFLAGS) -o3 -o $(EXE) $(INC) $(OBJECTS) $(LIB)
$(OBJECTS):%.o:%.cpp
	$(cpp) $(CXXFLAGS) -o3 $(INC) $(LIB) -c $< -o $@
clean:
	rm $(OBJECTS) $(EXE)
