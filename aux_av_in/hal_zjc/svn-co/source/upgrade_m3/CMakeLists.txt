# 设置cmake的最小版本
cmake_minimum_required( VERSION 2.8 ) 

# 设置工程名
project( hal_upgrade_m3-prj )

# 生成库文件名称
set( MODULE_LIB_NAME
	m3_upgrade_hal 
)	

# 设置安装目录	
set( CMAKE_INSTALL_PREFIX 
	${PROJECT_SOURCE_DIR} 
) 

# 头文件所在目录
set( COMMON_INC_PATH	
	${PROJECT_SOURCE_DIR}/../../common/include 
)		   	    
set( INC_PATH	
	${PROJECT_SOURCE_DIR}/src/include 
)        

# 输出文件所在目录 
set( EXPORT_FILES 
	${INC_PATH}/hal_upgrade_m3.h
	${PROJECT_BINARY_DIR}/lib${MODULE_LIB_NAME}.so
)

# 库文件所在目录 
set( COMMON_LIB_PATH	
	${PROJECT_SOURCE_DIR}/../../common/lib   
) 

# 项目宏文件所在目录
include( ${PROJECT_SOURCE_DIR}/../../configs/project.txt)

# 设置编译参数
add_definitions( 
	-DYOCTO_LINUX -DEGL_USE_WAYLAND
	"-Wall -O2 -fPIC" 
)

# 设置头文件所在路径
include_directories( 
	${COMMON_INC_PATH} 
	${INC_PATH} 
)

# 设置库文件所在路径
link_directories( 
	${COMMON_LIB_PATH}
)

# 设置生成库文件所需源文件
if( PRJ_RN4Y56 OR PRJ_RM4YC5 )
set( MODULE_LIB_SRC
	${PROJECT_SOURCE_DIR}/src/upgrade_m3_atlas7.c
)
endif()

# 生成库文件
add_library(
	${MODULE_LIB_NAME}			
	SHARED							
	${MODULE_LIB_SRC}	
)

# 安装输出文件
install(
	FILES 			${EXPORT_FILES}			
	DESTINATION		${CMAKE_CURRENT_SOURCE_DIR}/bin				
)
