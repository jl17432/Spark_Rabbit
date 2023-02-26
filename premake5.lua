workspace "SparkRabbit"
    architecture "x64"
    startproject "Editor"
    configurations { "Debug", "Release","Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDirs = {}
IncludeDirs["GLFW"] = "SparkRabbit/ThirdParties/GLFW/include"
IncludeDirs["GLAD"] = "SparkRabbit/ThirdParties/GLAD/include"
IncludeDirs["Imgui"] = "SparkRabbit/ThirdParties/Imgui"
IncludeDirs["glm"] = "SparkRabbit/ThirdParties/glm/glm"
IncludeDirs["assimp"] = "SparkRabbit/ThirdParties/assimp/include"

group "Dependencies"
    include "SparkRabbit/ThirdParties/GLFW"
    include "SparkRabbit/ThirdParties/GLAD"
    include "SparkRabbit/ThirdParties/Imgui"
    include "SparkRabbit/ThirdParties/assimp"
group ""

project "SparkRabbit"
    location "SparkRabbit"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp", "%{prj.name}/ThirdParties/glm/glm/**.hpp","%{prj.name}/ThirdParties/glm/glm/**.inl"}

    includedirs{"%{prj.name}/ThirdParties/spdlog/include"}
    includedirs{"%{prj.name}/src"}
    includedirs{"%{IncludeDirs.GLFW}"}
    includedirs{"%{IncludeDirs.GLAD}"}
    includedirs{"%{IncludeDirs.Imgui}"}
    includedirs{"%{IncludeDirs.glm}"}
    includedirs{"%{IncludeDirs.assimp}"}

    links{
        "GLAD",
        "GLFW",
        "Imgui",
        "assimp",
        "opengl32.lib"
    }

    pchheader "PreCompile.h"
    pchsource "SparkRabbit/src/PreCompile.cpp"


    filter "system:windows"
        systemversion "latest"
        defines {"SR_PLATFORM_WINDOWS", "SR_BUILD_DLL","GLFW_INCLUDE_NONE"}


    filter { "configurations:Debug" }
        defines { "SR_DEBUG" }
        runtime "Debug"
        symbols "on"

    filter { "configurations:Release" }
        defines { "SR_RELEASE" }
        runtime "Release"
        optimize "on"

    filter { "configurations:Dist" }
        defines { "SR_DIST" }
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
    files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

    includedirs { "SparkRabbit/ThirdParties/spdlog/include", "SparkRabbit/src","SparkRabbit/ThirdParties/Imgui"}

    links{"SparkRabbit"}

    filter "system:windows"
        systemversion "latest"
        defines {"SR_PLATFORM_WINDOWS"}


    filter { "configurations:Debug" }
        defines { "SR_DEBUG" }
        runtime "Debug"
        symbols "on"

    filter { "configurations:Release" }
        defines { "SR_RELEASE" }
        runtime "Release"
        optimize "on"

    filter { "configurations:Dist" }
        defines { "SR_DIST" }
        runtime "Release"
        optimize "on"



project "Editor"
    location "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"


    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp" }

    includedirs { "SparkRabbit/ThirdParties/spdlog/include", "SparkRabbit/src","SparkRabbit/ThirdParties/Imgui"}

    links{"SparkRabbit"}

    filter "system:windows"
        systemversion "latest"
        defines {"SR_PLATFORM_WINDOWS"}


    filter { "configurations:Debug" }
        defines { "SR_DEBUG" }
        runtime "Debug"
        symbols "on"

    filter { "configurations:Release" }
        defines { "SR_RELEASE" }
        runtime "Release"
        optimize "on"

    filter { "configurations:Dist" }
        defines { "SR_DIST" }
        runtime "Release"
        optimize "on"