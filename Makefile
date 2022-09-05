# assembler
ASM = "F:\Quality Software\OperatingSystemDevTools\nasm\nasm.exe"
# compiler
CC = "F:\Quality Software\OperatingSystemDevTools\i686-elf-tools\bin\i686-elf-gcc.exe"
# linker
LD = "F:\Quality Software\OperatingSystemDevTools\i686-elf-tools\bin\i686-elf-ld.exe"
# grub iso creator
GRUB = \usr\bin\grub-mkrescue
# sources
SRC = src
ASM_SRC = $(SRC)\asm
# objects
OBJ = obj
ASM_OBJ = $(OBJ)\asm
CONFIG = .\config
OUT = out
INC = .\include
INCLUDE=-I$(INC)

MKDIR= mkdir
CP = copy
DEFINES=

# assembler flags
ASM_FLAGS = -f elf32
# compiler flags
CC_FLAGS = $(INCLUDE) $(DEFINES) -m32 -std=gnu99 -ffreestanding -Wall -Wextra
# linker flags, for linker add linker.ld file too
LD_FLAGS = -m elf_i386 -T $(CONFIG)\linker.ld -nostdlib

# target file to create in linking
TARGET=$(OUT)\ATA.bin

# iso file target to create
TARGET_ISO=$(OUT)\ATA.iso
ISO_DIR=$(OUT)\isodir

OBJECTS=$(ASM_OBJ)\entry.o $(ASM_OBJ)\load_gdt.o\
		$(ASM_OBJ)\load_idt.o $(ASM_OBJ)\exception.o $(ASM_OBJ)\irq.o\
		$(OBJ)\io_ports.o $(OBJ)\vga.o\
		$(OBJ)\string.o $(OBJ)\console.o\
		$(OBJ)\gdt.o $(OBJ)\idt.o $(OBJ)\isr.o $(OBJ)\8259_pic.o $(OBJ)\ide.o\
		$(OBJ)\keyboard.o\
		$(OBJ)\kernel.o
		#$(OBJ)\fat.o

#grub-file --is-x86-multiboot $(TARGET)
#$(GRUB) -o $(TARGET_ISO) $(ISO_DIR)
#$(MKDIR) $(ISO_DIR)\boot\grub
#rm -f $(TARGET)
all: $(OBJECTS)
	$(LD) $(LD_FLAGS) -o $(TARGET) $(OBJECTS)
	$(CP) $(TARGET) $(ISO_DIR)\boot
	$(CP) $(CONFIG)\grub.cfg $(ISO_DIR)\boot\grub

$(ASM_OBJ)\entry.o : $(ASM_SRC)\entry.asm
	$(ASM) $(ASM_FLAGS) $(ASM_SRC)\entry.asm -o $(ASM_OBJ)\entry.o

$(ASM_OBJ)\load_gdt.o : $(ASM_SRC)\load_gdt.asm
	@printf "[ $(ASM_SRC)\load_gdt.asm ]\n"
	$(ASM) $(ASM_FLAGS) $(ASM_SRC)\load_gdt.asm -o $(ASM_OBJ)\load_gdt.o
	

$(ASM_OBJ)\load_idt.o : $(ASM_SRC)\load_idt.asm
	$(ASM) $(ASM_FLAGS) $(ASM_SRC)\load_idt.asm -o $(ASM_OBJ)\load_idt.o
	

$(ASM_OBJ)\exception.o : $(ASM_SRC)\exception.asm
	$(ASM) $(ASM_FLAGS) $(ASM_SRC)\exception.asm -o $(ASM_OBJ)\exception.o
	

$(ASM_OBJ)\irq.o : $(ASM_SRC)\irq.asm
	$(ASM) $(ASM_FLAGS) $(ASM_SRC)\irq.asm -o $(ASM_OBJ)\irq.o
	

$(OBJ)\io_ports.o : $(SRC)\io_ports.c
	$(CC) $(CC_FLAGS) -c $(SRC)\io_ports.c -o $(OBJ)\io_ports.o
	

$(OBJ)\vga.o : $(SRC)\vga.c
	$(CC) $(CC_FLAGS) -c $(SRC)\vga.c -o $(OBJ)\vga.o
	

$(OBJ)\ff.o : $(SRC)\ff.c
	$(CC) $(CC_FLAGS) -c $(SRC)\ff.c -o $(OBJ)\ff.o
	

$(OBJ)\diskio.o : $(SRC)\diskio.c
	$(CC) $(CC_FLAGS) -c $(SRC)\diskio.c -o $(OBJ)\diskio.o
	

$(OBJ)\string.o : $(SRC)\string.c
	$(CC) $(CC_FLAGS) -c $(SRC)\string.c -o $(OBJ)\string.o
	

$(OBJ)\console.o : $(SRC)\console.c
	$(CC) $(CC_FLAGS) -c $(SRC)\console.c -o $(OBJ)\console.o
	

$(OBJ)\gdt.o : $(SRC)\gdt.c
	$(CC) $(CC_FLAGS) -c $(SRC)\gdt.c -o $(OBJ)\gdt.o
	

$(OBJ)\idt.o : $(SRC)\idt.c
	$(CC) $(CC_FLAGS) -c $(SRC)\idt.c -o $(OBJ)\idt.o
	

$(OBJ)\isr.o : $(SRC)\isr.c
	$(CC) $(CC_FLAGS) -c $(SRC)\isr.c -o $(OBJ)\isr.o
	

$(OBJ)\8259_pic.o : $(SRC)\8259_pic.c
	$(CC) $(CC_FLAGS) -c $(SRC)\8259_pic.c -o $(OBJ)\8259_pic.o
	

$(OBJ)\ide.o : $(SRC)\ide.c
	$(CC) $(CC_FLAGS) -c $(SRC)\ide.c -o $(OBJ)\ide.o
	

$(OBJ)\keyboard.o : $(SRC)\keyboard.c
	$(CC) $(CC_FLAGS) -c $(SRC)\keyboard.c -o $(OBJ)\keyboard.o
	

$(OBJ)\kernel.o : $(SRC)\kernel.c
	$(CC) $(CC_FLAGS) -c $(SRC)\kernel.c -o $(OBJ)\kernel.o

clean:
	rm -f $(OBJ)\*.o
	rm -f $(ASM_OBJ)\*.o
	rm -rf $(OUT)\*
