############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
## Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
############################################################
open_project digitclassifier
set_top digit_mlp
add_files digitclassifier/digit_mlp.cpp
add_files -tb digitclassifier/digit_mlp_tb.cpp
add_files -tb digitclassifier/test_set.bin
open_solution "solution1" -flow_target vivado
set_part {xc7a35ticsg324-1L}
create_clock -period 10 -name default
config_export -output C:/HLS/digitclassifier/RTLexported
#source "./digitclassifier/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog -output C:/HLS/digitclassifier/RTLexported
