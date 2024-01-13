compile:
	gcc main.c src/scheduler.c src/process.c src/util.c src/instruction.c src/priority_queue.c -o scheduler -I include

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

test: compile

	@for i in 1 2 3 4 5 6 7 8 9 10 11; do \
		cp ./inputs/def$$i.txt ./definition.txt; \
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