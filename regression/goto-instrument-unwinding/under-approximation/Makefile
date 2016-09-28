default: tests.log

testalpha:
	@../../test.pl -c ../unwinding-chain.sh -C

testbeta:
	@../../test.pl -c ../unwinding-chain.sh -T

testimpr:
	@../../test.pl -c ../unwinding-chain.sh -K

testnew:
	@../../test.pl -c ../unwinding-chain.sh -F

test:
	@../../test.pl -c ../unwinding-chain.sh 

tests.log: ../test.pl
	@../../test.pl -c ../unwinding-chain.sh 

clean:
	@for dir in *; do \
                if [ -d "$$dir" ]; then \
                        rm $$dir/*.txt $$dir/*.dot $$dir/*.gb $$dir/*.out; \
                fi; \
        done;

show:
	@for dir in *; do \
		if [ -d "$$dir" ]; then \
			vim -o "$$dir/*.c" "$$dir/*.out"; \
		fi; \
	done;
