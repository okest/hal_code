#!/bin/bash

#
# HAL层整体编译输出目录结构:
# hal_export
# ├── include
# └── lib
# 
HAL_OUTOUT_PATH="../output" # 输出目录
INC_DIR="include"
LIB_DIR="lib"

# 
# 使用CMake编译所有
# 
source /opt/poky/1.7.3/environment-setup-cortexa7hf-vfp-poky-linux-gnueabi
cd ../build/
rm -rf *
cmake .. 
make install
cd -

# 
# 新建输出目录
# 
if [ ! -d $HAL_OUTOUT_PATH ]; then
  mkdir $HAL_OUTOUT_PATH
else
  rm -rf $HAL_OUTOUT_PATH/*
fi

if [ ! -d $HAL_OUTOUT_PATH/$INC_DIR ]; then
  mkdir $HAL_OUTOUT_PATH/$INC_DIR
fi

if [ ! -d $HAL_OUTOUT_PATH/$LIB_DIR ]; then
  mkdir $HAL_OUTOUT_PATH/$LIB_DIR
fi

# 
# 将编译后的库和头文件复制到输出目录
# 
for file in ../source/* 
do
	if [ -d $file ]
	then
		if [ -f $file/bin/*.h ]; then
			cp -rf $file/bin/*.h $HAL_OUTOUT_PATH/$INC_DIR
		fi
		if [ -f $file/bin/*.so ]; then
			cp -rf $file/bin/*.so $HAL_OUTOUT_PATH/$LIB_DIR
		fi
		if [ -f $file/bin/*.a ]; then
			cp -rf $file/bin/*.a $HAL_OUTOUT_PATH/$LIB_DIR
		fi					
	fi
done

# 
# 将公用的库和头文件复制到输出目录
# 
if [ -n "../common/include/" ]; then
	cp -rf ../common/include/* $HAL_OUTOUT_PATH/$INC_DIR
fi
if [ -f ../common/lib/* ]; then
	cp -rf ../common/lib/* $HAL_OUTOUT_PATH/$LIB_DIR
fi

