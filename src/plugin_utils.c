#include "plugin_utils.h"
// 读取 app.properties 文件内容
bool read_app_properties(zip_t *z, PluginDetail *detail)
{
    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat(z, "app.properties", 0, &st) != 0)
    {
        return false;
    }

    char *buf = (char *)malloc(st.size + 1);
    struct zip_file *f = zip_fopen(z, "app.properties", 0);
    zip_fread(f, buf, st.size);
    buf[st.size] = '\0';
    zip_fclose(f);

    char *line = strtok(buf, "\n");
    char *section = NULL;
    while (line != NULL)
    {
        if (line[0] == '[' && line[strlen(line) - 1] == ']')
        {
            section = line + 1;
            section[strlen(section) - 1] = '\0';
        }
        else if (section != NULL && strcmp(section, "application") == 0)
        {
            char *key = strtok(line, "=");
            char *value = strtok(NULL, "=");
            if (strcmp(key, "name") == 0)
            {
                strncpy(detail->name, value, sizeof(detail->name) - 1);
            }
            else if (strcmp(key, "version") == 0)
            {
                strncpy(detail->version, value, sizeof(detail->version) - 1);
            }
            else if (strcmp(key, "author") == 0)
            {
                strncpy(detail->author, value, sizeof(detail->author) - 1);
            }
            else if (strcmp(key, "description") == 0)
            {
                strncpy(detail->description, value, sizeof(detail->description) - 1);
            }
            else if (strcmp(key, "signature") == 0)
            {
                strncpy(detail->signature, value, sizeof(detail->signature) - 1);
            }
        }
        line = strtok(NULL, "\n");
    }

    free(buf);
    return true;
}

// 读取 app.properties 文件中的 [env] 部分并传递给 Lua
void read_env_params(zip_t *z, lua_State *L)
{
    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat(z, "app.properties", 0, &st) != 0)
    {
        return;
    }

    char *buf = (char *)malloc(st.size + 1);
    struct zip_file *f = zip_fopen(z, "app.properties", 0);
    zip_fread(f, buf, st.size);
    buf[st.size] = '\0';
    zip_fclose(f);

    char *line = strtok(buf, "\n");
    char *section = NULL;
    lua_newtable(L);
    while (line != NULL)
    {
        if (line[0] == '[' && line[strlen(line) - 1] == ']')
        {
            section = line + 1;
            section[strlen(section) - 1] = '\0';
        }
        else if (section != NULL && strcmp(section, "env") == 0)
        {
            char *key = strtok(line, "=");
            char *value = strtok(NULL, "=");
            if (key != NULL && value != NULL)
            {
                lua_pushstring(L, key);
                lua_pushstring(L, value);
                lua_settable(L, -3);
            }
        }
        line = strtok(NULL, "\n");
    }

    free(buf);
}

// 验证插件签名
bool verify_signature(const char *expected_signature)
{
    // 简单示例：假设签名验证通过
    return true;
}