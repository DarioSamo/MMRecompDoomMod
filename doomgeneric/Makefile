SRC_DIR    := src
BUILD_DIR  := build
INCLUDE    := include

C_SRCS     := $(wildcard $(SRC_DIR)/*.c)
CXX_SRCS   := $(wildcard $(SRC_DIR)/*.cpp)
OBJS       := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS)) \
              $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXX_SRCS))

# Detect platform
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
    TARGET   := $(BUILD_DIR)/doomgeneric.dll
    LDFLAGS  := -shared -luser32
    CFLAGS   := -O2 -I$(INCLUDE)
    CXXFLAGS := -O2 -std=c++20 -I$(INCLUDE)
    RM       := del /Q
    MKDIR    := mkdir
else
    PLATFORM := linux
    TARGET   := $(BUILD_DIR)/doomgeneric.so
    LDFLAGS  := -shared
    CFLAGS   := -O2 -fPIC -I$(INCLUDE)
    CXXFLAGS := -O2 -std=c++20 -fPIC -I$(INCLUDE)
    RM       := rm -rf
    MKDIR    := mkdir -p
endif

CC  := clang
CXX := clang++
LD  := clang++

all: $(TARGET)

$(BUILD_DIR):
	@$(MKDIR) $(BUILD_DIR)

$(TARGET): $(BUILD_DIR) $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@$(RM) $(BUILD_DIR)

.PHONY: all clean