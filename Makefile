# Main makefile

SUBDIR = src regress

OBJDIR = obj
BINDIR = bin

all:
	@for dir in ${SUBDIR} ; do \
		echo "[*] Building subdir $$dir" ; \
		$(MAKE) -C $$dir || exit ;\
	done

.PHONY: clean testu

testu:
	$(MAKE) -C test test

regress-valgrind:
	checkmate check --valgrind

clean:
	@echo "[*] Cleaning"
	rm -rf ${OBJDIR} ${BINDIR}
