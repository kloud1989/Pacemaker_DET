#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#

unexport MAKEFILE_LIST
MK_NOGENDEPS := $(filter clean,$(MAKECMDGOALS))
override PKGDIR = xconfig_helloWorld
XDCINCS = -I. -I$(strip $(subst ;, -I,$(subst $(space),\$(space),$(XPKGPATH))))
XDCCFGDIR = package/cfg/

#
# The following dependencies ensure package.mak is rebuilt
# in the event that some included BOM script changes.
#
ifneq (clean,$(MAKECMDGOALS))
D:/ti/xdctools_3_23_04_60/include/utils.tci:
package.mak: D:/ti/xdctools_3_23_04_60/include/utils.tci
D:/ti/xdctools_3_23_04_60/packages/xdc/xdc.tci:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/xdc.tci
D:/ti/xdctools_3_23_04_60/packages/xdc/template.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/template.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/om2.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/om2.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/xmlgen.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/xmlgen.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/xmlgen2.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/xmlgen2.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/IPackage.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/IPackage.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/package.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/package.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/services/global/Clock.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/services/global/Clock.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/services/global/Trace.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/services/global/Trace.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/bld.js:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/bld.js
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/BuildEnvironment.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/BuildEnvironment.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/PackageContents.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/PackageContents.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/_gen.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/_gen.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Library.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Library.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Executable.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Executable.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Repository.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Repository.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Configuration.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Configuration.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Script.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Script.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Manifest.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Manifest.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Utils.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/Utils.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/ITarget.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/ITarget.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/ITarget2.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/ITarget2.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/ITargetFilter.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/ITargetFilter.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/bld/package.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/bld/package.xs
package.mak: config.bld
D:/ti/xdctools_3_23_04_60/packages/ti/targets/ITarget.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/ti/targets/ITarget.xs
D:/ti/xdctools_3_23_04_60/packages/ti/targets/C28_large.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/ti/targets/C28_large.xs
D:/ti/xdctools_3_23_04_60/packages/ti/targets/C28_float.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/ti/targets/C28_float.xs
D:/ti/xdctools_3_23_04_60/packages/ti/targets/package.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/ti/targets/package.xs
D:/ti/xdctools_3_23_04_60/packages/ti/targets/elf/ITarget.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/ti/targets/elf/ITarget.xs
D:/ti/xdctools_3_23_04_60/packages/ti/targets/elf/TMS470.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/ti/targets/elf/TMS470.xs
D:/ti/xdctools_3_23_04_60/packages/ti/targets/elf/package.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/ti/targets/elf/package.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/services/io/File.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/services/io/File.xs
D:/ti/xdctools_3_23_04_60/packages/xdc/services/io/package.xs:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/services/io/package.xs
package.mak: package.bld
D:/ti/xdctools_3_23_04_60/packages/xdc/tools/configuro/template/compiler.opt.xdt:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/tools/configuro/template/compiler.opt.xdt
D:/ti/xdctools_3_23_04_60/packages/xdc/tools/configuro/template/custom.mak.exe.xdt:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/tools/configuro/template/custom.mak.exe.xdt
D:/ti/xdctools_3_23_04_60/packages/xdc/tools/configuro/template/package.xs.xdt:
package.mak: D:/ti/xdctools_3_23_04_60/packages/xdc/tools/configuro/template/package.xs.xdt
endif

ti.targets.elf.C66.rootDir ?= C:/PROGRA~1/TEXASI~2/C6000C~1.6
ti.targets.elf.packageBase ?= D:/ti/xdctools_3_23_04_60/packages/ti/targets/elf/
.PRECIOUS: $(XDCCFGDIR)/%.oe66
.PHONY: all,e66 .dlls,e66 .executables,e66 test,e66
all,e66: .executables,e66
.executables,e66: .libraries,e66
.executables,e66: .dlls,e66
.dlls,e66: .libraries,e66
.libraries,e66: .interfaces
	@$(RM) $@
	@$(TOUCH) "$@"

.help::
	@$(ECHO) xdc test,e66
	@$(ECHO) xdc .executables,e66
	@$(ECHO) xdc .libraries,e66
	@$(ECHO) xdc .dlls,e66


all: .executables 
.executables: .libraries .dlls
.libraries: .interfaces

PKGCFGS := $(wildcard package.xs) package/build.cfg
.interfaces: package/package.xdc.inc package/package.defs.h package.xdc $(PKGCFGS)

-include package/package.xdc.dep
package/%.xdc.inc package/%_xconfig_helloWorld.c package/%.defs.h: %.xdc $(PKGCFGS)
	@$(MSG) generating interfaces for package xconfig_helloWorld" (because $@ is older than $(firstword $?))" ...
	$(XSRUN) -f xdc/services/intern/cmd/build.xs $(MK_IDLOPTS) -m package/package.xdc.dep -i package/package.xdc.inc package.xdc

.dlls,e66 .dlls: helloWorld.pe66

-include package/cfg/helloWorld_pe66.mak
-include package/cfg/helloWorld_pe66.cfg.mak
ifeq (,$(MK_NOGENDEPS))
-include package/cfg/helloWorld_pe66.dep
endif
helloWorld.pe66: package/cfg/helloWorld_pe66.xdl
	@


ifeq (,$(wildcard .libraries,e66))
helloWorld.pe66 package/cfg/helloWorld_pe66.c: .libraries,e66
endif

