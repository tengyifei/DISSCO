DebugFlags = {"Symbols", "NoPCH", "NoManifest"}
ReleaseFlags = {}

solution "dissco"
  configurations {"Release", "Debug"}
  
--[[
project "bellebonnesage"
  language "C++"
  flags {"StaticRuntime", "ExtraWarnings"}
  files {"bbs/*.cpp", "bbs/*.h"}
  excludes {"bbs/lua.*"}
  kind "StaticLib"
  configuration "Debug" flags(DebugFlags) 
  configuration "Release" flags(ReleaseFlags)
  targetdir "lib"]]

project "lass"
  language "C++"
  flags {"StaticRuntime"}
  files {"LASS/src/*.cpp", "LASS/src/*.h"}
  kind "StaticLib"
  targetdir "lib"
  buildoptions {"-Wno-deprecated"}
  configuration "Debug" flags(DebugFlags) 
  configuration "Release" flags(ReleaseFlags)
  
project "lexyacc"
  language "C"
  flags {"StaticRuntime"}
  files {"CMOD/src/lexyacc/lex.yy.c"}
  kind "StaticLib"
  targetdir "lib"
  configuration "Debug" flags(DebugFlags) 
  configuration "Release" flags(ReleaseFlags)

project "cmod"
  language "C++"
  flags {"StaticRuntime"}
  files {"CMOD/src/**.cpp", "CMOD/src/**.h"}
  kind "ConsoleApp"
  libdirs {"lib"}
  links {"lass", "lexyacc", "pthread"}
  buildoptions {"-Wno-deprecated"}
  configuration "Debug" flags(DebugFlags) 
  configuration "Release" flags(ReleaseFlags)
  configuration "macosx" targetdir "bin"

if _ACTION == "clean" then
  print("Removing target and object directories.")
  os.rmdir("lib")
  os.rmdir("bin")
  os.rmdir("obj")
end

