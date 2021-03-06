#include "isout_options.h"

isout_protocol_options_t *
isout_protocol_options_create(isshe_mempool_t *mempool, isshe_log_t *log)
{
    isout_protocol_options_t *opts;

    opts = isshe_mpalloc(mempool, sizeof(isout_protocol_options_t));
    if (!opts) {
        isshe_log_alert(log, "mpalloc isout options failed");
        return NULL;
    }

    // init
    isshe_memzero(opts, sizeof(isout_protocol_options_t));

    return opts;
}


void isout_protocol_options_destroy(
    isout_protocol_options_t *opts, isshe_mempool_t *mempool)
{
    isshe_mpfree(mempool, opts, sizeof(isout_protocol_options_t));
}

isout_protocol_header_t *
isout_protocol_header_create(isshe_mempool_t *mempool, isshe_log_t *log)
{
    isout_protocol_header_t *opts;

    opts = isshe_mpalloc(mempool, sizeof(isout_protocol_header_t));
    if (!opts) {
        isshe_log_alert(log, "mpalloc isout header failed");
        return NULL;
    }

    // init
    isshe_memzero(opts, sizeof(isout_protocol_header_t));

    return opts;
}


void isout_protocol_header_destroy(
    isout_protocol_header_t *opts, isshe_mempool_t *mempool)
{
    isshe_mpfree(mempool, opts, sizeof(isout_protocol_header_t));
}


isshe_int_t
isout_protocol_options_from_string(
    isout_protocol_options_t *options,
    isshe_char_t *stropts,
    isshe_int_t stropts_len,
    isshe_mempool_t *mempool,
    isshe_log_t *log)
{
    isout_protocol_option_t     *opt;
    isshe_int_t                 index;

    if (!options || !stropts) {
        isshe_log_alert(log, "options from string: invalid parameters");
        return ISSHE_ERROR;
    }

    // 检查是否是完整的选项
    if (!isout_protocol_options_is_valid(stropts, stropts_len)) {
        isshe_log_alert(log, "invalid isout protocol options");
        return ISSHE_ERROR;
    }

    index = 0;
    do {
        opt = (isout_protocol_option_t *)(stropts + index);
        switch (opt->type)
        {
        case ISOUT_OPTION_RANDOM:
            options->random = ntohl(*(uint32_t *)opt->data);
            break;
        case ISOUT_OPTION_ADDR:
            if (!options->addr) {
                options->addr = isshe_memdup(
                    (isshe_char_t *)opt->data, opt->len, mempool);
                options->addr_len = opt->len;
            }
        case ISOUT_OPTION_PORT:
            options->port = ntohs(*(uint16_t *)opt->data);
            break;
        case ISOUT_OPTION_ADDR_TYPE:
            options->addr_type = opt->data[0];
            break;
        case ISOUT_OPTION_DATA_LEN:
            options->data_len = ntohl(*(uint32_t *)opt->data);
            break;
        case ISOUT_OPTION_CRYPTO_ALGO:
            break;
        case ISOUT_OPTION_CRYPTO_KEY:
            break;
        case ISOUT_OPTION_CRYPTO_IV:
            break;
        case ISOUT_OPTION_SESSION_CRYPTO_ALGO:
            options->session_crypto_algo = opt->data[0];
            break;
        case ISOUT_OPTION_SESSION_CRYPTO_KEY:
            if (!options->session_crypto_key) {
                options->session_crypto_key = isshe_memdup(
                    opt->data, opt->len, mempool);
            }
            break;
        case ISOUT_OPTION_SESSION_CRYPTO_IV:
            if (!options->session_crypto_iv) {
                options->session_crypto_iv = isshe_memdup(
                    opt->data, opt->len, mempool);
            }
            break;
        default:
            break;
        }
        index += sizeof(opt->type) + sizeof(opt->len) + opt->len;
    } while(opt->type != ISOUT_OPTION_END);
    return ISSHE_OK;
}