package/cfg/helloWorld_pe66.c package/cfg/helloWorld_pe66.h package/cfg/helloWorld_pe66.xdl: override _PROG_NAME := helloWorld.xe66
package/cfg/helloWorld_pe66.c: package/cfg/helloWorld_pe66.cfg

clean:: clean,e66
	-$(RM) package/cfg/helloWorld_pe66.cfg
	-$(RM) package/cfg/helloWorld_pe66.dep
	-$(RM) package/cfg/helloWorld_pe66.c
	-$(RM) package/cfg/helloWorld_pe66.xdc.inc

clean,e66::
	-$(RM) helloWorld.pe66
.executables,e66 .executables: helloWorld.xe66

helloWorld.xe66: |helloWorld.pe66

-include package/cfg/helloWorld.xe66.mak
helloWorld.xe66: package/cfg/helloWorld_pe66.oe66 
	$(RM) $@
	@$(MSG) lnke66 $@ ...
	$(RM) $(XDCCFGDIR)/$@.map
	$(ti.targets.elf.C66.rootDir)/bin/lnk6x -q -u _c_int00 -fs $(XDCCFGDIR)$(dir $@).  -q -o $@ package/cfg/helloWorld_pe66.oe66   package/cfg/helloWorld_pe66.xdl --abi=eabi -c -m $(XDCCFGDIR)/$@.map -l $(ti.targets.elf.C66.rootDir)/lib/rts6600_elf.lib
	
helloWorld.xe66: export C_DIR=
helloWorld.xe66: PATH:=$(ti.targets.elf.C66.rootDir)/bin/;$(PATH)
helloWorld.xe66: Path:=$(ti.targets.elf.C66.rootDir)/bin/;$(PATH)

helloWorld.test test,e66 test: helloWorld.xe66.test

helloWorld.xe66.test:: helloWorld.xe66
ifeq (,$(_TESTLEVEL))
	@$(MAKE) -R -r --no-print-directory -f $(XDCROOT)/packages/xdc/bld/xdc.mak _TESTLEVEL=1 helloWorld.xe66.test
else
	@$(MSG) running $<  ...
	$(call EXEC.helloWorld.xe66, ) 
endif

clean,e66::
	-$(RM) .tmp,helloWorld.xe66,0,*


clean:: clean,e66

clean,e66::
	-$(RM) helloWorld.xe66
clean:: 
	-$(RM) package/cfg/helloWorld_pe66.pjt
%,copy:
	@$(if $<,,$(MSG) don\'t know how to build $*; exit 1)
	@$(MSG) cp $< $@
	$(RM) $@
	$(CP) $< $@
helloWorld_pe66.oe66,copy : package/cfg/helloWorld_pe66.oe66
helloWorld_pe66.se66,copy : package/cfg/helloWorld_pe66.se66

$(XDCCFGDIR)%.c $(XDCCFGDIR)%.h $(XDCCFGDIR)%.xdl: $(XDCCFGDIR)%.cfg .interfaces $(XDCROOT)/packages/xdc/cfg/Main.xs
	@$(MSG) "configuring $(_PROG_NAME) from $< ..."
	$(CONFIG) $(_PROG_XSOPTS) xdc.cfg $(_PROG_NAME) $(XDCCFGDIR)$*.cfg $(XDCCFGDIR)$*

.PHONY: release,xconfig_helloWorld
xconfig_helloWorld.tar: package/package.bld.xml
xconfig_helloWorld.tar: package/package.ext.xml
xconfig_helloWorld.tar: package/package.rel.dot
xconfig_helloWorld.tar: package/build.cfg
xconfig_helloWorld.tar: package/package.xdc.inc
ifeq (,$(MK_NOGENDEPS))
-include package/rel/xconfig_helloWorld.tar.dep
endif
package/rel/xconfig_helloWorld/xconfig_helloWorld/package/package.rel.xml:

xconfig_helloWorld.tar: package/rel/xconfig_helloWorld.xdc.inc package/rel/xconfig_helloWorld/xconfig_helloWorld/package/package.rel.xml
	@$(MSG) making release file $@ "(because of $(firstword $?))" ...
	-$(RM) $@
	$(call MKRELTAR,package/rel/xconfig_helloWorld.xdc.inc,package/rel/xconfig_helloWorld.tar.dep)


release release,xconfig_helloWorld: all xconfig_helloWorld.tar
clean:: .clean
	-$(RM) xconfig_helloWorld.tar
	-$(RM) package/rel/xconfig_helloWorld.xdc.inc
	-$(RM) package/rel/xconfig_helloWorld.tar.dep

clean:: .clean
	-$(RM) .libraries .libraries,*
clean:: 
	-$(RM) .dlls .dlls,*
#
# The following clean rule removes user specified
# generated files or directories.
#

ifneq (clean,$(MAKECMDGOALS))
ifeq (,$(wildcard package))
    $(shell $(MKDIR) package)
endif
ifeq (,$(wildcard package/cfg))
    $(shell $(MKDIR) package/cfg)
endif
ifeq (,$(wildcard package/lib))
    $(shell $(MKDIR) package/lib)
endif
ifeq (,$(wildcard package/rel))
    $(shell $(MKDIR) package/rel)
endif
ifeq (,$(wildcard package/internal))
    $(shell $(MKDIR) package/internal)
endif
ifeq (,$(wildcard package/external))
    $(shell $(MKDIR) package/external)
endif
endif
clean::
	-$(RMDIR) package

include custom.mak
clean:: 
	-$(RM) package/xconfig_helloWorld.pjt
