SRC_DIR    := src
BUILD_DIR  := build
INCLUDE    := include

C_SRCS     := $(wildcard $(SRC_DIR)/*.c)
CXX_SRCS   := $(wildcard $(SRC_DIR)/*.cpp)
OBJS       := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS)) \
              $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXX_SRCS))

CC  := clang
CXX := clang++
LD  := clang++

CFLAGS    := -O2 -I$(INCLUDE)
CXXFLAGS  := -O2 -I$(INCLUDE)
LDFLAGS   := -shared -lUser32

TARGET    := $(BUILD_DIR)/doomgeneric.dll

all: $(TARGET)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(TARGET): $(BUILD_DIR) $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all clean
