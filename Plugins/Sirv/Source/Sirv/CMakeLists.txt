cmake_minimum_required(VERSION 3.15)
project(sirv_ros2_script)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rcutils REQUIRED)
find_package(sirv_msgs REQUIRED)
find_package(sirv_dev REQUIRED)
find_package(sirv_ros REQUIRED)
find_package(rmw REQUIRED)
find_package(rosidl_default_generators REQUIRED)

find_package(tf2_geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)

find_package(ament_cmake_ros REQUIRED)

find_package(libstatistics_collector REQUIRED)
find_package(rcl REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(rcl_yaml_param_parser REQUIRED)
find_package(rcpputils REQUIRED)

find_package(rosgraph_msgs REQUIRED)
find_package(rosidl_runtime_cpp REQUIRED)
find_package(rosidl_typesupport_c REQUIRED)
#find_package(rosidl_typesupport_cpp REQUIRED)
find_package(statistics_msgs REQUIRED)
find_package(tracetools REQUIRED)

find_package(Eigen3 REQUIRED)


#find_package(image_transport REQUIRED)
#find_package(sensor_msgs REQUIRED)

set(PACKAGES
	#ament_cmake
	#builtin_interfaces
	#rclcpp
	sirv_msgs
	sirv_dev
	sirv_ros
	EIGEN3
	#tf2_geometry_msgs
  	#tf2  
	#rmw
	#rcl_yaml_param_parser
	#rosidl_default_generators
	#rcl_interfaces
	#libstatistics_collector
	#rcpputils
	#rcutils
	#rcl
	#rosgraph_msgs
	#rosidl_typesupport_cpp
	#rosidl_typesupport_c
	#statistics_msgs
	#tracetools
	#pluginlib
	#image_transport
    	#sensor_msgs
        )        

set(include_paths)
set(libraries_paths)

# Process all paths from packages
foreach(library ${PACKAGES})
        if(${${library}_FOUND})
		foreach(path ${${library}_INCLUDE_DIRS})		  
			#string(CONCAT inc "I " ${path})
			#message(${path})
			#list(APPEND include_paths ${path})
			list_append_unique(include_paths ${path})
		endforeach()
		foreach(path ${${library}_LIBRARIES})
			if(${path} MATCHES ".so")
				#string(CONCAT lib_name "L " ${path})
			 	#message(${lib_name})
			 	#list(APPEND libraries_paths ${path})
			 	list_append_unique(libraries_paths ${path})
			endif()
		endforeach()
		foreach(path ${${library}_LIBRARIES})
			if(${path} MATCHES ".a")
				#string(CONCAT lib_name "L " ${path})
			 	#message(${lib_name})
			 	#list(APPEND libraries_paths ${path})
			 	list_append_unique(libraries_paths ${path})
			endif()
		endforeach()
	else()
		message(FATAL_ERROR "::" ${library} " NOT FOUND::")		
	endif()
endforeach()

# Output paths to UE5
foreach(path ${include_paths})		  
	string(CONCAT inc "I-" ${path})
	#message(${inc})
	execute_process(COMMAND echo ${inc})
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${path}
                                                           ${CMAKE_SOURCE_DIR}/../../SirvDependencies/include)
endforeach()
foreach(path ${libraries_paths})
	string(CONCAT lib_name "L-" ${path})
	#message(${lib_name})
	execute_process(COMMAND echo ${lib_name})
	file(COPY ${path}  DESTINATION ${CMAKE_SOURCE_DIR}/../../SirvDependencies/lib)
endforeach()


INSTALL( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*")
    
