/*
 * Copyright (c) 2009-2011 by Daniel Stenberg
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *
 *   Redistributions of source code must retain the above
 *   copyright notice, this list of conditions and the
 *   following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials
 *   provided with the distribution.
 *
 *   Neither the name of the copyright holder nor the names
 *   of any other contributors may be used to endorse or
 *   promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#include "libssh2_priv.h"
#include "misc.h"

struct known_host {
    struct list_node node;
    char *name;      /* points to the name or the hash (allocated) */
    size_t name_len; /* needed for hashed data */
    int port;        /* if non-zero, a specific port this key is for on this
                        host */
    int typemask;    /* plain, sha1, custom, ... */
    char *salt;      /* points to binary salt (allocated) */
    size_t salt_len; /* size of salt */
    char *key;       /* the (allocated) associated key. This is kept base64
                        encoded in memory. */
    char *comment;   /* the (allocated) optional comment text, may be NULL */

    /* this is the struct we expose externally */
    struct libssh2_knownhost external;
};

struct _LIBSSH2_KNOWNHOSTS
{
    LIBSSH2_SESSION *session;  /* the session this "belongs to" */
    struct list_head head;
};

static void free_host(LIBSSH2_SESSION *session, struct known_host *entry)
{
    if(entry) {
        if(entry->comment)
            LIBSSH2_FREE(session, entry->comment);
        if(entry->key)
            LIBSSH2_FREE(session, entry->key);
        if(entry->salt)
            LIBSSH2_FREE(session, entry->salt);
        if(entry->name)
            LIBSSH2_FREE(session, entry->name);
        LIBSSH2_FREE(session, entry);
    }
}

/*
 * libssh2_knownhost_init
 *
 * Init a collection of known hosts. Returns the pointer to a collection.
 *
 */
LIBSSH2_API LIBSSH2_KNOWNHOSTS *
libssh2_knownhost_init(LIBSSH2_SESSION *session)
{
    LIBSSH2_KNOWNHOSTS *knh =
        LIBSSH2_ALLOC(session, sizeof(struct _LIBSSH2_KNOWNHOSTS));

    if(!knh) {
        _libssh2_error(session, LIBSSH2_ERROR_ALLOC,
                       "Unable to allocate memory for known-hosts "
                       "collection");
        return NULL;
    }

    knh->session = session;

    _libssh2_list_init(&knh->head);

    return knh;
}

#define KNOWNHOST_MAGIC 0xdeadcafe
/*
 * knownhost_to_external()
 *
 * Copies data from the internal to the external representation struct.
 *
 */
static struct libssh2_knownhost *knownhost_to_external(struct known_host *node)
{
    struct libssh2_knownhost *ext = &node->external;

    ext->magic = KNOWNHOST_MAGIC;
    ext->node = node;
    ext->name = ((node->typemask & LIBSSH2_KNOWNHOST_TYPE_MASK) ==
                 LIBSSH2_KNOWNHOST_TYPE_PLAIN)? node->name:NULL;
    ext->key = node->key;
    ext->typemask = node->typemask;

    return ext;
}

