compile:
	gcc scheduler.c -o scheduler

run: compile
	./scheduler

default: compile
	pass

# Variable Definitions
GREEN = \033[92m
RED = \033[91m
BLUE = \033[94m
RESET = \033[0m
CHECK = ✓
CROSS = ✖

# Phony Targets
.PHONY: default test clean

test:
	@gcc -o scheduler scheduler.c

	@for i in 1 2 3 4 5 6 7 8 9 10 11; do \
		cp ./inputs/def$$i.txt ./input.txt; \
		./scheduler > ./output.txt; \
		if diff ./output.txt ./inputs/out$$i.txt > /dev/null; then \
			echo "• \033[92mTest $$i Passed ✓\033[0m"; \
		else \
			echo "• \033[91mTest $$i Failed ✖\033[0m"; \
		fi; \
	done

# Clean Target
clean:
	@rm -f scheduler ./output.txt