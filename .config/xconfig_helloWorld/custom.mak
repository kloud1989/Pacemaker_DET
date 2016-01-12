## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd \
  package/cfg/helloWorld_pe66.oe66 \

linker.cmd: package/cfg/helloWorld_pe66.xdl
	$(SED) 's"^\"\(package/cfg/helloWorld_pe66cfg.cmd\)\"$""\"D:/CCS_PRJ/Pacemaker_DET/.config/xconfig_helloWorld/\1\""' package/cfg/helloWorld_pe66.xdl > $@
