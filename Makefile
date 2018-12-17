# Main makefile

SUBDIR = src regress test

OBJDIR = obj
BINDIR = bin
COV    = cov

export

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
	rm -rf ${OBJDIR} ${BINDIR} ${COV}
