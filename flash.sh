# https://pigweed.googlesource.com/example/echo/+/refs/heads/main/tools/flash.py
ST_VENDOR_ID=0x0483
DISCOVERY_MODEL_ID=0x374B
openocd -f stm32f4xx.cfg -c "program $1.elf reset exit"