isshe_int_t isout_protocol_options_len(isout_protocol_options_t *opts)
{
    isshe_int_t len;
    isout_protocol_option_t opt;

    len = 0;
    if (opts->random != 0) {
        len += sizeof(opts->random)
            + sizeof(opt.type) + sizeof(opt.len);
    }

    if (opts->addr && opts->addr_len != 0) {
        len += opts->addr_len
            + sizeof(opt.type) + sizeof(opt.len);
    }

    if (opts->port != 0) {
        len += sizeof(opts->port)
            + sizeof(opt.type) + sizeof(opt.len);
    }

    if (opts->addr_type != 0) {
        len += sizeof(opts->addr_type)
            + sizeof(opt.type) + sizeof(opt.len);
    }

    if (opts->data_len != 0) {
        len += sizeof(opts->data_len)
            + sizeof(opt.type) + sizeof(opt.len);
    }

    if (opts->session_crypto_algo != ISOUT_CRYPTO_ALGO_UNKNOWN) {
        len += sizeof(opts->session_crypto_algo)
            + sizeof(opt.type) + sizeof(opt.len);
    }

    if (opts->session_crypto_iv) {
        switch (opts->session_crypto_algo)
        {
        case ISOUT_CRYPTO_ALGO_AES_128_CFB:
            len += ISSHE_AES_BLOCK_SIZE + sizeof(opt.type) + sizeof(opt.len);
            break;
        }
    }

    if (opts->session_crypto_key) {
        switch (opts->session_crypto_algo)
        {
        case ISOUT_CRYPTO_ALGO_AES_128_CFB:
            len += ISSHE_AES_BLOCK_SIZE + sizeof(opt.type) + sizeof(opt.len);
            break;
        }
    }

    // END OPTION
    len += sizeof(opt.type) + sizeof(opt.len);

    return len;
}


isshe_int_t
isout_protocol_options_to_string(
    isout_protocol_options_t *opts,
    isshe_char_t *stropts,
    isshe_int_t *stropts_len,
    isshe_log_t *log)
{
    // 计算所有OPTION长度
    isshe_int_t                 len;
    isshe_char_t                *tmp;
    isshe_uint16_t              ui16;
    isshe_uint32_t              ui32;

    len = isout_protocol_options_len(opts);
    if (len == 0 || len > ISOUT_PROTOCOL_OPTIONS_LEN_MAX) {
        isshe_log_error(log, "isout protocol options length = %d", len);
        return ISSHE_ERROR;
    }

    tmp = stropts;
    if (opts->random != 0) {
        ui32 = htonl(opts->random);
        tmp += isout_option_append(tmp,
            ISOUT_OPTION_RANDOM, sizeof(opts->random), &ui32);
    }

    if (opts->addr && opts->addr_len != 0) {
        tmp += isout_option_append(tmp,
            ISOUT_OPTION_ADDR, opts->addr_len, opts->addr);
    }

    if (opts->port != 0) {
        ui16 = htons(opts->port);
        tmp += isout_option_append(tmp,
            ISOUT_OPTION_PORT, sizeof(opts->port), &ui16);
    }

    if (opts->addr_type != 0) {
        tmp += isout_option_append(tmp,
            ISOUT_OPTION_ADDR_TYPE, sizeof(opts->addr_type), &opts->addr_type);
    }

    if (opts->data_len != 0) {
        ui32 = htonl(opts->data_len);
        tmp += isout_option_append(tmp,
            ISOUT_OPTION_DATA_LEN, sizeof(opts->data_len), &ui32);
    }

    if (opts->session_crypto_algo != ISOUT_CRYPTO_ALGO_UNKNOWN) {
        tmp += isout_option_append(
            tmp, ISOUT_OPTION_SESSION_CRYPTO_ALGO,
            sizeof(opts->session_crypto_algo), &opts->session_crypto_algo);
    }

    if (opts->session_crypto_iv) {
        switch (opts->session_crypto_algo)
        {
        case ISOUT_CRYPTO_ALGO_AES_128_CFB:
            tmp += isout_option_append(
                tmp, ISOUT_OPTION_SESSION_CRYPTO_IV,
                ISSHE_AES_BLOCK_SIZE, opts->session_crypto_iv);
            break;
        }
    }

    if (opts->session_crypto_key) {
        switch (opts->session_crypto_algo)
        {
        case ISOUT_CRYPTO_ALGO_AES_128_CFB:
            tmp += isout_option_append(
                tmp, ISOUT_OPTION_SESSION_CRYPTO_KEY,
                ISSHE_AES_BLOCK_SIZE, opts->session_crypto_key);
            break;
        }
    }

    // END OPTION
    tmp += isout_option_append(tmp, ISOUT_OPTION_END, 0, NULL);

    *stropts_len = len;

    return ISSHE_OK;
}


