MODULE := devtools/create_gametext

MODULE_OBJS := \
        file.o \
        po_parser.o \
        create_gametext.o

# Set the name of the executable
TOOL_EXECUTABLE := create_gametext

# Include common rules
include $(srcdir)/rules.mk
