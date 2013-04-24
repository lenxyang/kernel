
ROOTDIR=$(CURDIR)/

gdb:
	(cd $(ROOTDIR) && $(ROOTDIR)/tools/gdb.sh)

run:
	(cd $(ROOTDIR) && $(ROOTDIR)/tools/run.sh)

bgdb:
	(cd $(ROOTDIR) && $(ROOTDIR)/tools/bochs_gdb.sh)
