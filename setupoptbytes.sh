echo "00 00 ff 00 ff 08 f7 00 ff 00 ff 00 ff 00 ff" | xxd -r -p > option_bytes.bin
stm8flash -c stlinkv2 -p stm8s105k4 -s opt -w option_bytes.bin
rm option_bytes.bin
