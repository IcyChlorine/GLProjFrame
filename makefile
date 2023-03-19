all: main test

main:
	(cd GLProjFrame; make)

test:
	(cd test; make)