//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <glib.h>

int main() {
    g_print("GLib is working!\n");

    // Create a simple hash table
    GHashTable *table = g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(table, "name", "Alice");
    g_hash_table_insert(table, "language", "C with GLib");

    const char *name = g_hash_table_lookup(table, "name");
    const char *lang = g_hash_table_lookup(table, "language");

    printf("Name: %s\n", name);
    printf("Language: %s\n", lang);

    g_hash_table_destroy(table);

    printf("Libuv Test Starts!\n");
    uv_loop_t *loop = malloc(sizeof(uv_loop_t));
    uv_loop_init(loop);

    printf("Now quitting.\n");
    uv_run(loop, UV_RUN_DEFAULT);

    uv_loop_close(loop);
    free(loop);
    return 0;
}