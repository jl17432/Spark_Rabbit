workspace "SparkRabbit"
	architecture "x64"
	startproject "Sandbox"
	targetdir "build"

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
IncludeDir["entt"] = "SparkRabbit/vendor/entt/include"
IncludeDir["yaml_cpp"] = "SparkRabbit/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "SparkRabbit/vendor/Box2D/include"
IncludeDir["bullet3"] = "SparkRabbit/vendor/bullet3/include"

include "SparkRabbit/vendor/GLFW"
include "SparkRabbit/vendor/Glad"
include "SparkRabbit/vendor/imgui"
include "SparkRabbit/vendor/yaml-cpp"
include "SparkRabbit/vendor/Box2D"

LibraryDir = {}
LibraryDir["Bullet3Collision"] = "vendor/bullet3/lib/%{cfg.buildcfg}/Bullet3Collision_Debug.lib"
LibraryDir["Bullet3Common"] = "vendor/bullet3/lib/%{cfg.buildcfg}/Bullet3Common_Debug.lib"
LibraryDir["Bullet3Dynamics"] = "vendor/bullet3/lib/%{cfg.buildcfg}/Bullet3Dynamics_Debug.lib"
LibraryDir["Bullet3Geometry"] = "vendor/bullet3/lib/%{cfg.buildcfg}/Bullet3Geometry_Debug.lib"
LibraryDir["Bullet3OpenCL_clew"] = "vendor/bullet3/lib/%{cfg.buildcfg}/Bullet3OpenCL_clew_Debug.lib"
LibraryDir["BulletCollision"] = "vendor/bullet3/lib/%{cfg.buildcfg}/BulletCollision_Debug.lib"
LibraryDir["BulletDynamics"] = "vendor/bullet3/lib/%{cfg.buildcfg}/BulletDynamics_Debug.lib"
LibraryDir["BulletSoftBody"] = "vendor/bullet3/lib/%{cfg.buildcfg}/BulletSoftBody_Debug.lib"
LibraryDir["BulletWorldImporter"] = "vendor/bullet3/lib/%{cfg.buildcfg}/BulletWorldImporter_Debug.lib"
LibraryDir["LinearMath"] = "vendor/bullet3/lib/%{cfg.buildcfg}/LinearMath_Debug.lib"

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
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.c",
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
		"%{prj.name}/vendor",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{prj.name}/vendor/assimp/include",
		"%{prj.name}/vendor/stb/include",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.Box2D}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib",
		"%{LibraryDir.Bullet3Collision}",
		"Box2D",
       	"%{LibraryDir.Bullet3Common}",
       	"%{LibraryDir.Bullet3Dynamics}",
       	"%{LibraryDir.Bullet3Geometry}",
      	"%{LibraryDir.Bullet3OpenCL_clew}",
       	"%{LibraryDir.BulletCollision}",
       	"%{LibraryDir.BulletDynamics}",
        	"%{LibraryDir.BulletSoftBody}",
        	"%{LibraryDir.BulletWorldImporter}",
        	"%{LibraryDir.LinearMath}"
	}

	defines { "BT_USE_STATIC_LIBS" }
      pchheader "PrecompileH.h"
      pchsource "SparkRabbit/src/PrecompileH.cpp"

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"SR_PLATFORM_WINDOWS",
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
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.c"
	}

	includedirs
	{
		"SparkRabbit/vendor/spdlog/include",
		"SparkRabbit/vendor",
		"SparkRabbit/src",
		"%{prj.name}/src",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}"
	}

	links
	{
		"SparkRabbit"
	}
	postbuildcommands 
	{
		'{COPY} "../Sandbox/assets" "%{cfg.targetdir}/assets"'
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

		links
		{
			"SparkRabbit/vendor/assimp/bin/Debug/assimp-vc141-mtd.lib"
		}

		postbuildcommands 
		{
			'{COPY} "../SparkRabbit/vendor/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Release"
		defines "SPARK_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"SparkRabbit/vendor/assimp/bin/Release/assimp-vc141-mt.lib"
		}

		postbuildcommands 
		{
			'{COPY} "../SparkRabbit/vendor/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "SPARK_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"SparkRabbit/vendor/assimp/bin/Release/assimp-vc141-mt.lib"
		}

		postbuildcommands 
		{
			'{COPY} "../SparkRabbit/vendor/assimp/bin/Release/assimp-vc141-mtd.dll" "%{cfg.targetdir}"'
		}