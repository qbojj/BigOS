include $(MAKE_INCLUDE_PATH)/toolchain.mk

$(if $(value BUILD_DIR),,$(error "BUILD_DIR is not set"))
BPATH := $(DPATH)/$(NAME)/obj

LIB_OBJS := $(foreach l, $(DEPS), $(BUILD_DIR)/lib/lib$(l).a)
DEPS_LDFLAGS := $(foreach l, $(DEPS), -l$(l))

SRC_C := $(shell find . -name '*.c')
SRC_S := $(shell find . -name '*.s')
OBJ_C := $(patsubst %.c, $(BPATH)/%.c.o, $(SRC_C))
OBJ_S := $(patsubst %.s, $(BPATH)/%.s.o, $(SRC_S))

always:
	mkdir -p $(BPATH)

clean:
	rm -rf $(BPATH)
	rm -rf $(TPATH)

$(DPATH)/lib$(NAME).a: $(OBJ_C) $(OBJ_S) $(LIB_OBJS)
	$(RV_LD) $(LDFLAGS) $(OBJ_S) $(OBJ_C) $(DEPS_LDFLAGS) -o $@

$(BPATH)/%.c.o : %.c always
	$(RV_CC) $(CFLAGS) -c $< -o $@

$(BPATH)/%.s.o : %.s always
	$(RV_AS) $(AFLAGS) -c $< -o $@

.PHONY: all clean always
