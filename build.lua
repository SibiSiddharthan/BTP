cplus_include = os.getenv("CPLUS_INCLUDE_PATH")
library_path = os.getenv("LIBRARY_PATH")

workspace  "BTP"
	configurations {"Debug","Release"}
	architecture "x64"

project "Mesh"
	kind "ConsoleApp"
	language "C++"
	targetdir (".")
	files {"src/*.cpp","src/*.h"}
	cppdialect "C++17"
	
	filter {"action:gmake"}
		links {"glfw3" , "glew32" , "opengl32" , "gdi32"}
		objdir ("./obj/make/")
		filter{"configurations:Debug"}
			buildoptions{"-g"}
			targetname("mesh_debug")
		filter{"configurations:Release"}
			buildoptions {"-march=native"}
			
	filter {"action:vs*"}
		targetname ("mesh_msc")
		systemversion ("10.0.17763.0")
		library_path = os.getenv("VC_LIBS")
		links {"glfw3","glew32","opengl32"}
		includedirs {cplus_include}
		libdirs {library_path}
		objdir ("./obj/ms/")
		filter {"configurations:Release"}
			optimize "Full"