static int
knownhost_add(LIBSSH2_KNOWNHOSTS *hosts,
              const char *host, const char *salt,
              const char *key, size_t keylen,
              const char *comment, size_t commentlen,
              int typemask, struct libssh2_knownhost **store)
{
    struct known_host *entry;
    size_t hostlen = strlen(host);
    int rc;
    char *ptr;
    unsigned int ptrlen;

    /* make sure we have a key type set */
    if(!(typemask & LIBSSH2_KNOWNHOST_KEY_MASK))
        return _libssh2_error(hosts->session, LIBSSH2_ERROR_INVAL,
                              "No key type set");

    if(!(entry = LIBSSH2_ALLOC(hosts->session, sizeof(struct known_host))))
        return _libssh2_error(hosts->session, LIBSSH2_ERROR_ALLOC,
                              "Unable to allocate memory for known host "
                              "entry");

    memset(entry, 0, sizeof(struct known_host));

    entry->typemask = typemask;

    switch(entry->typemask  & LIBSSH2_KNOWNHOST_TYPE_MASK) {
    case LIBSSH2_KNOWNHOST_TYPE_PLAIN:
    case LIBSSH2_KNOWNHOST_TYPE_CUSTOM:
        entry->name = LIBSSH2_ALLOC(hosts->session, hostlen+1);
        if(!entry->name) {
            rc = _libssh2_error(hosts->session, LIBSSH2_ERROR_ALLOC,
                                "Unable to allocate memory for host name");
            goto error;
        }
        memcpy(entry->name, host, hostlen+1);
        break;
    case LIBSSH2_KNOWNHOST_TYPE_SHA1:
        rc = libssh2_base64_decode(hosts->session, &ptr, &ptrlen,
                                   host, hostlen);
        if(rc)
            goto error;
        entry->name = ptr;
        entry->name_len = ptrlen;

        rc = libssh2_base64_decode(hosts->session, &ptr, &ptrlen,
                                   salt, strlen(salt));
        if(rc)
            goto error;
        entry->salt = ptr;
        entry->salt_len = ptrlen;
        break;
    default:
        rc = _libssh2_error(hosts->session, LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                            "Unknown host name type");
        goto error;
    }

    if(typemask & LIBSSH2_KNOWNHOST_KEYENC_BASE64) {
        /* the provided key is base64 encoded already */
        if(!keylen)
            keylen = strlen(key);
        entry->key = LIBSSH2_ALLOC(hosts->session, keylen+1);
        if(!entry->key) {
            rc = _libssh2_error(hosts->session, LIBSSH2_ERROR_ALLOC,
                                "Unable to allocate memory for key");
            goto error;
        }
        memcpy(entry->key, key, keylen+1);
        entry->key[keylen]=0; /* force a terminating zero trailer */
    }
    else {
        /* key is raw, we base64 encode it and store it as such */
        size_t nlen = _libssh2_base64_encode(hosts->session, key, keylen,
                                             &ptr);
        if(!nlen) {
            rc = _libssh2_error(hosts->session, LIBSSH2_ERROR_ALLOC,
                                "Unable to allocate memory for "
                                "base64-encoded key");
            goto error;
        }

        entry->key = ptr;
    }

    if (comment) {
        entry->comment = LIBSSH2_ALLOC(hosts->session, commentlen+1);
        if(!entry->comment) {
            rc = _libssh2_error(hosts->session, LIBSSH2_ERROR_ALLOC,
                                "Unable to allocate memory for comment");
            goto error;
        }
        memcpy(entry->comment, comment, commentlen+1);
        entry->comment[commentlen]=0; /* force a terminating zero trailer */
    }
    else {
        entry->comment = NULL;
    }

    /* add this new host to the big list of known hosts */
    _libssh2_list_add(&hosts->head, &entry->node);

    if(store)
        *store = knownhost_to_external(entry);

    return LIBSSH2_ERROR_NONE;
  error:
    free_host(hosts->session, entry);
    return rc;
}

/*
 * libssh2_knownhost_add
 *
 * Add a host and its associated key to the collection of known hosts.
 *
 * The 'type' argument specifies on what format the given host and keys are:
 *
 * plain  - ascii "hostname.domain.tld"
 * sha1   - SHA1(<salt> <host>) base64-encoded!
 * custom - another hash
 *
 * If 'sha1' is selected as type, the salt must be provided to the salt
 * argument. This too base64 encoded.
 *
 * The SHA-1 hash is what OpenSSH can be told to use in known_hosts files.  If
 * a custom type is used, salt is ignored and you must provide the host
 * pre-hashed when checking for it in the libssh2_knownhost_check() function.
 *
 * The keylen parameter may be omitted (zero) if the key is provided as a
 * NULL-terminated base64-encoded string.
 */

LIBSSH2_API int
libssh2_knownhost_add(LIBSSH2_KNOWNHOSTS *hosts,
                      const char *host, const char *salt,
                      const char *key, size_t keylen,
                      int typemask, struct libssh2_knownhost **store)
{
    return knownhost_add(hosts, host, salt, key, keylen, NULL, 0, typemask,
                         store);
}


/*
 * libssh2_knownhost_addc
 *
 * Add a host and its associated key to the collection of known hosts.
 *
 * Takes a comment argument that may be NULL.  A NULL comment indicates
 * there is no comment and the entry will end directly after the key
 * when written out to a file.  An empty string "" comment will indicate an
 * empty comment which will cause a single space to be written after the key.
 *
 * The 'type' argument specifies on what format the given host and keys are:
 *
 * plain  - ascii "hostname.domain.tld"
 * sha1   - SHA1(<salt> <host>) base64-encoded!
 * custom - another hash
 *
 * If 'sha1' is selected as type, the salt must be provided to the salt
 * argument. This too base64 encoded.
 *
 * The SHA-1 hash is what OpenSSH can be told to use in known_hosts files.  If
 * a custom type is used, salt is ignored and you must provide the host
 * pre-hashed when checking for it in the libssh2_knownhost_check() function.
 *
 * The keylen parameter may be omitted (zero) if the key is provided as a
 * NULL-terminated base64-encoded string.
 */

