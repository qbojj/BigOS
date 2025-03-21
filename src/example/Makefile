NAME := example
DEPS = stdbigos

all: $(BUILD_DIR)/$(NAME)/$(NAME).bin

$(BUILD_DIR)/$(NAME)/$(NAME).bin: $(BUILD_DIR)/$(NAME)/$(NAME).elf
	$(RV_OBJCOPY) -O binary $< $@

include $(MAKE_INCLUDE_PATH)/common.mk
LDFLAGS += -T linker.ld