void
isout_protocol_options_print(isout_protocol_options_t *opts, isshe_log_t *log)
{
    isshe_log_info(log, "======================================");
    isshe_log_info(log, "isout options: ");

    if (opts->random != 0) {
        isshe_log_info(log, " - random: %lld", opts->random);
    }
    if (opts->addr && opts->addr_len != 0) {
        switch (opts->addr_type)
        {
        case ISSHE_ADDR_TYPE_DOMAIN:
            isshe_log_info(log, " - dname: (%d)%s", opts->addr_len, opts->addr);
            break;
        case ISSHE_ADDR_TYPE_IPV6:
            isshe_log_info(log, " - ipv6: (%d)%s", opts->addr_len, opts->addr);
            break;
        case ISSHE_ADDR_TYPE_IPV4:
            isshe_log_info(log, " - ipv4: (%d)%s", opts->addr_len, opts->addr);
            break;
        }
    }

    if (opts->port != 0) {
        isshe_log_info(log, " - port: %ud", opts->port);
    }

    if (opts->addr_type != 0) {
        isshe_log_info(log, " - addr_type: %ud", opts->addr_type);
    }

    if (opts->data_len != 0) {
        isshe_log_info(log, " - data_len: %ud", opts->data_len);
    }

    if (opts->session_crypto_algo) {
        isshe_log_info(log, " - session_crypto_algo: %ud", opts->session_crypto_algo);
    }

    if (opts->session_crypto_key) {
        isshe_log_info(log, " - session_crypto_key: %s", opts->session_crypto_key);
    }

    if (opts->session_crypto_iv) {
        isshe_log_info(log, " - session_crypto_iv: %s", opts->session_crypto_iv);
    }


    isshe_log_info(log, "======================================");
}

isshe_int_t
isout_protocol_options_string_len(isshe_char_t *buf, isshe_int_t buflen)
{
    isout_protocol_option_t  opt;
    isshe_int_t    len;

    len = isout_option_find_end(buf, buflen);
    if (len == ISSHE_ERROR) {
        return ISSHE_ERROR;
    }

    return (len + sizeof(opt.type) + sizeof(opt.len));
}

isshe_bool_t
isout_protocol_options_is_valid(isshe_char_t *buf, isshe_int_t buflen)
{
    if (isout_protocol_options_string_len(buf, buflen) == ISSHE_ERROR) {
        return ISSHE_FALSE;
    }

    return ISSHE_TRUE;
}

isshe_int_t
isout_protocol_send_opts_generate(
    isout_protocol_options_t *send, 
    isout_protocol_options_t *all,
    isshe_address_t *addrinfo,
    isshe_mempool_t *mempool,
    isshe_log_t *log)
{
    isshe_char_t        *key;
    isshe_char_t        *iv;

    if (!all->session_crypto_key && !all->session_crypto_iv) {
        key = isshe_mpalloc(mempool, ISSHE_AES_BLOCK_SIZE);
        iv = isshe_mpalloc(mempool, ISSHE_AES_BLOCK_SIZE);
        if (!key || !iv) {
            isshe_log_error(log, "mpalloc key or iv failed");
            return ISSHE_ERROR;
        }

        // 填充key/iv
        isshe_rand_bytes(key, ISSHE_AES_BLOCK_SIZE);
        isshe_rand_bytes(iv, ISSHE_AES_BLOCK_SIZE);

        all->session_crypto_algo = ISOUT_CRYPTO_ALGO_AES_128_CFB;
        all->session_crypto_key = key;
        all->session_crypto_iv = iv;

        send->session_crypto_algo = all->session_crypto_algo;
        send->session_crypto_key = all->session_crypto_key;
        send->session_crypto_iv = all->session_crypto_iv;
    }

    if (!all->addr && addrinfo->addr) {
        all->addr = addrinfo->addr;
        all->addr_len = addrinfo->addr_len;
        all->port = addrinfo->port;
        all->addr_type = addrinfo->addr_type;

        send->addr = all->addr;
        send->addr_len = all->addr_len;
        send->port = all->port;
        send->addr_type = all->addr_type;
    }

    return ISSHE_OK;
}

void
isout_protocol_send_opts_resume(
    isout_protocol_options_t *send, 
    isout_protocol_options_t *all,
    isshe_mempool_t *mempool,
    isshe_log_t *log)
{
    if (send->session_crypto_key) {
        isshe_mpfree(mempool, all->session_crypto_key, ISSHE_AES_BLOCK_SIZE);
        isshe_mpfree(mempool, all->session_crypto_iv, ISSHE_AES_BLOCK_SIZE);

        all->session_crypto_algo = ISOUT_CRYPTO_ALGO_UNKNOWN;
        all->session_crypto_key = NULL;
        all->session_crypto_iv = NULL;

        send->session_crypto_algo = all->session_crypto_algo;
        send->session_crypto_key = all->session_crypto_key;
        send->session_crypto_iv = all->session_crypto_iv;
    }
}