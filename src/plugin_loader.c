#include "plugin_loader.h"

bool load_plugin(const char *zip_path, PluginDetail *detail, lua_State **L)
{
    int err = 0;
    zip_t *z = zip_open(zip_path, ZIP_RDONLY, &err);
    if (!z)
    {
        printf("Failed to open ZIP file: %s\n", zip_strerror((zip_t *)(void *)&err));
        return false;
    }

    if (!read_app_properties(z, detail))
    {
        printf("Failed to read app.properties.\n");
        zip_close(z);
        return false;
    }

    if (!verify_signature(detail->signature))
    {
        printf("Plugin signature verification failed.\n");
        zip_close(z);
        return false;
    }

    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat(z, "main.lua", 0, &st) != 0)
    {
        printf("main.lua not found in the plugin.\n");
        zip_close(z);
        return false;
    }

    char *main_lua_content = (char *)malloc(st.size + 1);
    struct zip_file *f = zip_fopen(z, "main.lua", 0);
    zip_fread(f, main_lua_content, st.size);
    main_lua_content[st.size] = '\0';
    zip_fclose(f);

    *L = luaL_newstate();
    luaL_openlibs(*L);

    if (luaL_loadbuffer(*L, main_lua_content, st.size, "main.lua") != 0)
    {
        printf("Failed to load main.lua: %s\n", lua_tostring(*L, -1));
        lua_close(*L);
        free(main_lua_content);
        zip_close(z);
        return false;
    }

    if (lua_pcall(*L, 0, 0, 0) != 0)
    {
        printf("Failed to execute main.lua: %s\n", lua_tostring(*L, -1));
        lua_close(*L);
        free(main_lua_content);
        zip_close(z);
        return false;
    }

    // 读取 [env] 部分并传递给 Lua
    read_env_params(z, *L);

    // 获取 main 函数
    lua_getglobal(*L, "main");
    if (lua_isfunction(*L, -1) == 0)
    {
        printf("main function not found in main.lua.\n");
        lua_close(*L);
        free(main_lua_content);
        zip_close(z);
        return false;
    }

    // 传递参数并调用 main 函数
    if (lua_pcall(*L, 1, 0, 0) != 0)
    {
        printf("Failed to execute main function: %s\n", lua_tostring(*L, -1));
        lua_close(*L);
        free(main_lua_content);
        zip_close(z);
        return false;
    }

    free(main_lua_content);
    zip_close(z);

    return true;
}