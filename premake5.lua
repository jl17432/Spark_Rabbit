workspace "SparkRabbit"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "SparkRabbit/vendor/GLFW/include"
IncludeDir["Glad"] = "SparkRabbit/vendor/Glad/include"
IncludeDir["ImGui"] = "SparkRabbit/vendor/imgui"
IncludeDir["glm"] = "SparkRabbit/vendor/glm"

include "SparkRabbit/vendor/GLFW"
include "SparkRabbit/vendor/Glad"
include "SparkRabbit/vendor/imgui"

project "SparkRabbit"
	location "SparkRabbit"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "PrecompileH.h"
	pchsource "SparkRabbit/src/PrecompileH.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"SR_PLATFORM_WINDOWS",
			"SR_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "SPARK_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SPARK_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "SPARK_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"SparkRabbit/vendor/spdlog/include",
		"SparkRabbit/src",
		"%{IncludeDir.glm}"
	}

	links
	{
		"SparkRabbit"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"SR_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "SPARK_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SPARK_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "SPARK_DIST"
		runtime "Release"
		optimize "On"