set_project("AgentDemo")
set_languages("cxx17")

add_rules("mode.debug", "mode.release")

add_includedirs("include")

target("ag")
    set_kind("binary")

    add_files("src/main.cpp", "src/agent.cpp", "src/api_client.cpp")

    add_links("curl")
    add_syslinks("readline")

    add_includedirs("/usr/include", "/usr/include/x86_64-linux-gnu")

    set_targetdir("$(builddir)")
    set_basename("ag")