LIBSSH2_API int
libssh2_knownhost_addc(LIBSSH2_KNOWNHOSTS *hosts,
                       const char *host, const char *salt,
                       const char *key, size_t keylen,
                       const char *comment, size_t commentlen,
                       int typemask, struct libssh2_knownhost **store)
{
    return knownhost_add(hosts, host, salt, key, keylen, comment, commentlen,
                         typemask, store);
}

/*
 * knownhost_check
 *
 * Check a host and its associated key against the collection of known hosts.
 *
 * The typemask is the type/format of the given host name and key
 *
 * plain  - ascii "hostname.domain.tld"
 * sha1   - NOT SUPPORTED AS INPUT
 * custom - prehashed base64 encoded. Note that this cannot use any salts.
 *
 * Returns:
 *
 * LIBSSH2_KNOWNHOST_CHECK_FAILURE
 * LIBSSH2_KNOWNHOST_CHECK_NOTFOUND
 * LIBSSH2_KNOWNHOST_CHECK_MATCH
 * LIBSSH2_KNOWNHOST_CHECK_MISMATCH
 */
static int
knownhost_check(LIBSSH2_KNOWNHOSTS *hosts,
                const char *hostp, int port,
                const char *key, size_t keylen,
                int typemask,
                struct libssh2_knownhost **ext)
{
    struct known_host *node;
    struct known_host *badkey = NULL;
    int type = typemask & LIBSSH2_KNOWNHOST_TYPE_MASK;
    char *keyalloc = NULL;
    int rc = LIBSSH2_KNOWNHOST_CHECK_NOTFOUND;
    char hostbuff[270]; /* most host names can't be longer than like 256 */
    const char *host;
    int numcheck; /* number of host combos to check */
    int match = 0;

    if(type == LIBSSH2_KNOWNHOST_TYPE_SHA1)
        /* we can't work with a sha1 as given input */
        return LIBSSH2_KNOWNHOST_CHECK_MISMATCH;

    if(!(typemask & LIBSSH2_KNOWNHOST_KEYENC_BASE64)) {
        /* we got a raw key input, convert it to base64 for the checks below */
        size_t nlen = _libssh2_base64_encode(hosts->session, key, keylen,
                                             &keyalloc);
        if(!nlen) {
            _libssh2_error(hosts->session, LIBSSH2_ERROR_ALLOC,
                           "Unable to allocate memory for base64-encoded "
                           "key");
            return LIBSSH2_KNOWNHOST_CHECK_FAILURE;
        }

        /* make the key point to this */
        key = keyalloc;
    }

    /* if a port number is given, check for a '[host]:port' first before the
       plain 'host' */
    if(port >= 0) {
        snprintf(hostbuff, sizeof(hostbuff), "[%s]:%d", hostp, port);
        host = hostbuff;
        numcheck = 2; /* check both combos, start with this */
    }
    else {
        host = hostp;
        numcheck = 1; /* only check this host version */
    }

    do {
        node = _libssh2_list_first(&hosts->head);
        while (node) {
            switch(node->typemask & LIBSSH2_KNOWNHOST_TYPE_MASK) {
            case LIBSSH2_KNOWNHOST_TYPE_PLAIN:
                if(type == LIBSSH2_KNOWNHOST_TYPE_PLAIN)
                    match = !strcmp(host, node->name);
                break;
            case LIBSSH2_KNOWNHOST_TYPE_CUSTOM:
                if(type == LIBSSH2_KNOWNHOST_TYPE_CUSTOM)
                    match = !strcmp(host, node->name);
                break;
            case LIBSSH2_KNOWNHOST_TYPE_SHA1:
                if(type == LIBSSH2_KNOWNHOST_TYPE_PLAIN) {
                    /* when we have the sha1 version stored, we can use a
                       plain input to produce a hash to compare with the
                       stored hash.
                    */
                    libssh2_hmac_ctx ctx;
                    unsigned char hash[SHA_DIGEST_LENGTH];

                    if(SHA_DIGEST_LENGTH != node->name_len) {
                        /* the name hash length must be the sha1 size or
                           we can't match it */
                        break;
                    }
                    libssh2_hmac_sha1_init(&ctx, node->salt, node->salt_len);
                    libssh2_hmac_update(ctx, (unsigned char *)host,
                                        strlen(host));
                    libssh2_hmac_final(ctx, hash);
                    libssh2_hmac_cleanup(&ctx);

                    if(!memcmp(hash, node->name, SHA_DIGEST_LENGTH))
                        /* this is a node we're interested in */
                        match = 1;
                }
                break;
            default: /* unsupported type */
                break;
            }
            if(match) {
                /* host name match, now compare the keys */
                if(!strcmp(key, node->key)) {
                    /* they match! */
                    if (ext)
                        *ext = knownhost_to_external(node);
                    badkey = NULL;
                    rc = LIBSSH2_KNOWNHOST_CHECK_MATCH;
                    break;
                }
                else {
                    /* remember the first node that had a host match but a
                       failed key match since we continue our search from
                       here */
                    if(!badkey)
                        badkey = node;
                    match = 0; /* don't count this as a match anymore */
                }
            }
            node= _libssh2_list_next(&node->node);
        }
        host = hostp;
    } while(!match && --numcheck);

