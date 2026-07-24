old: 0,8,2
new: 0,8,3
files: [
    %VERSION                [{echo "$v"}]
    %libevolvotron/common.h [{APP_VERSION "$v"}]
    %dist/evolvotron.spec   ["Version: $v"]
]
