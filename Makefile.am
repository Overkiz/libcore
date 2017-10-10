SUBDIRS = include src

if PKGCONFIG
pkgconfigdir =   ${prefix}/lib/pkgconfig
pkgconfig_DATA = @PACKAGE_NAME@.pc
CLEANFILES =     ${pkgconfig_DATA}

@PACKAGE_NAME@.pc: pkg-config.pc
	( if test -e @PACKAGE_NAME@.pc; then rm -f @PACKAGE_NAME@.pc; else exit 0; fi )
	cp pkg-config.pc @PACKAGE_NAME@.pc && sed -e '/.*: *$$/d' -e 's/ \+/ /g' -i @PACKAGE_NAME@.pc
endif
