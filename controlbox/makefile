## -*- Makefile -*-


CCC = gcc
CXX = g++
LD = g++
CCFLAGS = $(CFLAGS)
CXXFLAGS = $(CFLAGS)
RM = rm -f
RMDIR = rm -f -r
MKDIR = mkdir -p

TARGETDIR=build/
TARGET=cbox_test_runner

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))
remove_slash = $(patsubst %/,%,$1)
SOURCE_PATH = $(call remove_slash,$(abspath $(mkfile_dir)))

include $(SOURCE_PATH)/../build/checkers.mk # sanitizer and gcov

BUILD_PATH=$(TARGETDIR)

# catch test
INCLUDE_DIRS += $(SOURCE_PATH)/../platform/spark/device-os/third_party/catch2/catch2/single_include/catch2
							   
# add all tests
CPPSRC += $(patsubst $(SOURCE_PATH)/%,%,$(wildcard $(SOURCE_PATH)/test/*.cpp))
# and sources
CPPSRC += $(patsubst $(SOURCE_PATH)/%,%,$(wildcard $(SOURCE_PATH)/src/cbox/*.cpp))

# include dir
CFLAGS += -I$(SOURCE_PATH)/src/cbox

CFLAGS += -ffunction-sections -Wall

# Flag compiler error for [-Wdeprecated-declarations]
CFLAGS += -Werror=deprecated-declarations

# Generate dependency files automatically.
CFLAGS += -MD -MP -MF $@.d
CFLAGS += -DDEBUG_BUILD

CPPFLAGS += -std=gnu++14
CFLAGS += -pthread


# the following warnings can help find opportunities for impromevent in virtual functions
# Warn when virtual functions are overriden without override/override final specifier (requires gcc 5.1)
CPPFLAGS += -Wsuggest-override
# Warn when functions and classes can be marked final
CPPFLAGS += -Wsuggest-final-types
CPPFLAGS += -Wsuggest-final-methods

CFLAGS += $(patsubst %,-I%,$(INCLUDE_DIRS)) -I.

# doesn't work on osx
#LDFLAGS +=  -Wl,--gc-sections 

# Collect all object and dep files
ALLOBJ += $(addprefix $(BUILD_PATH), $(CSRC:.c=.o))
ALLOBJ += $(addprefix $(BUILD_PATH), $(CPPSRC:.cpp=.o))

ALLDEPS += $(addprefix $(BUILD_PATH), $(CSRC:.c=.o.d))
ALLDEPS += $(addprefix $(BUILD_PATH), $(CPPSRC:.cpp=.o.d))

all: runner

runner: $(TARGETDIR)$(TARGET)

$(TARGETDIR)$(TARGET) : $(BUILD_PATH) $(ALLOBJ)
	@echo Building target: $@
	@echo Invoking: GCC C++ Linker
	$(MKDIR) $(dir $@)
	$(LD) $(CFLAGS) $(ALLOBJ) --output $@ $(LDFLAGS)
	@echo

$(BUILD_PATH): 
	$(MKDIR) $(BUILD_PATH)

# Tool invocations

# C compiler to build .o from .c in $(BUILD_DIR)
$(BUILD_PATH)%.o : $(SOURCE_PATH)/%.c
	@echo Building file: $<
	@echo Invoking: GCC C Compiler
	$(MKDIR) $(dir $@)
	$(CCC) $(CCFLAGS) -c -o $@ $<
	@echo

# CPP compiler to build .o from .cpp in $(BUILD_DIR)
# Note: Calls standard $(CC) - gcc will invoke g++ as appropriate
$(BUILD_PATH)%.o : $(SOURCE_PATH)/%.cpp
	@echo Building file: $<
	@echo Invoking: GCC CPP Compiler
	$(MKDIR) $(dir $@)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
	@echo

# Other Targets
clean:	
	$(RM) $(ALLOBJ) $(ALLDEPS) $(TARGETDIR)$(TARGET)
	$(RMDIR) $(TARGETDIR)
	@echo

# print variable by invoking make print-VARIABLE as VARIABLE = the_value_of_the_variable
print-%  : ; @echo $* = $($*)

.PHONY: all clean runner
.SECONDARY:

# Include auto generated dependency files
-include $(ALLDEPS)



