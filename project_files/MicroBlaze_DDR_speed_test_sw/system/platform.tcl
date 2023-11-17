# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\MicroZed_projects\ArtyA7_MicroBlaze_demo\MicroBlaze_DDR_speed_test_sw\system\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\MicroZed_projects\ArtyA7_MicroBlaze_demo\MicroBlaze_DDR_speed_test_sw\system\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {system}\
-hw {C:\MicroZed_projects\ArtyA7_MicroBlaze_demo\ArtyA7_MicroBlaze_demo_hw\system_wrapper.xsa}\
-proc {microblaze_0} -os {standalone} -out {C:/MicroZed_projects/ArtyA7_MicroBlaze_demo/MicroBlaze_DDR_speed_test_sw}

platform write
platform generate -domains 
platform active {system}
bsp reload
bsp reload
platform generate
platform active {system}
platform config -updatehw {C:/MicroZed_projects/ArtyA7_MicroBlaze_demo/ArtyA7_MicroBlaze_demo_hw/system_wrapper.xsa}
platform clean
platform generate
platform clean
