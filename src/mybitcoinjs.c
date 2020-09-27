//sun sep 20 01:45:47 -03 2020 
#include "utility.h"

napi_value bitcoin_private_key_to_wif(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[2], res;
   size_t argc=2, sz_tmp;
   uint32_t wif_type;
   char *p;

// private_key: string, wif_type: integer (optional)

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc<1) {
      napi_throw_error(env, NULL, "Missing: Bitcoin private key");
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }
//https://en.bitcoinwiki.org/wiki/Bitcoin_history
   if (argc==1)
      wif_type=(int)F_BITCOIN_WIF_MAINNET;
   else if (napi_get_value_uint32(env, argv[1], &wif_type)!=napi_ok) {
      napi_throw_error(env, "2008", "Can't parse Bitcoin WIF type"); // 2008 The big economical crisis
      return NULL;
   } else if ((wif_type!=(int)F_BITCOIN_WIF_MAINNET)&&(wif_type!=(int)F_BITCOIN_WIF_TESTNET)) {
      napi_throw_error(env, "2009", "Wrong WIF type"); // Bitcoin Price in 2009 = 0.00 USD (USD => One kind of state shitcoin)
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], _buf, 65+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "2010", "Can't parse Bitcoin hex string to private key"); //In May 2010 1 BTC = 0.01 USD
      return NULL;
   }

   if (sz_tmp!=64) {
      sprintf(_buf, "%lu", (unsigned long int)sz_tmp);
      sprintf(p=_buf+128, "Wrong Bitcoin hex string private key size %s", _buf);
      napi_throw_error(env, "2011", (const char *)p); // In initial 2011, 1 Bitcoin = 1 USD
      return NULL;
   }

   _buf[64]=0;
   res=NULL;

   if (f_str_to_hex((uint8_t *)(p=(_buf+128)), _buf)) {
      sprintf(p, "Can't convert hash \"%s\" to binary", _buf);
      napi_throw_error(env, "2012", (const char *)p); // 2013 -> 1 Bitcoin = 13 USD
      goto bitcoin_private_key_to_wif_EXIT1;
   }

   if ((err=f_private_key_to_wif(_buf+(sizeof(_buf)>>1), (sizeof(_buf)>>1), &sz_tmp, (uint8_t)wif_type, (uint8_t *)p))) {
      sprintf(_buf, "Can't parse Bitcoin private key to WIF: f_private_key_to_wif error = %d", err);
      napi_throw_error(env, "2013", (const char *)_buf); // April 2013 -> 1 Bitcoin = 266 USD
      goto bitcoin_private_key_to_wif_EXIT1;
   }

   if (napi_create_string_utf8(env, _buf+(sizeof(_buf)>>1), sz_tmp, &res)!=napi_ok) {
      napi_throw_error(env, "2014", "Unable to parse WIF conversion to JavaScript"); // January 2014 -> 1 Bitcoin = 750 - 1000 USD
      res=NULL;
   }

bitcoin_private_key_to_wif_EXIT1:
   memory_flush();
   return res;
}

napi_value bitcoin_wif_to_private_key(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[1], res;
   size_t argc=1, sz_tmp;
   uint32_t wif_type;
   char *p;

// wif: string

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc<1) {
      napi_throw_error(env, NULL, "Missing: Bitcoin WIF");
      return NULL;
   }

   if (argc>1) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], _buf, sizeof(_buf)>>1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "2015", "Can't parse WIF to buffer");
      return NULL;
   }

   _buf[sz_tmp]=0;
   res=NULL;

   if (f_wif_to_private_key((uint8_t *)(p=&_buf[sizeof(_buf)>>1]), NULL, (const char *)_buf)) {
      napi_throw_error(env, "2016", "Can't convert WIF to private key");
      goto bitcoin_wif_to_private_key_EXIT1;
   }

   if (napi_create_string_utf8(env, (const char *)fhex2strv2(_buf, (const void *)p, 32, 1), 64, &res)!=napi_ok) {
      napi_throw_error(env, "2017", "Unable to parse Bitcoin private key conversion to JavaScript");
      res=NULL;
   }

bitcoin_wif_to_private_key_EXIT1:
   memory_flush();
   return res;
}

