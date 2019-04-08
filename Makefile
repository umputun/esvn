MAKECMD=make

.PHONY : esvn esvn-diff-wrapper

all: esvn esvn-diff-wrapper
	@echo "** done **"

esvn:
	qmake esvn.pro
	make -f esvn.mak
	
esvn-diff-wrapper:
	qmake esvn-diff-wrapper.pro
	make -f esvn-diff-wrapper.mak

install:
	make -f esvn.mak install
	make -f esvn-diff-wrapper.mak install
	mkdir -p /usr/share/doc/esvn
	cp -f -r html-docs /usr/share/doc/esvn

#cp -f esvn.pdf /usr/share/doc/esvn

install-doc:	
	mkdir -p /usr/share/doc/esvn
	cp -f -r html-docs /usr/share/doc/esvn

#cp -f esvn.pdf /usr/share/doc/esvn

clean:
	make -f esvn.mak clean
	make -f esvn-diff-wrapper.mak clean

	

