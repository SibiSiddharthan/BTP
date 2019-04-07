import os
from pathlib import Path
import sys

base_dir = os.getcwd()
install_dir = Path("{}".format(base_dir))
build_dir = Path("{}/build/external".format(base_dir))
# print(install_dir)
glew_dir = Path("{}/external/glew/build/cmake".format(base_dir))
glfw_dir = Path("{}/external/glfw".format(base_dir))
glm_dir = Path("{}/external/glm".format(base_dir))
imgui_dir = Path("{}/external/imgui".format(base_dir))

if os.name == "nt":
	make = "mingw32-make.exe"
	generator = {"vs": "Visual Studio 15 2017 Win64",
				 "mingw": "MinGW Makefiles", "ninja": "Ninja"}
elif os.name == "posix":
	make = "make"
	generator = {"make": "Unix Makefiles", "ninja": "Ninja"}

if len(sys.argv) == 2:
	gen = sys.argv[1]
	if gen not in generator:
		for key, value in generator.items():
			print("{} \t {}".format(key, value))
		sys.exit()
else:
	print("python [script] [generator]\n")
	sys.exit()

# Configure glfw
print("Configuring GLFW\n")
os.system("cmake -G\"{}\" -S {} -B {}/glfw -DCMAKE_INSTALL_PREFIX:PATH={} -DCMAKE_BUILD_TYPE=Release -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DCMAKE_C_FLAGS_RELEASE:STRING=\"-O2 -DNDEBUG\"".format(
	generator[gen], glfw_dir, build_dir, install_dir))
# Build glfw
os.system("cmake --build {}/glfw -- -j6".format(build_dir))
# Install glfw
os.system("cmake --build {}/glfw --target install ".format(build_dir))

# Configure glew
print("Configuring GLEW\n")
os.system("cd {}/external/glew && {} extensions".format(base_dir, make))
os.system("cmake -G\"{}\" -S {} -B {}/glew -DCMAKE_INSTALL_PREFIX:PATH={} -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE:STRING=\"-O2 -DNDEBUG\"".format(
	generator[gen], glew_dir, build_dir, install_dir))
# Build glew
os.system("cmake --build {}/glew -- -j6".format(build_dir))
# Install glew
os.system("cmake --build {}/glew --target install ".format(build_dir))

# Configure glm
print("Configuring GLM\n")
os.system("cmake -G\"{}\" -S {} -B {}/glm -DCMAKE_INSTALL_PREFIX:PATH={} -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE:STRING=\"-O2 -DNDEBUG -mavx\" -DGLM_TEST_ENABLE=OFF".format(
	generator[gen], glm_dir, build_dir, install_dir))
# Build glm
os.system("cmake --build {}/glm -- -j6".format(build_dir))
# Install glm
os.system("cmake --build {}/glm --target install ".format(build_dir))

# Configure imgui
print("Configuring Dear ImGui\n")
os.system("cmake -G\"{}\" -S {} -B {}/imgui -DCMAKE_INSTALL_PREFIX:PATH={} -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE:STRING=\"-O2 -DNDEBUG \" ".format(
	generator[gen], imgui_dir, build_dir, install_dir))
# Build glm
os.system("cmake --build {}/imgui -- -j6".format(build_dir))
# Install glm
os.system("cmake --build {}/imgui --target install ".format(build_dir))