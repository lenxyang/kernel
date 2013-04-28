
BASE_PATH=$(dirname `which $0`)

(cd  ${BASE_PATH} && "/cygdrive/d/Tools/Bochs-2.6/bochs_gdbstub.exe" -q -f "../vm/bochsrc.bxrc")
