workspace "NES-Emulator"
    architecture "x64"
    configurations { "Debug", "Release" }

project "NES-Emulator"
    location "src"
    kind "ConsoleApp"
    language "C++"

    targetdir "bin/${cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}"

    files {
        "./src/**.h",
        "./src/**.cpp"
    }

    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "On"