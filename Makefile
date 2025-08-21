DEBUG = -g
CC = qcc
LD = qcc

TARGET = -Vgcc_ntox86_64
#TARGET = -Vgcc_ntoaarch64le

SRC_DIR   = src
INC_DIR   = $(SRC_DIR)/include
OBJDIR    = build
OUTDIR    = out

CFLAGS += $(DEBUG) $(TARGET) -Wall -I$(INC_DIR) -MMD -MP
LDFLAGS+= $(DEBUG) $(TARGET)

# Find all .c files in src
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files in build/
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Final binary
BIN  = $(OUTDIR)/main

VM_IP = "192.168.56.102"

all: $(BIN)


# Link step
$(BIN): $(OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Compile step
$(OBJDIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

clean:
	rm -rf $(OBJDIR) $(OUTDIR)
