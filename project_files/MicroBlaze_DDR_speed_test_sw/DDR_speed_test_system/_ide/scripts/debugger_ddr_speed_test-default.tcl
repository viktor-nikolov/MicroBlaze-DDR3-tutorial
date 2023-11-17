# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: C:\MicroZed_projects\ArtyA7_MicroBlaze_demo\MicroBlaze_DDR_speed_test_sw\DDR_speed_test_system\_ide\scripts\debugger_ddr_speed_test-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source C:\MicroZed_projects\ArtyA7_MicroBlaze_demo\MicroBlaze_DDR_speed_test_sw\DDR_speed_test_system\_ide\scripts\debugger_ddr_speed_test-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "Digilent Arty A7-35T 210319B2683AA" && level==0 && jtag_device_ctx=="jsn-Arty A7-35T-210319B2683AA-0362d093-0"}
fpga -file C:/MicroZed_projects/ArtyA7_MicroBlaze_demo/MicroBlaze_DDR_speed_test_sw/DDR_speed_test/_ide/bitstream/download.bit
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
loadhw -hw C:/MicroZed_projects/ArtyA7_MicroBlaze_demo/MicroBlaze_DDR_speed_test_sw/system/export/system/hw/system_wrapper.xsa -regs
configparams mdm-detect-bscan-mask 2
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
rst -system
after 3000
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
dow C:/MicroZed_projects/ArtyA7_MicroBlaze_demo/MicroBlaze_DDR_speed_test_sw/DDR_speed_test/Release/DDR_speed_test.elf
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
con
