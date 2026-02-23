# Makefile for Lustre V6 (lv6) + luciole
# Targets required by the assignment:
#   make sim  -> luciole GUI simulation
#   make exe  -> command-line executable
# Also includes clean.

LUS  := switch.lus
NODE := switch

GEN  := *.ec *.oc *.c *.h *.sh *.exec

.PHONY: sim exe run clean

# Build the executable
exe: $(NODE).exec
$(NODE).exec: $(LUS)
	@rm -f $(GEN)
	lv6 -cc $(LUS) -n $(NODE)

# GUI simulation via luciole
sim: $(LUS)
	@rm -f $(GEN)
	lv6 -cc $(LUS) -n $(NODE)
	# Patience: luciole may take a while to open on ARC
	luciole $(LUS) $(NODE)

# Convenience target: build + run
TRACE ?= traces/turn_on.txt
run: exe
	./$(NODE).exec < $(TRACE)

clean:
	rm -f $(GEN)