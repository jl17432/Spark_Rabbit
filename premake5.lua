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
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
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
		systemversion "latest"

		defines
		{
			"SR_PLATFORM_WINDOWS",
			"SR_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}


	filter "configurations:Debug"
		defines "SPARK_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SPARK_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SPARK_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"		
	cppdialect "C++17"
	staticruntime "on"

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
		systemversion "latest"

		defines
		{
			"SR_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "SPARK_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SPARK_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SPARK_DIST"
		runtime "Release"
		optimize "on"