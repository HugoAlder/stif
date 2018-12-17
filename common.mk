# Variable shared by all targets

# Use gcc by default
CC ?= gcc

# Speed up the build
NPROC_EXISTS := $(shell which nproc)
ifeq ($(NPROC_EXISTS),)
CPUS ?= 1 # probably safer on ARM
else
CPUS := $(shell nproc)
endif
$(info [*] Nb of CPUs: ${CPUS})
MAKEFLAGS += --jobs=$(CPUS)

CFLAGS += -Wall -Werror -Wextra -g
