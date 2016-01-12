## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd \
  package/cfg/Pacemaker_DET_pe66.oe66 \

linker.cmd: package/cfg/Pacemaker_DET_pe66.xdl
	$(SED) 's"^\"\(package/cfg/Pacemaker_DET_pe66cfg.cmd\)\"$""\"D:/CCS_PRJ/Pacemaker_DET/Debug/configPkg/\1\""' package/cfg/Pacemaker_DET_pe66.xdl > $@
