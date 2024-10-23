CPP = clang++
C = clang

FILES = shader\
				computeShader

LIBFILES = glad

FLAGS = -std=c++20 \
				-lglfw \
				-lGL \
				-lX11 \
				-lpthread \
				-lXrandr \
				-lXi \
				-ldl \
				-Wunused-command-line-argument \
				-g

BUILD_DIR = build
IMGUI_DIR = imgui-src
IMGUI_BUILD = imgui-build
IMGUI = $(shell find $(IMGUI_DIR) -name '*.cpp' | sed 's/$(IMGUI_DIR)\/\(.*\)\(.cpp\)/\1/')
LIB_O = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(LIBFILES)))
BD_O = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(FILES)))
IMGUI_FILES = $(addprefix $(IMGUI_DIR)/, $(addsuffix .cpp, $(IMGUI)))
IMGUI_O = $(addprefix $(IMGUI_BUILD)/, $(addsuffix .o, $(IMGUI)))

all: lib $(BD_O) window

$(BUILD_DIR)/%.o: %.cpp
	$(CPP) -g -c $^ -std=c++20 -o $@

example: $(BUILD_DIR)/example.o
	$(CPP) $^ $(IMGUI_O) $(FLAGS) $(BUILD_DIR)/$(LIBFILES).so -o $(BUILD_DIR)/$@


window: $(BUILD_DIR)/window.o $(BD_O)
	$(CPP) $^ $(IMGUI_O) $(FLAGS) $(BUILD_DIR)/$(LIBFILES).so -o $(BUILD_DIR)/$@

computeSimple: $(BUILD_DIR)/computeSimple.o
	$(CPP) $^ $(BD_O) $(IMGUI_O) $(FLAGS) $(BUILD_DIR)/$(LIBFILES).so -o $(BUILD_DIR)/$@

imgui: $(IMGUI_O)

$(IMGUI_O): $(IMGUI_BUILD)/%.o: $(IMGUI_DIR)/%.cpp
	mkdir -p ./$(IMGUI_BUILD)
	$(CPP) -g -c $< -std=c++20 -o $@

lib: $(LIB_O)
	$(C) $^ -shared -o $(BUILD_DIR)/$(LIBFILES).so

$(LIB_O): $(LIBFILES).c
	mkdir -p ./$(BUILD_DIR)
	$(C) -c -fPIC $^ -o $(BUILD_DIR)/$(LIBFILES).o

$(BD_O): $(BUILD_DIR)/%.o: %.cpp
	mkdir -p ./$(BUILD_DIR)
	$(CPP) -g -c $< -std=c++20 -o $@

clean:
	rm -rf ./$(BUILD_DIR)
