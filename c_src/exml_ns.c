#include "exml_ns.h"

static ErlNifEnv* ns_env = NULL;
static ns_list* namespaces = NULL;

static ns_list* ns_lookup(const char* ns)
{
    ns_list* cur;
    for(cur = namespaces; cur != NULL; cur = cur->next) {
        if(strcmp(ns, cur->key) == 0) {
            return cur;
        }
    }
    return NULL;
};

static ns_list* ns_put(const char* ns)
{
    ns_list* new = malloc(sizeof(ns_list));
    size_t ns_len = strlen(ns);
    char* ns_string = malloc((ns_len+1)*sizeof(char));
    strcpy(ns_string, ns);

    new->next = namespaces;
    new->key = ns_string;
    new->term = enif_make_refc_binary(ns_env, ns_string, ns_len);

    namespaces = new;
    return new; 
};

static void ns_remove(ns_list* removed)
{
    ns_list* prev;
    if(removed != NULL && enif_free_refc_binary(removed->term) == 0)
    {
        if(removed == namespaces)
        {
            namespaces = removed->next;
        }
        else {
            for(prev = namespaces; prev->next != NULL; prev = prev->next) {
                if(prev->next == removed)
                {
                    prev->next = removed->next;
                    break;
                }
            }
        }
        free(removed->key);
        free(removed);
    }
};

ERL_NIF_TERM ns_binary(const char* ns)
{
    ns_list* bin = ns_lookup(ns);
    if(ns_env == NULL)
        ns_env = enif_alloc_env();
    if(bin == NULL)
        bin = ns_put(ns);
    return bin->term;
};

int ns_cleanup()
{
    int cleans = 0;
    ns_list* cur;
    for(cur = namespaces; cur != NULL; cur = cur->next)
    {
        if(enif_binary_get_refc(cur->term) == 1) {
            cleans++;
            ns_remove(cur);
        }
    }
    return cleans;
};