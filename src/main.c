//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <glib.h>

void print_key_value(gpointer key, gpointer value, gpointer user_data)
{
    (void)user_data;
    printf("%s => %s\n", (char*)key, (char*)value);
}

int main() {
    GHashTable* table = g_hash_table_new(g_str_hash, g_str_equal);

    g_hash_table_insert(table, "name", "Alice");
    g_hash_table_insert(table, "city", "Wonderland");
    g_hash_table_insert(table, "job", "Adventurer");

    const char* name = g_hash_table_lookup(table, "name");
    printf("\nAll k-v pairs:\n");
    g_hash_table_foreach(
        table, print_key_value, NULL
    );

    g_hash_table_destroy(table);
    return 0;
}