# Lustre V6 build for cruisectr
# - Generates into ./gen/
# - Renames lv6 outputs to cruisectr.{c,h} + cruisectr_loop.c
# - "make" builds the executable by default


LUS      := cruisectr.lus
NODE     := cruisectr

OUTDIR   := gen
PREFIX   := cruisectr

cruisectr-sim: sim
cruisectr-exe: exe

# lv6 default naming scheme: <lusbase>_<node>.*
LUSBASE  := $(basename $(notdir $(LUS)))
LV6PFX   := $(OUTDIR)/$(LUSBASE)_$(NODE)

# Canonical names we want
CAN_C    := $(OUTDIR)/$(PREFIX).c
CAN_H    := $(OUTDIR)/$(PREFIX).h
CAN_LOOP := $(OUTDIR)/$(PREFIX)_loop.c

GEN      := $(LV6PFX).c $(LV6PFX).h $(LV6PFX)_loop.c $(CAN_C) $(CAN_H) $(CAN_LOOP) *.ec *.oc *.sh *.exec

.PHONY: all exe genfiles clean

.PHONY: sim

sim: genfiles
	luciole $(LUS) $(NODE)

all: exe

# Build executable (lv6 also produces a .exec in the working directory)
exe: genfiles
	@# If you want lv6's .exec to live in OUTDIR too, see note below.

# Generate + rename files into a clean set of names
genfiles:
	@mkdir -p $(OUTDIR)
	@rm -f $(OUTDIR)/*.c $(OUTDIR)/*.h
	@cd $(OUTDIR) && lv6 -cc ../$(LUS) -n $(NODE)
	@# Rename lv6 outputs to stable names
	@mv -f $(LV6PFX).c       $(CAN_C)
	@mv -f $(LV6PFX).h       $(CAN_H)
	@mv -f $(LV6PFX)_loop.c  $(CAN_LOOP)

clean:
	@echo "Cleaning generated files..."
	@rm -rf $(OUTDIR)
	@rm -f *.ec *.oc *.c *.h *.sh *.exec