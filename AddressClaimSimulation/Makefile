SUBDIRS = node Application

.PHONY: all clean $(SUBDIRS)

App: Application/main.o node/node.o
	$(CC) -o addr_claim Application/main.o node/node.o -lpthread -g

objects: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	$(foreach d,$(SUBDIRS),$(MAKE) -C $(d) clean;)
	rm -f *.o