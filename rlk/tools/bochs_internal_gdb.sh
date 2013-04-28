
BASE_PATH=$(dirname `which $0`)
(cd  ${BASE_PATH} && "/cygdrive/d/Tools/Bochs-2.6/bochsdbg.exe" -q -f "../vm/internaldebug.bxrc")
