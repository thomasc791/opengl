CPP = clang++
C = clang

PROJECT_FILES = project

FILES = shader \
				computeShader \
				framebuffer \
				shaderStorageBuffer \
				texture

LIBFILES = glad

FLAGS = -std=c++20 \
				-lglfw \
				-L./lib/\
				-lglad \
				-lGL \
				-lX11 \
				-lpthread \
				-lXrandr \
				-lXi \
				-ldl \
				-Wunused-command-line-argument \
				-march=native \
				-O3

PROJECT_DIR = project
LIB_DIR = lib
BUILD_DIR = build
IMGUI_DIR = imgui-src
IMGUI_BUILD = imgui-build
OPENGL_OBJECTS_DIR = opengl-objects

IMGUI = $(shell find $(IMGUI_DIR) -name '*.cpp' | sed 's/$(IMGUI_DIR)\/\(.*\)\(.cpp\)/\1/')
LIB_O = $(addprefix $(LIB_DIR)/, $(addsuffix .o, $(LIBFILES)))
BD_O = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(FILES)))
PROJECT_O = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(PROJECT_FILES)))
PROJECT_CPP = $(addprefix $(PROJECT_DIR)/, $(addsuffix .cpp, $(PROJECT_FILES)))
IMGUI_FILES = $(addprefix $(IMGUI_DIR)/, $(addsuffix .cpp, $(IMGUI)))
IMGUI_O = $(addprefix $(IMGUI_BUILD)/, $(addsuffix .o, $(IMGUI)))

all: lib $(BD_O) waves 

$(BUILD_DIR)/%.o: %.cpp
	$(CPP) -g -c $^ -std=c++20 -o $@

$(PROJECT_O): $(PROJECT_CPP)
	$(CPP) -g -c $^ -std=c++20 -o $@

project: $(PROJECT_O) $(BD_O)
	$(CPP) $^ $(IMGUI_O)  $(FLAGS) -o $(BUILD_DIR)/$@

imgui: $(IMGUI_O)

$(IMGUI_O): $(IMGUI_BUILD)/%.o: $(IMGUI_DIR)/%.cpp
	mkdir -p ./$(IMGUI_BUILD)
	$(CPP) -g -c $< -std=c++20 -o $@

lib: $(LIB_O)
	$(C) $^ -shared -o $(LIB_DIR)/lib$(LIBFILES).so

$(LIB_O): $(LIBFILES).c
	mkdir -p ./$(LIB_DIR)
	$(C) -c -fPIC $^ -o $(LIB_DIR)/$(LIBFILES).o

$(BD_O): $(BUILD_DIR)/%.o: $(OPENGL_OBJECTS_DIR)/%.cpp
	mkdir -p ./$(BUILD_DIR)
	$(CPP) -g -c $< -std=c++20 -o $@

clean:
	rm -rf ./$(BUILD_DIR)
	rm -f core.*
	rm -rf ./$(LIB_DIR)
