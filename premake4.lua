if _ACTION ~= "gmake" and _ACTION ~= "clean" then
  _ACTION = "gmake"
end

newoption({trigger="examples",description="Creates makefiles for LASS examples"})

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
  
project "parser"
  language "C"
  flags {"StaticRuntime"}
  files {"CMOD/src/parser/lex.yy.c"}
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
  links {"lass", "parser", "pthread"}
  buildoptions {"-Wno-deprecated"}
  configuration "Debug" flags(DebugFlags) 
  configuration "Release" flags(ReleaseFlags)
  configuration "macosx" targetdir "bin"
  
--------------------------------------------------------------------------------
--                       The LASS Examples Directory
--------------------------------------------------------------------------------
if _OPTIONS["examples"] then
  all_files = os.matchfiles("LASS/examples/*.cpp")
  print("Detected " .. table.getn(all_files) .. " LASS examples")
  print("  from individual .cpp files in 'LASS/examples'")
  for i = 1, table.getn(all_files) do
    example_path = "LASS/examples/"
    example_filename = all_files[i]
    example_name = path.getbasename(all_files[i])    
 
    project(example_name)
      language "C++"
      kind "ConsoleApp"
      flags {"StaticRuntime"}
      files {example_path .. example_name .. ".cpp"}
      includedirs {"LASS/src/"}
      libdirs {"lib/"}
      links {"lass", "pthread"}
      buildoptions {"-Wno-deprecated"}
      postbuildcommands {"rm " .. example_name .. ".make"}
      targetdir("bin/")
      configuration "Debug"
        flags(DebugFlags)
        objdir("obj/Debug/" .. example_name)
        postbuildcommands {"rm -rf obj/Debug/" .. example_name}
      configuration "Release"
        flags(ReleaseFlags)
        objdir("obj/Release/" .. example_name)
        postbuildcommands {"rm -rf obj/Release/" .. example_name}
  end
end

if _ACTION == "clean" then
  print("Removing target and object directories.")
  os.execute("rm -f *.make")
  os.rmdir("lib")
  os.rmdir("bin")
  os.rmdir("obj")
end

