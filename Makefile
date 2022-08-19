BINDIR=bin
OBJDIR=obj
TOOLSDIR=tools

TARGET=$(BINDIR)/sixoffive.prg

$(TARGET): main.a65 main2.a65 demo.a65 \
	macros.a65 memory.a65 music.a65 util.a65 \
	$(OBJDIR)/data/nl-cram.bin \
	$(OBJDIR)/data/nl-cset.bin \
	$(OBJDIR)/data/nl-scr.bin \
	$(OBJDIR)/data/scr_font.bin \
	$(OBJDIR)/data/music.sid \
	$(OBJDIR)/data/text.a65
	mkdir -vp $(dir $@)
	vasm6502_oldstyle -mega65 -sect -L $(TARGET:prg=lst) -illegal -Fbin -cbm-prg -o $@ main.a65

FORCE: $(TARGET)

$(OBJDIR)/data:
	mkdir -vp $@

$(OBJDIR)/data/nl-cram.bin: assets/nl-cram.bin $(OBJDIR)/data
	cp $< $@
$(OBJDIR)/data/nl-cset.bin: assets/nl-cset.bin $(OBJDIR)/data
	cp $< $@
$(OBJDIR)/data/nl-scr.bin: assets/nl-scr.bin $(OBJDIR)/data
	cp $< $@

$(OBJDIR)/data/music.sid: assets/cerror-iirrookoos_short.sid $(OBJDIR)/data
	cp $< $@

$(OBJDIR)/data/scr_font.bin: assets/font14_1bpl_08x16.png bin/png2mono
	mkdir -vp $(dir $@)
	$(word 2,$^) $< $@ 8 16

$(OBJDIR)/data/text.a65: assets/nl.txt bin/nl_make_text
	mkdir -vp $(dir $@)
	$(word 2,$^) $< $@

$(BINDIR)/nl_make_text: $(TOOLSDIR)/nl_make_text.c
	mkdir -vp $(dir $@)
	gcc -Og -g -o $@ $< -lm

$(BINDIR)/png2mono: $(TOOLSDIR)/png2mono.c $(TOOLSDIR)/stb_image.h
	mkdir -vp $(dir $@)
	gcc -Og -g -o $@ $< -lm


tools: $(BINDIR)/png2mono

clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)

run-emu: $(TARGET)
	xemu-xmega65 -prg $<

run-hw: $(TARGET)
	m65 -l /dev/ttyUSB0 -r -F $<

print-hex: $(TARGET)
	xxd $<
