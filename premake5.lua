workspace "Cockroach"
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
IncludeDir["GLFW"] = "Cockroach/vendor/GLFW/include"
IncludeDir["Glad"] = "Cockroach/vendor/Glad/include"
IncludeDir["ImGui"] = "Cockroach/vendor/imgui"
IncludeDir["glm"] = "Cockroach/vendor/glm"

include "Cockroach/vendor/GLFW"
include "Cockroach/vendor/Glad"
include "Cockroach/vendor/imgui"

project "Cockroach"
    location "Cockroach"
    kind "SharedLib"
    language "C++"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "crpch.h"
    pchsource "Cockroach/src/crpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
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
            "CR_PLATFORM_WINDOWS",
            "CR_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
        }

    filter "configurations:Debug"
        defines "CR_DEBUG"
        runtime "Debug"
        symbols "On"
        
    filter "configurations:Release"
        defines "CR_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "CR_DIST"
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
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
    }

    includedirs
    {
        "Cockroach/vendor/spdlog/include",
        "Cockroach/src",
        "%{IncludeDir.glm}"
    }

    links
    {
        "Cockroach"
    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"

        defines
        {
            "CR_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "CR_DEBUG"
        runtime "Debug"
        symbols "On"
        
    filter "configurations:Release"
        defines "CR_RELEASE"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines "CR_DIST"
        runtime "Release"
        optimize "On"