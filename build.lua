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
	
	filter {"action:gmake","system:windows"}
		links {"glfw3" , "glew32" , "opengl32" , "gdi32"}
		objdir ("./obj/make/")
		pchheader ("src/preprocess.h")
		filter{"configurations:Debug"}
			targetname("mesh_debug")
			symbols("On")
		filter{"configurations:Release"}
			targetname("mesh")
			symbols("Off")
			optimize ("Size")  --* Size -> -O2 ; On ,Speed -> -O3

	filter {"action:gmake","system:linux"}
		links {"glfw" , "GLEW" , "GL" , "dl", "rt"}
		linkoptions{"-x11"}
		objdir ("./obj/")
		pchheader ("src/preprocess.h")
		filter{"configurations:Debug"}
			targetname("mesh_debug")
			symbols("On")
			defines{"_GLIBCXX_DEBUG"}
		filter{"configurations:Release"}
			targetname("mesh")
			symbols("Off")
			optimize ("Size")
			
	filter {"action:vs*"}
		targetname ("mesh_msc")
		systemversion ("10.0.17763.0")
		library_path = os.getenv("VC_LIBS")
		links {"glfw3","glew32","opengl32"}
		includedirs {cplus_include}
		libdirs {library_path}
		objdir ("./obj/ms/")
		filter {"configurations:Release"}
			optimize "On"
