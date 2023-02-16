project "Imgui"
	kind "StaticLib"
	language "C++"
	staticruntime "On"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
        "imgui_demo.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
        "imstb_textdit.h",
        "imstb_truetype.h",
        "imgui_tables.cpp"


	}


	filter "system:windows"
		systemversion "latest"
        cppdialect "C++17"

	filter {"system:windows","configurations:Release"}
        buildoptions "/MT"
