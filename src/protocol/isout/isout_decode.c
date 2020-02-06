
#include "isout_decode.h"


isshe_int_t isout_decode_aes_cfb128(isshe_uchar_t *ckey, isshe_uchar_t *ivec,
    isshe_char_t *data, isshe_size_t data_len, isshe_log_t *log)
{
    isshe_aes_key_t     key;
    isshe_int_t         num = 0;
    isshe_uchar_t       ivec_cp[ISSHE_AES_BLOCK_SIZE];

    if (!ckey || !ivec || !data) {
        isshe_log_error(log, "isout_decode_aes_cfb128: invalid parameters");
        return ISSHE_FAILURE;
    }

    // copy ivec, because isshe_aes_cfb128_encrypt will change ivec
    isshe_memcpy(ivec_cp, ivec, ISSHE_AES_BLOCK_SIZE);

    isshe_aes_set_encrypt_key(ckey, ISSHE_AES_BLOCK_SIZE_BIT, &key);

    //isshe_log_debug(log, 
    //"before decode: ivec = %s, data = (%d)%s", ivec_cp, data_len, data);

    isshe_aes_cfb128_encrypt(
        (const unsigned char *)data, (unsigned char *)data,
        data_len, &key, ivec_cp, &num, ISSHE_AES_DECRYPT);

    //isshe_log_debug(log, 
    //"after decode: ivec = %s, data = (%d)%s", ivec_cp, data_len, data);

    return ISSHE_SUCCESS;
}

static void isout_decode_test(isout_options_t *opts)
{
    isshe_char_t iv[] = "1234567890abcdef";
    isshe_char_t key[] = "abcdef1234567890";

    isshe_memcpy(opts->session_crypto_iv, iv, strlen(iv));
    isshe_memcpy(opts->session_crypto_key, key, strlen(key));
}


isshe_int_t
isout_decode(isout_options_t *opts, isshe_char_t *data,
    isshe_size_t data_len, isshe_log_t *log)
{
    //isshe_log_debug(log, 
    //"---isshe---: isout_decode---1--- session_crypto_algo = %d", opts->session_crypto_algo);
    isout_decode_test(opts);
    // 判断加密算法，使用相应的解密方式
    if (opts->session_crypto_algo == ISOUT_CRYPTO_ALGO_AES_128_CFB) {
        return isout_decode_aes_cfb128(
            (unsigned char *)opts->session_crypto_key,
            (unsigned char *)opts->session_crypto_iv, data, data_len, log);
    }
    //isshe_log_debug(log, "---isshe---: isout_decode---2---");
    return ISSHE_SUCCESS;
}