    if(badkey) {
        /* key mismatch */
        if (ext)
            *ext = knownhost_to_external(badkey);
        rc = LIBSSH2_KNOWNHOST_CHECK_MISMATCH;
    }

    if(keyalloc)
        LIBSSH2_FREE(hosts->session, keyalloc);

    return rc;
}

/*
 * libssh2_knownhost_check
 *
 * Check a host and its associated key against the collection of known hosts.
 *
 * The typemask is the type/format of the given host name and key
 *
 * plain  - ascii "hostname.domain.tld"
 * sha1   - NOT SUPPORTED AS INPUT
 * custom - prehashed base64 encoded. Note that this cannot use any salts.
 *
 * Returns:
 *
 * LIBSSH2_KNOWNHOST_CHECK_FAILURE
 * LIBSSH2_KNOWNHOST_CHECK_NOTFOUND
 * LIBSSH2_KNOWNHOST_CHECK_MATCH
 * LIBSSH2_KNOWNHOST_CHECK_MISMATCH
 */
LIBSSH2_API int
libssh2_knownhost_check(LIBSSH2_KNOWNHOSTS *hosts,
                        const char *hostp, const char *key, size_t keylen,
                        int typemask,
                        struct libssh2_knownhost **ext)
{
    return knownhost_check(hosts, hostp, -1, key, keylen,
                           typemask, ext);
}

/*
 * libssh2_knownhost_checkp
 *
 * Check a host+port and its associated key against the collection of known
 * hosts.
 *
 * Note that if 'port' is specified as greater than zero, the check function
 * will be able to check for a dedicated key for this particular host+port
 * combo, and if 'port' is negative it only checks for the generic host key.
 *
 * The typemask is the type/format of the given host name and key
 *
 * plain  - ascii "hostname.domain.tld"
 * sha1   - NOT SUPPORTED AS INPUT
 * custom - prehashed base64 encoded. Note that this cannot use any salts.
 *
 * Returns:
 *
 * LIBSSH2_KNOWNHOST_CHECK_FAILURE
 * LIBSSH2_KNOWNHOST_CHECK_NOTFOUND
 * LIBSSH2_KNOWNHOST_CHECK_MATCH
 * LIBSSH2_KNOWNHOST_CHECK_MISMATCH
 */
LIBSSH2_API int
libssh2_knownhost_checkp(LIBSSH2_KNOWNHOSTS *hosts,
                         const char *hostp, int port,
                         const char *key, size_t keylen,
                         int typemask,
                         struct libssh2_knownhost **ext)
{
    return knownhost_check(hosts, hostp, port, key, keylen,
                           typemask, ext);
}


/*
 * libssh2_knownhost_del
 *
 * Remove a host from the collection of known hosts.
 *
 */
LIBSSH2_API int
libssh2_knownhost_del(LIBSSH2_KNOWNHOSTS *hosts,
                      struct libssh2_knownhost *entry)
{
    struct known_host *node;

    /* check that this was retrieved the right way or get out */
    if(!entry || (entry->magic != KNOWNHOST_MAGIC))
        return _libssh2_error(hosts->session, LIBSSH2_ERROR_INVAL,
                              "Invalid host information");

    /* get the internal node pointer */
    node = entry->node;

    /* unlink from the list of all hosts */
    _libssh2_list_remove(&node->node);

