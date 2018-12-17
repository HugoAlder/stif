# src makefile

BASE     := ..
COMMON   := ${BASE}/common.mk
SRC      := $(wildcard  *.c)
HDR      := $(wildcard  ${BASE}/include/*.h)
OBJDIR   := ${BASE}/obj/tests
OBJ      := $(addprefix ${OBJDIR}/, $(patsubst %.c,%.o,$(SRC)))
STIF_OBJ := ${wildcard  ${BASE}/obj/stif/*.o}
BINDIR   := ${BASE}/bin
EXE      := ${BINDIR}/check_main

include ${COMMON}

CFLAGS  += -I${BASE}/include
LDFLAGS += -lcheck

all: ${EXE}

${EXE}: ${OBJ} ${STIF_OBJ}
	@mkdir -p ${BINDIR}
	${CC} -o $@ $^ ${LDFLAGS}

${OBJDIR}/%.o: %.c ${HDR} ${COMMON}
	@mkdir -p ${OBJDIR}
	@echo "[*] Building $@"
	${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo "[*] Cleaning"
	@rm -rf ${LIB} ${OBJ} ${COVDIR}
