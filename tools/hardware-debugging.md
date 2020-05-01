# Hardware debugging in vscode
Check configuration in launch.json for correct paths of tools
Check whehther the GDB launch has no errors by running it manually:
`Launching GDB: "/home/elco/source/gcc-arm-none-eabi-5_3-2016q1/bin/arm-none-eabi-gdb-py" "-q" "--interpreter=mi2" "-command=./tools/system.gdbinit" "-command=./tools/arm-pretty.gdbinit"`

If you get this error: `error while loading shared libraries: libncurses.so.5: cannot open shared object file: No such file or directory` 
Fix it by simlinking the newer version:
`sudo ln -s /lib/i386-linux-gnu/libncurses.so.6 /lib/i386-linux-gnu/libncurses.so.5`



## Cloning eeprom data for debugging
On problem Spark:
```
docker pull brewblox/firmware-flasher:edge
docker run --privileged -it --rm -v /dev:/dev brewblox/firmware-flasher:edge trigger-dfu
docker run --privileged -it --rm -v /dev:/dev -v /home/pi/brewblox/:/dump -v /dev:/dev brewblox/firmware-flasher:edge -c "dfu-util -d 2b04:d006 -a 0 -s 0x800C000:0x18000 -U /dump/eeprom.bin"
curl https://bashupload.com/eeprom.bin --data-binary @/home/pi/brewblox/eeprom.bin
```

On a test spark with a hardware debugger:
st-flash write eeprom.bin 0x800c000
flash modules and app again