    /* clear the struct now since the memory in which it is allocated is
       about to be freed! */
    memset(entry, 0, sizeof(struct libssh2_knownhost));

    /* free all resources */
    free_host(hosts->session, node);

    return 0;
}

/*
 * libssh2_knownhost_free
 *
 * Free an entire collection of known hosts.
 *
 */
LIBSSH2_API void
libssh2_knownhost_free(LIBSSH2_KNOWNHOSTS *hosts)
{
    struct known_host *node;
    struct known_host *next;

    for(node = _libssh2_list_first(&hosts->head); node; node = next) {
        next = _libssh2_list_next(&node->node);
        free_host(hosts->session, node);
    }
    LIBSSH2_FREE(hosts->session, hosts);
}


/* old style plain text: [name]([,][name])*

   for the sake of simplicity, we add them as separate hosts with the same
   key
*/
static int oldstyle_hostline(LIBSSH2_KNOWNHOSTS *hosts,
                             const char *host, size_t hostlen,
                             const char *key, size_t keylen, int key_type,
                             const char *comment, size_t commentlen)
{
    int rc = 0;
    size_t namelen = 0;
    const char *name = host + hostlen;

    if(hostlen < 1)
        return _libssh2_error(hosts->session,
                              LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Failed to parse known_hosts line "
                              "(no host names)");

    while(name > host) {
        --name;
        ++namelen;

        /* when we get the the start or see a comma coming up, add the host
           name to the collection */
        if((name == host) || (*(name-1) == ',')) {

            char hostbuf[256];

            /* make sure we don't overflow the buffer */
            if(namelen >= sizeof(hostbuf)-1)
                return _libssh2_error(hosts->session,
                                      LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                                      "Failed to parse known_hosts line "
                                      "(unexpected length)");

            /* copy host name to the temp buffer and zero terminate */
            memcpy(hostbuf, name, namelen);
            hostbuf[namelen]=0;

            rc = knownhost_add(hosts, hostbuf, NULL, key, keylen,
                               comment, commentlen,
                               key_type | LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                               LIBSSH2_KNOWNHOST_KEYENC_BASE64, NULL);
            if(rc)
                return rc;

            if(name > host) {
                namelen = 0;
                --name; /* skip comma */
            }
        }
    }

    return rc;
}

/* |1|[salt]|[hash] */
static int hashed_hostline(LIBSSH2_KNOWNHOSTS *hosts,
                           const char *host, size_t hostlen,
                           const char *key, size_t keylen, int key_type,
                           const char *comment, size_t commentlen)
{
    const char *p;
    char saltbuf[32];
    char hostbuf[256];

    const char *salt = &host[3]; /* skip the magic marker */
    hostlen -= 3;    /* deduct the marker */

    /* this is where the salt starts, find the end of it */
    for(p = salt; *p && (*p != '|'); p++)
        ;

    if(*p=='|') {
        const char *hash = NULL;
        size_t saltlen = p - salt;
        if(saltlen >= (sizeof(saltbuf)-1)) /* weird length */
            return _libssh2_error(hosts->session,
                                  LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                                  "Failed to parse known_hosts line "
                                  "(unexpectedly long salt)");

        memcpy(saltbuf, salt, saltlen);
        saltbuf[saltlen] = 0; /* zero terminate */
        salt = saltbuf; /* point to the stack based buffer */

        hash = p+1; /* the host hash is after the separator */

        /* now make the host point to the hash */
        host = hash;
        hostlen -= saltlen+1; /* deduct the salt and separator */

        /* check that the lengths seem sensible */
        if(hostlen >= sizeof(hostbuf)-1)
            return _libssh2_error(hosts->session,
                                  LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                                  "Failed to parse known_hosts line "
                                  "(unexpected length)");

        memcpy(hostbuf, host, hostlen);
        hostbuf[hostlen]=0;

        return knownhost_add(hosts, hostbuf, salt, key, keylen, comment,
                             commentlen,
                             key_type | LIBSSH2_KNOWNHOST_TYPE_SHA1 | 
                             LIBSSH2_KNOWNHOST_KEYENC_BASE64, NULL);
    }
    else
        return 0; /* XXX: This should be an error, shouldn't it? */
}

/*
 * hostline()
 *
 * Parse a single known_host line pre-split into host and key.
 *
 * The key part may include an optional comment which will be parsed here
 * for ssh-rsa and ssh-dsa keys.  Comments in other key types aren't handled.
 *
 * The function assumes new-lines have already been removed from the arguments.
 */
