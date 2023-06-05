# ELS_final_riscv_sw

Software part of K-means segmentation module execute in riscv-vp.

There are two branches in both software and platform part. 
The branches are main and multi, where main is simulation with one core and multi is simulation with two cores.
change both repo to same branch before executing. However, multi branch still can't work.
> $ git checkout main   

or  

> $ git checkout multi  

## Execution
1. Clone the software part in riscv-vp as lab 8.
> $ cd $EE6470/riscv-vp/sw/  
> $ git clone https://github.com/Thuniinae/ELS_final_riscv_sw.git
2. Clone the platform part as well
> $ cd $EE6470/riscv-vp/vp/src/platform/  
> $ git clone https://github.com/Thuniinae/ELS_final_riscv_vp.git
3. Login Ubuntu docker provided by the course
4.  build the platform
> $ cd $EE6470/riscv-vp/vp/build  
> $ cmake .. && make install
7. build and execute the software
> $ cd $EE6470/riscv-vp/sw/ELS_final_riscv/  
> $ make && make sim

The output should be lena_color_512_seg.bmp which is K-means segmentation result of lena_color_512.bmp
