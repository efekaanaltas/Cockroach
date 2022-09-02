workspace "Cockroach"
    architecture "x64"
    startproject "Game"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Cockroach/vendor/GLFW/include"
IncludeDir["Glad"] = "Cockroach/vendor/Glad/include"
IncludeDir["ImGui"] = "Cockroach/vendor/imgui"
IncludeDir["glm"] = "Cockroach/vendor/glm"
IncludeDir["stb_image"] = "Cockroach/vendor/stb_image"

group "Dependencies"
    include "Cockroach/vendor/GLFW"
    include "Cockroach/vendor/Glad"
    include "Cockroach/vendor/imgui"
group ""

project "Cockroach"
    location "Cockroach"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "crpch.h"
    pchsource "Cockroach/src/crpch.cpp"

    files
    {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
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
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{prj.name}/vendor/miniaudio"
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
            "CR_PLATFORM_WINDOWS",
            "CR_BUILD_DLL",
            "GLFW_INCLUDE_NONE",
        }

    filter "configurations:Debug"
        defines "CR_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        defines "CR_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "CR_DIST"
        runtime "Release"
        optimize "on"

project "Accord"
    location "Accord"
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
    }

    includedirs
    {
        "%{prj.name}/src",
        "Cockroach/vendor/spdlog/include",
        "Cockroach/src",
        "Cockroach/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.GLFW}",
        "Cockroach/vendor/miniaudio",
    }

    links
    {
        "Cockroach"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "CR_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "CR_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        defines "CR_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "CR_DIST"
        runtime "Release"
        optimize "on"
