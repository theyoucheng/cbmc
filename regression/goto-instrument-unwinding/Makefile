DIRS = default

test:
	$(foreach var,$(DIRS), $(MAKE) -C $(var) test || exit 1;)
