# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\HLS\digitclassifier\digitClassifier_vitis\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\HLS\digitclassifier\digitClassifier_vitis\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {digitClassifier_vitis}\
-hw {C:\HLS\digitclassifier\project_vivado\design_1_wrapper.xsa}\
-proc {microblaze_0} -os {standalone} -out {C:/HLS/digitclassifier}

platform write
platform generate -domains 
platform active {digitClassifier_vitis}
bsp reload
bsp reload
platform generate
platform generate
platform active {digitClassifier_vitis}
platform config -updatehw {C:/HLS/digitclassifier/project_vivado/design_1_wrapper.xsa}
platform generate -domains 
bsp reload
bsp write
platform config -updatehw {C:/HLS/digitclassifier/project_vivado/design_1_wrapper.xsa}
platform generate -domains 
