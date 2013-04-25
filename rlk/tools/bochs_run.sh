BASE_PATH=$(dirname `which $0`)

(cd  ${BASE_PATH} && "/cygdrive/d/Program Files (x86)/Bochs-2.6/bochs.exe" -q -f   "../vm/just_run.bxrc")