static int hostline(LIBSSH2_KNOWNHOSTS *hosts,
                    const char *host, size_t hostlen,
                    const char *key, size_t keylen)
{
    const char *comment = NULL;
    size_t commentlen = 0;
    int key_type;

    /* make some checks that the lengths seem sensible */
    if(keylen < 20)
        return _libssh2_error(hosts->session,
                              LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Failed to parse known_hosts line "
                              "(key too short)");
    
    switch(key[0]) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        key_type = LIBSSH2_KNOWNHOST_KEY_RSA1;

        /* Note that the old-style keys (RSA1) aren't truly base64, but we
         * claim it is for now since we can get away with strcmp()ing the
         * entire anything anyway! We need to check and fix these to make them
         * work properly.
         */
        break;

    case 's': /* ssh-dss or ssh-rsa */
        if(!strncmp(key, "ssh-dss", 7))
            key_type = LIBSSH2_KNOWNHOST_KEY_SSHDSS;
        else if(!strncmp(key, "ssh-rsa", 7))
            key_type = LIBSSH2_KNOWNHOST_KEY_SSHRSA;
        else
            /* unknown key type */
            return _libssh2_error(hosts->session,
                                  LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                                  "Unknown key type");

        key += 7;
        keylen -= 7;

        /* skip whitespaces */
        while((*key ==' ') || (*key == '\t')) {
            key++;
            keylen--;
        }

        comment = key;
        commentlen = keylen;

        /* move over key */
        while(commentlen && *comment &&
              (*comment != ' ') && (*comment != '\t')) {
            comment++;
            commentlen--;
        }

        /* reduce key by comment length */
        keylen -= commentlen;

        /* Distinguish empty comment (a space) from no comment (no space) */
        if (commentlen == 0)
            comment = NULL;

        /* skip whitespaces */
        while(commentlen && *comment &&
              ((*comment ==' ') || (*comment == '\t'))) {
            comment++;
            commentlen--;
        }
        break;

    default: /* unknown key format */
        return _libssh2_error(hosts->session,
                              LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Unknown key format");
    }

    /* Figure out host format */
    if((hostlen >2) && memcmp(host, "|1|", 3)) {
        /* old style plain text: [name]([,][name])*

           for the sake of simplicity, we add them as separate hosts with the
           same key
        */
        return oldstyle_hostline(hosts, host, hostlen, key, keylen, key_type,
                                 comment, commentlen);
    }
    else {
        /* |1|[salt]|[hash] */
        return hashed_hostline(hosts, host, hostlen, key, keylen, key_type,
                               comment, commentlen);
    }
}

/*
 * libssh2_knownhost_readline()
 *
 * Pass in a line of a file of 'type'.
 *
 * LIBSSH2_KNOWNHOST_FILE_OPENSSH is the only supported type.
 *
 * OpenSSH line format:
 *
 * <host> <key>
 *
 * Where the two parts can be created like:
 *
 * <host> can be either
 * <name> or <hash>
 *
 * <name> consists of
 * [name] optionally followed by [,name] one or more times
 *
 * <hash> consists of
 * |1|<salt>|hash
 *
 * <key> can be one of:
 * [RSA bits] [e] [n as a decimal number]
 * 'ssh-dss' [base64-encoded-key]
 * 'ssh-rsa' [base64-encoded-key]
 *
 */
