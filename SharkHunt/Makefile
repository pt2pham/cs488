# GNU Make solution makefile autogenerated by Premake
# Type "make help" for usage help

ifndef config
  config=debug
endif
export config

PROJECTS := SharkHunt

.PHONY: all clean help $(PROJECTS)

all: $(PROJECTS)

SharkHunt: 
	@echo "==== Building SharkHunt ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f Makefile

clean:
	@${MAKE} --no-print-directory -C build -f Makefile clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "   debug"
	@echo "   release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   SharkHunt"
	@echo ""
	@echo "For more information, see http://industriousone.com/premake/quick-start"
