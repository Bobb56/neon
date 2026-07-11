# ----------------------------
# Makefile Options
# ----------------------------

NAME = NEONINST
ICON = ../../../../src/assets/neon16x16.png
DESCRIPTION = "Neon app installer"
COMPRESSED = NO
ARCHIVED = NO
LTO = NO
HAS_PRINTF = NO

APPVAR_PREFIX = "Neon"
APPVAR_SPLIT_SIZE = 65200

CFLAGS = -Wall -Wextra -Oz -DAPPVAR_PREFIX=\"$(APPVAR_PREFIX)\" -DAPPVAR_SPLIT_SIZE=$(APPVAR_SPLIT_SIZE)
CXXFLAGS = -Wall -Wextra -Oz -DAPPVAR_PREFIX=\"$(APPVAR_PREFIX)\" -DAPPVAR_SPLIT_SIZE=$(APPVAR_SPLIT_SIZE)

# ----------------------------

include $(shell cedev-config --makefile)
