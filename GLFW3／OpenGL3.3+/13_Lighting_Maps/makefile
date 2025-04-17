#OBJS specifies which files to compile as part of the project
OBJS = Source\EnableLibrary\glad.c Source\EnableLibrary\stb_image.cpp \
       Source\MyShader.cpp Source\MyCamera.cpp Source\main.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -ILibrary\glfw-3.4.bin.WIN64\include \
                -ILibrary\glad\include \
				-ILibrary\stb\include \
				-ILibrary\glm-1.0.1-light\include

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -LLibrary\glfw-3.4.bin.WIN64\lib-mingw-w64

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
DEBUG_COMPILER_FLAGS = -w #-Wl,-subsystem,windows
RELEASE_COMPILER_FLAGS = -w -Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lglfw3 -lopengl32 -lgdi32

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = Build\main.exe

#This is the target that compiles our executable
debug : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(DEBUG_COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
release : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(RELEASE_COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)