#################################################################
## This Makefile Exported by MinGW Developer Studio
## Copyright (c) 2002-2004 by Parinya Thipchart
#################################################################

ifneq (,$(findstring Release, $(CFG)))
  override CFG = Release
else
  override CFG = Debug
endif

PROJECT = HZ_3WayValveOne
CC = "C:\Program Files (x86)\MinGWStudio\MinGW\bin\gcc.exe"

ifeq ($(CFG),Debug)
  OBJ_DIR = Debug
  OUTPUT_DIR = Debug
  TARGET = HZ_3WayValveOne.dll
  C_INCLUDE_DIRS = -I"C:/QNX650/host/win32/x86/usr/include" -I"C:/QNX650/target/qnx6/usr/include" 
  C_PREPROC = -D__LITTLEENDIAN__ -D__X86__ -D__QNXNTO__ 
  CFLAGS = -pipe  -Wall -g2 -O0 
  RC_INCLUDE_DIRS = 
  RC_PREPROC = 
  RCFLAGS = 
  LIB_DIRS = -L"C:/QNX650/host/win32/x86/usr/lib" 
  LIBS = 
  LDFLAGS = -pipe -shared -Wl,--output-def,"$(OBJ_DIR)\HZ_3WayValveOne.def",--out-implib,"$(OBJ_DIR)\libHZ_3WayValveOne.dll.a" -lph -lkernel32
endif

ifeq ($(CFG),Release)
  OBJ_DIR = Release
  OUTPUT_DIR = Release
  TARGET = HZ_3WayValveOne.dll
  C_INCLUDE_DIRS = 
  C_PREPROC = 
  CFLAGS = -pipe  -Wall -g0 -O2 
  RC_INCLUDE_DIRS = 
  RC_PREPROC = 
  RCFLAGS = 
  LIB_DIRS = 
  LIBS = 
  LDFLAGS = -pipe -shared -Wl,--output-def,"$(OBJ_DIR)\HZ_3WayValveOne.def",--out-implib,"$(OBJ_DIR)\libHZ_3WayValveOne.dll.a" -s 
endif

ifeq ($(OS),Windows_NT)
  NULL =
else
  NULL = nul
endif

SRC_OBJS = \
  $(OBJ_DIR)/HZ_3WayValveOne.o

define build_target
@echo Linking...
@$(CC) -o "$(OUTPUT_DIR)\$(TARGET)" $(SRC_OBJS) $(LIB_DIRS) $(LIBS) $(LDFLAGS)
endef

define compile_source
@echo Compiling $<
@$(CC) $(CFLAGS) $(C_PREPROC) $(C_INCLUDE_DIRS) -c "$<" -o "$@"
endef

.PHONY: print_header directories

$(TARGET): print_header directories $(SRC_OBJS)
	$(build_target)

.PHONY: clean cleanall

cleanall:
	@echo Deleting intermediate files for 'HZ_3WayValveOne - $(CFG)'
	-@del $(OBJ_DIR)\*.o
	-@del "$(OUTPUT_DIR)\$(TARGET)"
	-@del "$(OBJ_DIR)\$(PROJECT).def"
	-@del "$(OBJ_DIR)\lib$(PROJECT).dll.a"
	-@rmdir "$(OUTPUT_DIR)"

clean:
	@echo Deleting intermediate files for 'HZ_3WayValveOne - $(CFG)'
	-@del $(OBJ_DIR)\*.o

print_header:
	@echo ----------Configuration: HZ_3WayValveOne - $(CFG)----------

directories:
	-@if not exist "$(OUTPUT_DIR)\$(NULL)" mkdir "$(OUTPUT_DIR)"
	-@if not exist "$(OBJ_DIR)\$(NULL)" mkdir "$(OBJ_DIR)"

$(OBJ_DIR)/HZ_3WayValveOne.o: HZ_3WayValveOne.c	\
HZ_3WayValveOne.h
	$(compile_source)

