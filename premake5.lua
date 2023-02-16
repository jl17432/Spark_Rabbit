workspace "SparkRabbit"
    architecture "x64"
    configurations { "Debug", "Release","Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDirs = {}
IncludeDirs["GLFW"] = "SparkRabbit/ThirdParties/GLFW/include"
IncludeDirs["GLAD"] = "SparkRabbit/ThirdParties/GLAD/include"
IncludeDirs["Imgui"] = "SparkRabbit/ThirdParties/Imgui"

include "SparkRabbit/ThirdParties/GLFW"
include "SparkRabbit/ThirdParties/GLAD"
include "SparkRabbit/ThirdParties/Imgui"

project "SparkRabbit"
    location "SparkRabbit"
    kind "SharedLib"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

    includedirs{"%{prj.name}/ThirdParties/spdlog/include"}
    includedirs{"%{prj.name}/src"}
    includedirs{"%{IncludeDirs.GLFW}"}
    includedirs{"%{IncludeDirs.GLAD}"}
    includedirs{"%{IncludeDirs.Imgui}"}

    links{
        "GLAD",
        "GLFW",
        "Imgui",
        "opengl32.lib"
    }

    pchheader "PreCompile.h"
    pchsource "SparkRabbit/src/PreCompile.cpp"


    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        defines {"SR_PLATFORM_WINDOWS", "SR_BUILD_DLL","GLFW_INCLUDE_NONE"}


    postbuildcommands{
        ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
    }

    filter { "configurations:Debug" }
        defines { "SR_DEBUG" }
        buildoptions "/MDd"
        symbols "On"

    filter { "configurations:Release" }
        defines { "SR_RELEASE" }
        buildoptions "/MD"
        optimize "On"

    filter { "configurations:Dist" }
        defines { "SR_DIST" }
        buildoptions "/MD"
        optimize "On"


project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

    includedirs { "SparkRabbit/ThirdParties/spdlog/include", "SparkRabbit/src"}

    links{"SparkRabbit"}

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        defines {"SR_PLATFORM_WINDOWS"}


    filter { "configurations:Debug" }
        defines { "SR_DEBUG" }
        buildoptions "/MDd"
        symbols "On"

    filter { "configurations:Release" }
        defines { "SR_RELEASE" }
        buildoptions "/MD"
        optimize "On"

    filter { "configurations:Dist" }
        defines { "SR_DIST" }
        buildoptions "/MD"
        optimize "On"