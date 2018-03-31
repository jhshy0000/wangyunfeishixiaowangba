SET PATH=D:\Keil_v5\ARM\ARMCC\Bin;C:\ProgramData\Oracle\Java\javapath;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\Nordic Semiconductor\nrf5x\bin\;D:\sam-ba_2.12\drv\;D:\sam-ba_2.12
SET CPU_TYPE=STM32F103VC
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x00B71B00
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\core_cm3.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\system_stm32f10x.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via "..\output\startup_stm32f10x_hd._ia"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\stm32f10x_it.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\output\test.__i"