LIBSSH2_API int
libssh2_knownhost_readline(LIBSSH2_KNOWNHOSTS *hosts,
                           const char *line, size_t len, int type)
{
    const char *cp;
    const char *hostp;
    const char *keyp;
    size_t hostlen;
    size_t keylen;
    int rc;

    if(type != LIBSSH2_KNOWNHOST_FILE_OPENSSH)
        return _libssh2_error(hosts->session,
                              LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Unsupported type of known-host information "
                              "store");

    cp = line;

    /* skip leading whitespaces */
    while(len && ((*cp==' ') || (*cp == '\t'))) {
        cp++;
        len--;
    }

    if(!len || !*cp || (*cp == '#') || (*cp == '\n'))
        /* comment or empty line */
        return LIBSSH2_ERROR_NONE;

    /* the host part starts here */
    hostp = cp;

    /* move over the host to the separator */
    while(len && *cp && (*cp!=' ') && (*cp != '\t')) {
        cp++;
        len--;
    }

    hostlen = cp - hostp;

    /* the key starts after the whitespaces */
    while(len && *cp && ((*cp==' ') || (*cp == '\t'))) {
        cp++;
        len--;
    }

    if(!*cp || !len) /* illegal line */
        return _libssh2_error(hosts->session,
                              LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Failed to parse known_hosts line");

    keyp = cp; /* the key starts here */
    keylen = len;

    /* check if the line (key) ends with a newline and if so kill it */
    while(len && *cp && (*cp != '\n')) {
        cp++;
        len--;
    }

    /* zero terminate where the newline is */
    if(*cp == '\n')
        keylen--; /* don't include this in the count */

    /* deal with this one host+key line */
    rc = hostline(hosts, hostp, hostlen, keyp, keylen);
    if(rc)
        return rc; /* failed */

    return LIBSSH2_ERROR_NONE; /* success */
}

/*
 * libssh2_knownhost_readfile
 *
 * Read hosts+key pairs from a given file.
 *
 * Returns a negative value for error or number of successfully added hosts.
 *
 */

LIBSSH2_API int
libssh2_knownhost_readfile(LIBSSH2_KNOWNHOSTS *hosts,
                           const char *filename, int type)
{
    FILE *file;
    int num = 0;
    char buf[2048];

    if(type != LIBSSH2_KNOWNHOST_FILE_OPENSSH)
        return _libssh2_error(hosts->session,
                              LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Unsupported type of known-host information "
                              "store");

    file = fopen(filename, "r");
    if(file) {
        while(fgets(buf, sizeof(buf), file)) {
            if(libssh2_knownhost_readline(hosts, buf, strlen(buf), type)) {
                num = _libssh2_error(hosts->session, LIBSSH2_ERROR_KNOWN_HOSTS,
                                     "Failed to parse known hosts file");
                break;
            }
            num++;
        }
        fclose(file);
    }
    else
        return _libssh2_error(hosts->session, LIBSSH2_ERROR_FILE,
                              "Failed to open file");

    return num;
}

/*
 * knownhost_writeline()
 *
 * Ask libssh2 to convert a known host to an output line for storage.
 *
 * Note that this function returns LIBSSH2_ERROR_BUFFER_TOO_SMALL if the given
 * output buffer is too small to hold the desired output. The 'outlen' field
 * will then contain the size libssh2 wanted to store, which then is the
 * smallest sufficient buffer it would require.
 *
 */
static int
knownhost_writeline(LIBSSH2_KNOWNHOSTS *hosts,
                    struct known_host *node,
                    char *buf, size_t buflen,
                    size_t *outlen, int type)
{
    int rc = LIBSSH2_ERROR_NONE;
    int tindex;
    const char *keytypes[4]={
        "", /* not used */
        "", /* this type has no name in the file */
        " ssh-rsa",
        " ssh-dss"
    };
    const char *keytype;
    size_t nlen;
    size_t commentlen = 0;

    /* we only support this single file type for now, bail out on all other
       attempts */
    if(type != LIBSSH2_KNOWNHOST_FILE_OPENSSH)
        return _libssh2_error(hosts->session,
                              LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Unsupported type of known-host information "
                              "store");

    tindex = (node->typemask & LIBSSH2_KNOWNHOST_KEY_MASK) >>
        LIBSSH2_KNOWNHOST_KEY_SHIFT;

    /* set the string used in the file */
    keytype = keytypes[tindex];

    /* calculate extra space needed for comment */
    if(node->comment)
        commentlen = strlen(node->comment) + 1;

    if((node->typemask & LIBSSH2_KNOWNHOST_TYPE_MASK) ==
       LIBSSH2_KNOWNHOST_TYPE_SHA1) {
        char *namealloc;
        char *saltalloc;
        nlen = _libssh2_base64_encode(hosts->session, node->name,
                                      node->name_len, &namealloc);
        if(!nlen)
            return _libssh2_error(hosts->session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to allocate memory for "
                                  "base64-encoded host name");

        nlen = _libssh2_base64_encode(hosts->session,
                                      node->salt, node->salt_len,
                                      &saltalloc);
        if(!nlen) {
            free(namealloc);
            return _libssh2_error(hosts->session, LIBSSH2_ERROR_ALLOC,
                                  "Unable to allocate memory for "
                                  "base64-encoded salt");
        }

        nlen = strlen(saltalloc) + strlen(namealloc) + strlen(keytype) +
            strlen(node->key) + commentlen + 7;
        /* |1| + | + ' ' + \n + \0 = 7 */

        if(nlen <= buflen)
            if(node->comment)
                snprintf(buf, buflen, "|1|%s|%s%s %s %s\n", saltalloc, namealloc,
                        keytype, node->key, node->comment);
            else
                snprintf(buf, buflen, "|1|%s|%s%s %s\n", saltalloc, namealloc,
                        keytype, node->key);
        else
            rc = _libssh2_error(hosts->session, LIBSSH2_ERROR_BUFFER_TOO_SMALL,
                                "Known-host write buffer too small");

        free(namealloc);
        free(saltalloc);
    }
    else {
        nlen = strlen(node->name) + strlen(keytype) + strlen(node->key) +
            commentlen + 3;
        /* ' ' + '\n' + \0 = 3 */
        if(nlen <= buflen)
            /* these types have the plain name */
            if(node->comment)
                snprintf(buf, buflen, "%s%s %s %s\n", node->name, keytype, node->key,
                        node->comment);
            else
                snprintf(buf, buflen, "%s%s %s\n", node->name, keytype, node->key);
        else
            rc = _libssh2_error(hosts->session, LIBSSH2_ERROR_BUFFER_TOO_SMALL,
                                "Known-host write buffer too small");
    }

    /* we report the full length of the data with the trailing zero excluded */
    *outlen = nlen-1;

    return rc;
}

/*
 * libssh2_knownhost_writeline()
 *
 * Ask libssh2 to convert a known host to an output line for storage.
 *
 * Note that this function returns LIBSSH2_ERROR_BUFFER_TOO_SMALL if the given
 * output buffer is too small to hold the desired output.
 */
LIBSSH2_API int
libssh2_knownhost_writeline(LIBSSH2_KNOWNHOSTS *hosts,
                            struct libssh2_knownhost *known,
                            char *buffer, size_t buflen,
                            size_t *outlen, /* the amount of written data */
                            int type)
{
    struct known_host *node;

    if(known->magic != KNOWNHOST_MAGIC)
        return _libssh2_error(hosts->session, LIBSSH2_ERROR_INVAL,
                              "Invalid host information");

    node = known->node;

    return knownhost_writeline(hosts, node, buffer, buflen, outlen, type);
}

/*
 * libssh2_knownhost_writefile()
 *
 * Write hosts+key pairs to the given file.
 */
LIBSSH2_API int
libssh2_knownhost_writefile(LIBSSH2_KNOWNHOSTS *hosts,
                            const char *filename, int type)
{
    struct known_host *node;
    FILE *file;
    int rc = LIBSSH2_ERROR_NONE;
    char buffer[2048];

    /* we only support this single file type for now, bail out on all other
       attempts */
    if(type != LIBSSH2_KNOWNHOST_FILE_OPENSSH)
        return _libssh2_error(hosts->session,
                              LIBSSH2_ERROR_METHOD_NOT_SUPPORTED,
                              "Unsupported type of known-host information "
                              "store");

    file = fopen(filename, "w");
    if(!file)
        return _libssh2_error(hosts->session, LIBSSH2_ERROR_FILE,
                              "Failed to open file");

    for(node = _libssh2_list_first(&hosts->head);
        node;
        node= _libssh2_list_next(&node->node) ) {
        size_t wrote;
        size_t nwrote;
        rc = knownhost_writeline(hosts, node, buffer, sizeof(buffer), &wrote,
                                 type);
        if(rc)
            break;

        nwrote = fwrite(buffer, 1, wrote, file);
        if(nwrote != wrote) {
            /* failed to write the whole thing, bail out */
            rc = _libssh2_error(hosts->session, LIBSSH2_ERROR_FILE,
                                "Write failed");
            break;
        }
    }
    fclose(file);

    return rc;
}


/*
 * libssh2_knownhost_get()
 *
 * Traverse the internal list of known hosts. Pass NULL to 'prev' to get
 * the first one.
 *
 * Returns:
 * 0 if a fine host was stored in 'store'
 * 1 if end of hosts
 * [negative] on errors
 */
LIBSSH2_API int
libssh2_knownhost_get(LIBSSH2_KNOWNHOSTS *hosts,
                      struct libssh2_knownhost **ext,
                      struct libssh2_knownhost *oprev)
{
    struct known_host *node;
    if(oprev && oprev->node) {
        /* we have a starting point */
        struct known_host *prev = oprev->node;

        /* get the next node in the list */
        node = _libssh2_list_next(&prev->node);

    }
    else
        node = _libssh2_list_first(&hosts->head);

    if(!node)
        /* no (more) node */
        return 1;

    *ext = knownhost_to_external(node);

    return 0;
}
