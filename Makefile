# Nom de l'exécutable final
TARGET = bin/neon

# Compilateur et options
CC = gcc
ASM = gcc
CFLAGS = -O3 -fno-fast-math -ftrapping-math -fsigned-zeros -frounding-math -g # -pg -fsanitize=address
LDFLAGS = -lm -g # Flags de linkage
ASMFLAGS = -g

# Fichiers src et objets
C_src = src/neon.c src/operators.c src/builtinfunctions.c src/strings.c src/dynarrays.c src/parser.c src/syntaxtrees.c src/neonio.c src/objects.c src/printerror.c src/linenoise.c src/gc.c src/runtime.c

C_OBJECTS = $(patsubst src/%.c,obj/%.o,$(C_src))

ASM_src = src/lowlevel.S
ASM_OBJECTS = $(patsubst src/%.S,obj/%.o,$(ASM_src))

# Règle par défaut
all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(C_OBJECTS) $(ASM_OBJECTS)
	$(CC) $(C_OBJECTS) $(ASM_OBJECTS) $(LDFLAGS) -o $(TARGET)

# Compilation des fichiers objets
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.S
	$(ASM) $(ASMFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(C_OBJECTS) $(ASM_OBJECTS) $(TARGET)

.PHONY: all clean

