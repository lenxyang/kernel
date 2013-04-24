
output=
subst:
	output=$(subst ee,EE,IeeE)

names := a b c d
foreach:
	files=$(foreach n,$(names),$(shell echo ${n}))
