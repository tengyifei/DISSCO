all:
	doxygen doxygen.cfg
	cd docs/latex; make;
	cp docs/latex/refman.pdf lass-reference.pdf
clean:
	rm -rf docs/html/*
	rm -rf docs/latex/*
	rm -rf lass-reference.pdf
