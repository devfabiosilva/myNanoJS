#include "utility.h"
//27 de junho de 2019 15:43

napi_value nanojs_license(napi_env env, napi_callback_info info)
{
   napi_value license, argv;
   size_t argc=1;

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc) {
      napi_throw_error(env, "1", ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)LICENSE, sizeof(LICENSE)-1, &license)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, "Fail on parse LICENSE");
      return NULL;
   }

   return license;

}

napi_value nanojs_wallet_to_public_key(napi_env env, napi_callback_info info) 
{
   int err;
   size_t nano_wallet_sz, argc=1;
   napi_value argv, res;
   char nano_wallet[MAX_STR_NANO_CHAR];

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>1) {
      napi_throw_error(env, "1", ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (!argc) {
      napi_throw_error(env, "2", "Missing Nano/Xrb Wallet");
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv, nano_wallet, sizeof(nano_wallet)+1, &nano_wallet_sz)!=napi_ok) {
      napi_throw_error(env, "102", "Can't parse public key to myNanoEmbedded C library");
      return NULL;
   }

   if (nano_wallet_sz==MAX_STR_NANO_CHAR) {
      napi_throw_error(env, "123", "String excess greater than MAX_STR_NANO_CHAR");
      return NULL;
   }

   nano_wallet[nano_wallet_sz]=0;

   if ((err=nano_base_32_2_hex((uint8_t *)(_buf+128), nano_wallet))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, "Can't convert Nano wallet to binary");
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(_buf, (unsigned char *)(_buf+128)), 64, &res)!=napi_ok) {
      napi_throw_error(env, "100", "Failed when parse binary to string");
      return NULL;
   }

   return res;
}

napi_value nanojs_add_sub(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp;
   uint32_t add_sub_type;
   char *A, *B, *Result, *Tmp1, *Tmp2;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, "114", ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, "1", ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc==3) {
      if (napi_get_value_uint32(env, argv[2], &add_sub_type)!=napi_ok) {
         napi_throw_error(env, "115", "Wrong Nano Big number ADD/SUB mode");
         return NULL;
      }
   } else
      add_sub_type=F_NANO_RES_REAL_STRING|F_NANO_ADD_A_B|F_NANO_A_REAL_STRING|F_NANO_B_REAL_STRING;

   if (napi_get_value_string_utf8(env, argv[0], A=_buf, F_RAW_STR_MAX_SZ+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "116", "Can't parse Nano Big Number A value");
      return NULL;
   }

   if (sz_tmp==F_RAW_STR_MAX_SZ) {
      napi_throw_error(env, "117", ERROR_DIGITS_A_TOO_LONG);
      return NULL;
   }

   A[sz_tmp]=0;

   if (napi_get_value_string_utf8(env, argv[1], B=(A+F_RAW_STR_MAX_SZ), F_RAW_STR_MAX_SZ+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "118", "Can't parse Nano Big Number B value");
      return NULL;
   }

   if (sz_tmp==F_RAW_STR_MAX_SZ) {
      napi_throw_error(env, "119", ERROR_DIGITS_B_TOO_LONG);
      return NULL;
   }

   B[sz_tmp]=0;

   Result=(B+F_RAW_STR_MAX_SZ);
   Tmp1=(Result+F_RAW_STR_MAX_SZ);
   Tmp2=(Tmp1+F_RAW_STR_MAX_SZ);

   if (add_sub_type&F_NANO_A_RAW_128) {
      if ((err=f_str_to_hex((uint8_t *)Tmp1, A))) {
         napi_throw_error(env, "120", "Can't parse Nano Big Number A to binary");
         return NULL;
      }

      A=Tmp1;
   }

   if (add_sub_type&F_NANO_B_RAW_128) {
      if ((err=f_str_to_hex((uint8_t *)Tmp2, B))) {
         napi_throw_error(env, "121", "Can't parse Nano Big Number B to binary");
         return NULL;
      }

      B=Tmp2;
   }

   if ((add_sub_type&(F_NANO_RES_RAW_128|F_NANO_RES_RAW_STRING|F_NANO_RES_REAL_STRING))==0)
      add_sub_type|=F_NANO_RES_REAL_STRING;

   if ((err=f_nano_add_sub(Result, A, B, add_sub_type))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, ERROR_ADD_SUB_BIG_NUMBERS);
      return NULL;
   }

   if (add_sub_type&F_NANO_RES_RAW_128)
      Result=fhex2strv2(A, (const void *)Result, sizeof(f_uint128_t), 0);

   if (napi_create_string_utf8(env, (const char *)Result, NAPI_AUTO_LENGTH, &res)!=napi_ok) {
      napi_throw_error(env, "122", "Can't perform result operation");
      return NULL;
   }

   return res;

}

napi_value nanojs_pow(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp;
   uint64_t threshold;
   int32_t n_thr;
   bool lossless;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc==2)
      threshold=F_DEFAULT_THRESHOLD;
   else {
      if (napi_get_value_bigint_uint64(env, argv[2], &threshold, &lossless)!=napi_ok) {
         napi_throw_error(env, "124", "Error when parsing PoW threshold");
         return NULL;
      }

      if (!lossless) {
         napi_throw_error(env, "125", "Precision loss in threshold big int");
         return NULL;
      }
   }

   if (napi_get_value_int32(env, argv[1], &n_thr)!=napi_ok) {
      napi_throw_error(env, "126", "Error when parsing CPU number of threads");
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "127", "Can't parse Nano Big Number B value");
      return NULL;
   }

   if (sz_tmp!=64) {
      sprintf(_buf, "Wrong hash size %lu to calculate PoW", (unsigned long int)sz_tmp);
      napi_throw_error(env, "128", _buf);
      return NULL;
   }

   _buf[64]=0;

   if ((err=f_str_to_hex((uint8_t *)(_buf+128), _buf))) {
      sprintf((_buf+128), "Can't convert hash \"%s\" to binary", _buf);
      napi_throw_error(env, "129", (_buf+128));
      return NULL;
   }

   f_random_attach(gen_rand_no_entropy);

   if ((err=f_nano_pow((uint64_t *)_buf, (unsigned char *)(_buf+128), (const uint64_t)threshold, (int)n_thr))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, "Internal error in myNanoEmbedded C function 'f_nano_pow'");
   }

   f_random_detach();

   if (err)
      return NULL;

   if (napi_create_bigint_uint64(env, (uint64_t)*((uint64_t *)_buf), &res)!=napi_ok) {
      napi_throw_error(env, "130", "Can't parse result threshold JavaScript environment");
      return NULL;
   }

   return res;
}

napi_value nanojs_extract_seed_from_brainwallet(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp;
   uint32_t allow_mode;
   char *warning_msg, *brainwallet, *salt, *seed;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc==3) {

      if (napi_get_value_uint32(env, argv[2], &allow_mode)!=napi_ok) {
         napi_throw_error(env, "131", "Can't get Brainwallet allow mode");
         return NULL;
      }

   } else
      allow_mode=F_BRAIN_WALLET_PERFECT;

   if (napi_get_value_string_utf8(env, argv[0], brainwallet=_buf, (sizeof(_buf)>>1)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "132", "Can't parse Brainwallet text");
      return NULL;
   }

   if (sz_tmp==(sizeof(_buf)>>1)) {
      napi_throw_error(env, "134", "Brainwallet text too long");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   brainwallet[sz_tmp]=0;

   if (napi_get_value_string_utf8(env, argv[1], salt=(_buf+(sizeof(_buf)>>1)), (sizeof(_buf)>>1)+1-32, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "135", "Can't parse Salt text");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (sz_tmp==((sizeof(_buf)>>1)-32)) {
      napi_throw_error(env, "136", "Salt text is too long");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   salt[sz_tmp]=0;

   if ((err=f_extract_seed_from_brainwallet((uint8_t *)(seed=(_buf+sizeof(_buf)-32)), &warning_msg, allow_mode, (const char *)brainwallet, (const char *)salt))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, 
         "Error in myNanoEmbedded function 'f_extract_seed_from_brainwallet' %s. Can't extract Nano SEED from brainwallet. Warning message: %s", _buf, warning_msg);
      napi_throw_error(env, _buf, _buf+128);
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_create_object(env, &res)!=napi_ok) {
      napi_throw_error(env, "137", "myNanoEmbedded C error. Can't create object in 'nanojs_extract_seed_from_brainwallet'");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(_buf, (unsigned char *)seed), 64, &argv[0])!=napi_ok) {
      napi_throw_error(env, "138", "Can't export NANO SEED from Brainwallet");
      goto  nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_set_named_property(env, res, "seed", argv[0])!=napi_ok) {
      napi_throw_error(env, "139", "myNanoEmbedded C error. Can't set 'seed' property to 'nanojs_seed_to_nano_wallet'");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_create_string_utf8(env, (const char *)warning_msg, NAPI_AUTO_LENGTH, &argv[1])!=napi_ok) {
      napi_throw_error(env, "140", "Can't export warning message");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_set_named_property(env, res, "warning_message", argv[1])!=napi_ok) {
      napi_throw_error(env, "141", "myNanoEmbedded C error. Can't set 'warningMessage' property to 'nanojs_seed_to_nano_wallet'");
      res=NULL;
   }

   memory_flush();
   return res;

nanojs_extract_seed_from_brainwallet_EXIT1:
   memory_flush();
   return NULL;
}

//account 0
//previous 1
//representative 2
//balance 3
//balance_type 4
//value_to_send_or_receive 5
//value_to_send_or_receive_type 6
//link or destination address 7
//direction 8
napi_value nanojs_create_block(napi_env env, napi_callback_info info)
{
   int err, xrb_prefix;
   napi_value argv[9], res;
   size_t argc=9, sz_tmp;
   uint8_t *p;
   char *Balance, *Value_To_Send_Rec;
   uint32_t type, type_tmp;
   F_BLOCK_TRANSFER nano_block;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>9) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<9) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "142", "Can't parse account to Nano block");
      return NULL;
   }

   memset(&nano_block, 0, sizeof(nano_block));

   if ((err=extract_public_key_from_wallet_or_hex_str_util(&xrb_prefix, nano_block.account, _buf, sz_tmp))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't write Nano Wallet/Public key to block %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (xrb_prefix)
      nano_block.prefixes=SENDER_XRB;

   if (napi_get_value_string_utf8(env, argv[1], _buf+128, (sizeof(_buf)-128), &sz_tmp)==napi_ok) {
      if (sz_tmp==64) {
         if ((err=f_str_to_hex((uint8_t *)_buf, _buf+128))) {
            napi_throw_error(env, "144", "Error when parsing previous block to binary");
            return NULL;
         }

         p=(uint8_t *)_buf;
      } else if (sz_tmp) {
         napi_throw_error(env, "143", "Invalid previous block size");
         return NULL;
      } else
         p=nano_block.account;

   } else {
      if (napi_get_null(env, &argv[1])!=napi_ok) {
         napi_throw_error(env, "145", "Error when parsing previous account to block");
         return NULL;
      }

      p=nano_block.account;
   }

   memcpy(nano_block.previous, p, 32);

   if (napi_get_value_string_utf8(env, argv[2], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "146", "Can't parse representative to Nano block");
      return NULL;
   }

   if ((err=extract_public_key_from_wallet_or_hex_str_util(&xrb_prefix, nano_block.representative, _buf, sz_tmp))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't write Representative Nano Wallet/Public key to block %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (xrb_prefix)
      nano_block.prefixes|=REP_XRB;

   if (napi_get_value_string_utf8(env, argv[3], Balance=_buf, F_RAW_STR_MAX_SZ+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "147", "Can't parse Balance Number value");
      return NULL;
   }

   if (sz_tmp==F_RAW_STR_MAX_SZ) {
      napi_throw_error(env, "148", "Balance digits are too long");
      return NULL;
   }

   Balance[sz_tmp]=0;

   if (napi_get_value_uint32(env, argv[4], &type_tmp)!=napi_ok) {
      napi_throw_error(env, "149", "Can't determine Balance Big number type");
      return NULL;
   }

   if (type_tmp&BALANCE_RAW_128) {
      if ((err=f_str_to_hex(p=(uint8_t *)(_buf+2*F_RAW_STR_MAX_SZ), Balance))) {
         napi_throw_error(env, "157", "Can't parse Balance to binary");
         return NULL;
      }

      Balance=(char *)p;
   }

   type=type_tmp;

   if (napi_get_value_string_utf8(env, argv[5], Value_To_Send_Rec=(_buf+F_RAW_STR_MAX_SZ), F_RAW_STR_MAX_SZ+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "150", "Can't parse Value to send/Receive Big Number value");
      return NULL;
   }

   if (sz_tmp==F_RAW_STR_MAX_SZ) {
      napi_throw_error(env, "151", "Value to send/receive digits are too long");
      return NULL;
   }

   Value_To_Send_Rec[sz_tmp]=0;

   if (napi_get_value_uint32(env, argv[6], &type_tmp)!=napi_ok) {
      napi_throw_error(env, "152", "Can't determine Value to send/receive Big number type");
      return NULL;
   }

   if (type_tmp&VALUE_SEND_RECEIVE_RAW_128) {
      if ((err=f_str_to_hex(p=(uint8_t *)(_buf+3*F_RAW_STR_MAX_SZ), Value_To_Send_Rec))) {
         napi_throw_error(env, "157", "Can't parse Value to Send/Receive to binary");
         return NULL;
      }

      Value_To_Send_Rec=(char *)p;
   }

   type|=type_tmp;
   type_tmp|=F_NANO_A_RAW_128;

   if ((err=f_nano_value_compare_value(memset(_buf+256, 0, sizeof(f_uint128_t)), Value_To_Send_Rec, &type_tmp))) {
      napi_throw_error(env, "158", "Can't compare Value to send big numbers");
      return NULL;
   }

   if (type_tmp&F_NANO_COMPARE_EQ) {
      napi_throw_error(env, "159", "You cannot send or receive 0 amount");
      return NULL;
   }

   if (napi_get_value_uint32(env, argv[8], &type_tmp)!=napi_ok) {
      napi_throw_error(env, "153", "Can't determine direction (Send or Receive)");
      return NULL;
   }

   type|=type_tmp;

   if (memcmp(nano_block.account, nano_block.previous, 32)==0) {
      if (type_tmp&VALUE_TO_SEND) {
         napi_throw_error(env, "160", "You cannot send amount in genesis block");
         return NULL;
      }

      type_tmp=((type&(BALANCE_RAW_128|BALANCE_REAL_STRING|BALANCE_RAW_STRING))|F_NANO_B_RAW_128);

      if ((err=f_nano_value_compare_value(Balance, _buf+256, &type_tmp))) {
         napi_throw_error(env, "158", "Can't compare Balance big number");
         return NULL;
      }

      if (type_tmp&F_NANO_COMPARE_GT) {
         napi_throw_error(env, "161", "Genesis block should have 0 amount");
         return NULL;
      }
   }

   if ((err=f_nano_add_sub(nano_block.balance, Balance, Value_To_Send_Rec, F_NANO_RES_RAW_128|type))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, ERROR_ADD_SUB_BIG_NUMBERS);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[7], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "154", "Can't parse link to Nano block");
      return NULL;
   }

   if ((err=extract_public_key_from_wallet_or_hex_str_util(&xrb_prefix, nano_block.link, _buf, sz_tmp))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't write link to block %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (xrb_prefix)
      nano_block.prefixes|=DEST_XRB;

   nano_block.preamble[31]=0x06;

   if (napi_create_arraybuffer(env, sizeof(nano_block), (void **)&p, &res)!=napi_ok) {
      napi_throw_error(env, "155", "Can't create array buffer to store Nano Block");
      return NULL;
   }

   if (napi_create_external_arraybuffer(env, memcpy(p, &nano_block, sizeof(nano_block)), sizeof(nano_block), NULL, NULL, &res)!=napi_ok) {
      napi_throw_error(env, "156", "Can't copy array buffer to store Nano Block in Javascript");
      return NULL;
   }

   return res;
}

napi_value nanojs_block_to_JSON(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv, block, res;
   size_t argc=1, sz_tmp;
   void *buffer;
   F_BLOCK_TRANSFER nano_block;

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (napi_get_arraybuffer_info(env, argv, &buffer, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_READ_NANO_BLOCK_NUMBER, ERROR_CANT_READ_NANO_BLOCK);
      return NULL;
   }

   if (sz_tmp!=sizeof(F_BLOCK_TRANSFER)) {
      napi_throw_error(env, WRONG_NANO_BLOCK_SZ_ERR, WRONG_NANO_BLOCK_SZ);
      return NULL;
   }

   if (!f_nano_is_valid_block((F_BLOCK_TRANSFER *)buffer)) {
      napi_throw_error(env, ERROR_INVALID_NANO_BLK_NUMBER, ERROR_INVALID_NANO_BLK);
      return NULL;
   }

   memcpy(&nano_block, buffer, sizeof(nano_block));

   if (napi_create_object(env, &block)!=napi_ok) {
      napi_throw_error(env, "165", "Can't create JSON block");
      return NULL;
   }

   if (napi_create_string_utf8(env, "state", NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "type", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if ((err=pk_to_wallet(_buf, (nano_block.prefixes&SENDER_XRB)?XRB_PREFIX:NANO_PREFIX, memcpy(_buf+128, nano_block.account, 32)))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't convert public key account to Base 32 Nano Wallet %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)_buf, NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "account", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(_buf, (unsigned char *)nano_block.previous), NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "previous", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if ((err=pk_to_wallet(_buf, (nano_block.prefixes&REP_XRB)?XRB_PREFIX:NANO_PREFIX, memcpy(_buf+128, nano_block.representative, 32)))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't convert public key representative to Base 32 Nano Wallet %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)_buf, NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "representative", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if ((err=f_nano_balance_to_str(_buf, sizeof(_buf), &sz_tmp, nano_block.balance))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't parse Nano RAW to string %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)_buf, sz_tmp, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "balance", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(_buf, (unsigned char *)nano_block.link), NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "link", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if ((err=pk_to_wallet(_buf, (nano_block.prefixes&DEST_XRB)?XRB_PREFIX:NANO_PREFIX, memcpy(_buf+128, nano_block.link, 32)))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't convert link to Base 32 Nano Wallet %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)_buf, NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "link_as_account", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)fhex2strv2(_buf, (unsigned char *)nano_block.signature, 64, 1), NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "signature", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   sprintf(_buf, "%016llx", (unsigned long long int)nano_block.work);

   if (napi_create_string_utf8(env, (const char *)_buf, NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "work", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }
//
   if (napi_create_object(env, &res)!=napi_ok) {
      napi_throw_error(env, "166", "Unable to create Nano Block to parse to JavaScript");
      return NULL;
   }

   if (napi_create_string_utf8(env, "process", NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, res, "action", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_create_string_utf8(env, "true", NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, res, "json_block", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_set_named_property(env, res, "block", block)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   return res;
}

#define BUFFER_ADJUST (size_t)3*F_RAW_STR_MAX_SZ
napi_value nanojs_convert_balance(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[2], res;
   size_t argc=2, sz_tmp, tmp;
   uint32_t type;
   void *buffer;
   char *p;
//type is optional. If ommited then assume value is raw -> real
   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>2) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (!argc) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc==1)
      type=RAW_TO_REAL;
   else if (napi_get_value_uint32(env, argv[1], &type)!=napi_ok) {
      napi_throw_error(env, "167", "Can't parse type for conversion");
      return NULL;
   }

   if ((int)type&(HEX_TO_REAL|HEX_TO_RAW)) {
      if (napi_get_arraybuffer_info(env, argv[0], &buffer, &sz_tmp)!=napi_ok) {
         napi_throw_error(env, "600", "Can't read Big number in ArrayBuffer");
         return NULL;
      }

      if (!sz_tmp) {
         napi_throw_error(env, "601", "Big number lengh in ArrayBuffer is zero");
         return NULL;
      }

      if (sz_tmp>sizeof(f_uint128_t)) {
         sprintf(_buf, "Hex length %lu. Nano big number must be 16 bytes long", (unsigned long int)sz_tmp);
         napi_throw_error(env, "169", (const char *)_buf);
         return NULL;
      }

      memcpy(p=(_buf+192), buffer, sz_tmp);
   } else if (napi_get_value_string_utf8(env, argv[0], p=(_buf+192), (sizeof(_buf)-192)+1, &sz_tmp)==napi_ok) {
      if (!sz_tmp) {
         napi_throw_error(env, "173", "Empty value");
         return NULL;
      }

      if (sz_tmp>(F_RAW_STR_MAX_SZ-1)) {
         sprintf(_buf, "Wrong balance size = %lu", (unsigned long int)sz_tmp);
         napi_throw_error(env, "167", (const char *)_buf);
         return NULL;
      }

      p[sz_tmp]=0;
   } else{
      napi_throw_error(env, "168", "Can't parse Big number value for conversion");
      return NULL;
   }

   switch ((int)type) {
      case REAL_TO_HEX:
      case REAL_TO_RAW:
         if ((err=f_nano_parse_real_str_to_raw128_t((uint8_t *)(_buf+BUFFER_ADJUST), (const char *)p))) {
            sprintf(_buf, "%d", err);
            sprintf(p, "Can't parse Real to Nao Raw big int %s", _buf);
            napi_throw_error(env, (const char *)_buf, (const char *)p);
            return NULL;
         }

         if ((int)type&REAL_TO_RAW) {
            if ((err=f_nano_balance_to_str(_buf, BUFFER_ADJUST, NULL, (uint8_t *)(_buf+BUFFER_ADJUST)))) {
               sprintf(_buf, "%d", err);
               sprintf(p, "Can't convert raw hex big number to raw string %s", _buf);
               napi_throw_error(env, (const char *)_buf, (const char *)p);
               return NULL;
            }
            break;
         }

         memcpy(_buf, (_buf+BUFFER_ADJUST), sizeof(f_uint128_t));
         break;
      case RAW_TO_REAL:
         if ((err=f_nano_raw_to_string(_buf, NULL, BUFFER_ADJUST, p, F_RAW_TO_STR_STRING))) {
            sprintf(_buf, "%d", err);
            sprintf(p, "Can't convert raw string big number to real string %s", _buf);
            napi_throw_error(env, (const char *)_buf, (const char *)p);
            return NULL;
         }
         break;
      case RAW_TO_HEX:
         if ((err=f_nano_parse_raw_str_to_raw128_t((uint8_t *)_buf, (const char *)p))) {
            sprintf(_buf, "%d", err);
            sprintf(p, "Can't convert raw string big number to raw hex binary %s", _buf);
            napi_throw_error(env, (const char *)_buf, (const char *)p);
            return NULL;
         }
         break;
      case HEX_TO_REAL:
      case HEX_TO_RAW:;
         tmp=0;

         if (sz_tmp^sizeof(f_uint128_t)) {
            tmp=sizeof(f_uint128_t)-sz_tmp;
            memset(_buf+BUFFER_ADJUST, 0, tmp);
         }

         memcpy(_buf+BUFFER_ADJUST+tmp, p, sz_tmp);

         if ((int)type&HEX_TO_REAL) {
            if ((err=f_nano_raw_to_string(_buf, NULL, BUFFER_ADJUST, (void *)(_buf+BUFFER_ADJUST), F_RAW_TO_STR_UINT128))) {
               sprintf(_buf, "%d", err);
               sprintf(p, "Can't parse raw binary hex to real string %s", _buf);
               napi_throw_error(env, (const char *)_buf, (const char *)p);
               return NULL;
            }
            break;
         }

         if ((err=f_nano_balance_to_str(_buf, BUFFER_ADJUST, NULL, (uint8_t *)(_buf+BUFFER_ADJUST)))) {
            sprintf(_buf, "%d", err);
            sprintf(p, "Can't parse raw hex binary to raw string %s", _buf);
            napi_throw_error(env, (const char *)_buf, (const char *)p);
            return NULL;
         }

         break;
      default:
         napi_throw_error(env, "171", "Unknown conversion type");
         return NULL;
   }

   if ((int)type&(REAL_TO_HEX|RAW_TO_HEX)) {
      if (napi_create_arraybuffer(env, sizeof(f_uint128_t), &buffer, &res)!=napi_ok) {
         napi_throw_error(env, "602", "Can't create array buffer to store Big Number uint128");
         return NULL;
      }

      if (napi_create_external_arraybuffer(env, memcpy(buffer, _buf, sizeof(f_uint128_t)), sizeof(f_uint128_t), NULL, NULL, &res)!=napi_ok) {
         napi_throw_error(env, "603", "Can't copy array buffer to store Big Number uint128 in Javascript");
         res=NULL;
      }
   } else if (napi_create_string_utf8(env, _buf, NAPI_AUTO_LENGTH, &res)!=napi_ok) {
      napi_throw_error(env, "172", "Unable to parse conversion to JavaScript");
      res=NULL;
   }

   return res;
}

napi_value nanojs_sign_block(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[2], res;
   size_t argc=2, sz_tmp;
   uint8_t *p;
   void *buffer;
   F_BLOCK_TRANSFER nano_block;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>2) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_arraybuffer_info(env, argv[0], &buffer, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_READ_NANO_BLOCK_NUMBER, ERROR_CANT_READ_NANO_BLOCK);
      return NULL;
   }

   if (sz_tmp!=sizeof(F_BLOCK_TRANSFER)) {
      napi_throw_error(env, WRONG_NANO_BLOCK_SZ_ERR, WRONG_NANO_BLOCK_SZ);
      return NULL;
   }

   if (!f_nano_is_valid_block((F_BLOCK_TRANSFER *)buffer)) {
      napi_throw_error(env, ERROR_INVALID_NANO_BLK_NUMBER, ERROR_INVALID_NANO_BLK);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[1], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "174", "Can't parse Nano private key to sign block");
      return NULL;
   }

   if (sz_tmp!=128) {
      napi_throw_error(env, "175", "Wrong private key size");
      return NULL;
   }

   _buf[128]=0;
   memcpy(&nano_block, buffer, sizeof(nano_block));

   if ((err=f_str_to_hex(p=(uint8_t *)(_buf+256), _buf))) {
      memory_flush();
      sprintf(_buf, "%d", err);
      sprintf((char *)p, "Can't parse private key to binary hex %s", _buf);
      napi_throw_error(env, _buf, (char *)p);
      return NULL;
   }

   if ((err=f_nano_sign_block(&nano_block, NULL, p))) {
      sprintf(_buf, "%d", err);
      sprintf((char *)p, "Can't sign Nano block %s", _buf);
      napi_throw_error(env, _buf, (char *)p);
   }

   memory_flush();

   if (err)
      return NULL;

   if (napi_create_arraybuffer(env, sizeof(nano_block), (void **)&p, &res)!=napi_ok) {
      napi_throw_error(env, "176", "Can't create array buffer to store signed Nano Block");
      return NULL;
   }

   if (napi_create_external_arraybuffer(env, memcpy(p, &nano_block, sizeof(nano_block)), sizeof(nano_block), NULL, NULL, &res)!=napi_ok) {
      napi_throw_error(env, "177", "Can't copy array buffer to store signed Nano Block in Javascript");
      return NULL;
   }

   return res;
}

napi_value nanojs_public_key_to_wallet(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[2], res;
   size_t argc=2, sz_tmp;
   char *prefix, *p;
   void *buffer;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>2) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (!argc) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc==1)
      prefix=NANO_PREFIX;
   else if (napi_get_value_string_utf8(env, argv[1], prefix=(_buf+128), sizeof(NANO_PREFIX)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "178", "Can't determine Nano wallet prefix");
      return NULL;
   }

   if (sz_tmp==sizeof(NANO_PREFIX)) {
      napi_throw_error(env, "179", "Wrong or invalid prefix size");
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], p=(_buf+256), (65+1), &sz_tmp)==napi_ok) {
      if (sz_tmp!=64) {
         sprintf(_buf, "%lu", (unsigned long int)sz_tmp);
         sprintf(p, "Wrong hex public key size %s", _buf);
         napi_throw_error(env, _buf, p);
         return NULL;
      }

      p[64]=0;

      if ((err=f_str_to_hex((uint8_t *)_buf, p))) {
         napi_throw_error(env, "181", "Can't convert hex string public key to hex binary public key");
         return NULL;
      }
   } else if (napi_get_arraybuffer_info(env, argv[0], &buffer, &sz_tmp)==napi_ok) {
      if (sz_tmp!=32) {
         napi_throw_error(env, "98", "Can't parse ArrayBuffer public key");
         return NULL;
      }

      memcpy(_buf, buffer, 32);
   } else {
      napi_throw_error(env, "180", "Can't parse string hex/ArrayBuffer public key");
      return NULL;
   }

   if ((err=pk_to_wallet(p, prefix, (uint8_t *)_buf))) {
      sprintf(_buf, "%d", err);
      sprintf(p, "Can't convert public key to wallet %s", _buf);
      napi_throw_error(env, _buf, p);
      return NULL;
   }

   if (napi_create_string_utf8(env, p, NAPI_AUTO_LENGTH, &res)!=napi_ok) {
      napi_throw_error(env, "182", "Unable to parse Nano wallet conversion to JavaScript");
      return NULL;
   }

   return res;
}

napi_value nanojs_get_block_hash(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv, res;
   size_t argc=1, sz_tmp;
   uint8_t *p;
   void *buffer;
   F_BLOCK_TRANSFER nano_block;

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>1) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (!argc) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_arraybuffer_info(env, argv, &buffer, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_READ_NANO_BLOCK_NUMBER, ERROR_CANT_READ_NANO_BLOCK);
      return NULL;
   }

   if (sz_tmp!=sizeof(F_BLOCK_TRANSFER)) {
      napi_throw_error(env, WRONG_NANO_BLOCK_SZ_ERR, WRONG_NANO_BLOCK_SZ);
      return NULL;
   }

   if (!f_nano_is_valid_block((F_BLOCK_TRANSFER *)buffer)) {
      napi_throw_error(env, ERROR_INVALID_NANO_BLK_NUMBER, ERROR_INVALID_NANO_BLK);
      return NULL;
   }

   memcpy(&nano_block, buffer, sizeof(nano_block));

   if ((err=f_nano_get_block_hash(p=(uint8_t *)(_buf+128), &nano_block))) {
      sprintf(_buf, "%d", err);
      sprintf((char *)p, "Can't calculate block hash %s", _buf);
      napi_throw_error(env, _buf, (char *)p);
      return NULL;
   }

   if (napi_create_string_utf8(env, fhex2strv2(_buf, p, 32, 1), NAPI_AUTO_LENGTH, &res)!=napi_ok) {
      napi_throw_error(env, "183", "Unable to parse HASH result of the block to JavaScript");
      return NULL;
   }

   return res;
}

napi_value nanojs_verify_message(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp, buffer_sz;
   uint32_t type;
   char *p;
   void *buffer;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<3) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_arraybuffer_info(env, argv[1], &buffer, &buffer_sz)!=napi_ok) {
      napi_throw_error(env, "184", "Unable to parse ArrayBuffer to verify signature");
      return NULL;
   }

   if (!buffer_sz) {
      napi_throw_error(env, ERROR_EMPTY_ARRAY_BUFFER_ERR, ERROR_EMPTY_ARRAY_BUFFER_MSG);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[2], _buf, MAX_STR_NANO_CHAR+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "186", "Can't parse wallet or public key for verify signature");
      return NULL;
   }

   if (sz_tmp==MAX_STR_NANO_CHAR) {
      napi_throw_error(env, "187", "Public key/Nano Wallet too long");
      return NULL;
   }

   _buf[sz_tmp]=0;

   if (napi_get_value_string_utf8(env, argv[0], p=(_buf+256), 130, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "188", "Can't parse signature to verify signed ArrayBuffer");
      return NULL;
   }

   if (sz_tmp==129) {
      napi_throw_error(env, "189", "Wrong signature size");
      return NULL;
   }

   p[sz_tmp]=0;

   ((is_nano_prefix((const char *)_buf, NANO_PREFIX))||(is_nano_prefix((const char *)_buf, XRB_PREFIX)))?
      (type=F_VERIFY_SIG_NANO_WALLET):(type=F_VERIFY_SIG_ASCII_HEX);

   if ((err=f_verify_signed_data((const unsigned char *)p, (const unsigned char *)buffer, buffer_sz, (const void *)_buf, type|F_IS_SIGNATURE_RAW_HEX_STRING))<0) {
      sprintf(_buf, "%d", err);
      sprintf(p, "Internal C funtion error. Can't verify signed data %s", _buf);
      napi_throw_error(env, _buf, p);
      return NULL;
   }

   if (napi_get_boolean(env, (bool)(err>0), &res)!=napi_ok) {
      napi_throw_error(env, "190", "Can't determine valid signature of the ArrayBuffer");
      return NULL;
   }

   return res;
}

napi_value nanojs_generate_seed(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv, res;
   size_t argc=1;
   uint32_t entropy;
   uint8_t *p;

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>1) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (!argc) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_value_uint32(env, argv, &entropy)!=napi_ok) {
      napi_throw_error(env, "191", "Can't parse entropy");
      return NULL;
   }

   if (filter_no_entropy_util(entropy)) {
      napi_throw_error(env, NO_ENTROPY_FOUND, NO_ENTROPY_FOUND_MSG);
      return NULL;
   }

   f_random_attach(gen_rand_no_entropy);

   if ((err=f_generate_nano_seed(p=(uint8_t *)(_buf+128), entropy))) {
      sprintf(_buf, "%d", err);
      sprintf((char *)p, ERROR_CANT_GENERATE_ENTROPY, f_get_entropy_name(entropy));
      napi_throw_error(env, (const char *)_buf, (const char *)p);
      return NULL;
   }

   f_random_detach();

   if (napi_create_string_utf8(env, f_nano_key_to_str(_buf, (unsigned char *)p), NAPI_AUTO_LENGTH, &res)!=napi_ok) {
      napi_throw_error(env, "192", "Unable to parse created Nano SEED to JavaScript");
      res=NULL;
   }

   memory_flush();

   return res;
}

#define BIP39_BUFFER_ADJUST (size_t)768
napi_value nanojs_bip39_to_seed(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[2], res;
   size_t argc=2, sz_tmp;
   char *p, *q, *buffer_tmp;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>2) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (!(buffer_tmp=malloc(BIP39_BUFFER_ADJUST))) {
      napi_throw_error(env, NULL, ERROR_FATAL_MALLOC);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], p=buffer_tmp, (BIP39_BUFFER_ADJUST>>1)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_PARSE_BIP39_TO_C_STR, ERROR_CANT_PARSE_BIP39_TO_C_STR_MSG);
      goto nanojs_bip39_to_seed_EXIT1;
   }

   if (sz_tmp==(BIP39_BUFFER_ADJUST>>1)) {
      napi_throw_error(env, ERROR_BIP39_TOO_LONG, ERROR_BIP39_TOO_LONG_MSG);
      goto nanojs_bip39_to_seed_EXIT1;
   }

   p[sz_tmp]=0;

   if (napi_get_value_string_utf8(env, argv[1], q=buffer_tmp+(BIP39_BUFFER_ADJUST>>1), (BIP39_BUFFER_ADJUST>>1)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_PARSE_FILE_AND_PATH, ERROR_PARSE_FILE_AND_PATH_MSG);
      goto nanojs_bip39_to_seed_EXIT1;
   }

   if (sz_tmp==(BIP39_BUFFER_ADJUST>>1)) {
      napi_throw_error(env, ERROR_FILE_PATH_LONG, ERROR_FILE_PATH_TOO_LONG_MSG);
      goto nanojs_bip39_to_seed_EXIT1;
   }

   q[sz_tmp]=0;

   if ((err=f_bip39_to_nano_seed((uint8_t *)(_buf+256), p, q))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, ERROR_CANT_PARSE_BIP39_TO_SEED, _buf);
      napi_throw_error(env, (const char *)_buf, (const char *)_buf+128);
      goto nanojs_bip39_to_seed_EXIT2;
   } 

   if (napi_create_string_utf8(env, f_nano_key_to_str(_buf, (unsigned char *)(_buf+256)), 64, &res)!=napi_ok) {
      napi_throw_error(env, "198", "Unable to parse extracted Nano SEED to JavaScript");
      res=NULL;
   }

   return res;

nanojs_bip39_to_seed_EXIT2:
   memory_flush();

nanojs_bip39_to_seed_EXIT1:
   memset(buffer_tmp, 0, BIP39_BUFFER_ADJUST);
   free(buffer_tmp);

   return NULL;
}

napi_value nanojs_encrypted_stream_to_seed(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, buffer_sz, sz_tmp;
   char *p, *q;
   void *buffer;
   char *bip39_buffer;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_arraybuffer_info(env, argv[0], &buffer, &buffer_sz)!=napi_ok) {
      napi_throw_error(env, ERROR_UNABLE_TO_PARSE_ARRAY_BUFFER_TO_ENCRTYPTED_STREAM, ERROR_UNABLE_TO_PARSE_ARRAY_BUFFER_TO_ENCRTYPTED_STREAM_MSG);
      return NULL;
   }

   if (buffer_sz!=sizeof(F_NANO_CRYPTOWALLET)) {
      napi_throw_error(env, ERROR_WRONG_ENCRYPTED_STREAM_SIZE, ERROR_WRONG_ENCRYPTED_STREAM_SIZE_MSG);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[1], p=_buf, (sizeof(_buf)>>1)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_PARSE_PASSWORD, ERROR_CANT_PARSE_PASSWORD_MSG);
      goto nanojs_encrypted_stream_to_seed_EXIT1;
   }

   if (!sz_tmp) {
      napi_throw_error(env, ERROR_EMPTY_PASSWORD, ERROR_EMPTY_PASSWORD_MSG);
      return NULL;
   }

   if (sz_tmp==(sizeof(_buf)>>1)) {
      napi_throw_error(env, ERROR_PASSWORD_TOO_LONG, ERROR_PASSWORD_TOO_LONG_MSG);
      return NULL;
   }

   p[sz_tmp]=0;

   if ((err=f_read_seed((uint8_t *)(q=(_buf+sizeof(_buf)-32)), (const char *)p, buffer, 0, READ_SEED_FROM_STREAM))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, ERROR_CANT_READ_ENCRYPTED_STREAM, _buf);
      napi_throw_error(env, (const char *)_buf, (const char *)_buf+128);
      goto nanojs_encrypted_stream_to_seed_EXIT1;
   }

   if (napi_create_object(env, &res)!=napi_ok) {
      napi_throw_error(env, "505", "myNanoEmbedded C error. Can't create object in 'nanojs_encrypted_stream_to_seed'");
      goto nanojs_encrypted_stream_to_seed_EXIT1;
   }

   bip39_buffer=NULL;

   if (argc==3) {
      if (!(bip39_buffer=malloc(BIP39_BUFFER_ADJUST))) {
         napi_throw_error(env, NULL, ERROR_FATAL_MALLOC);
         goto nanojs_encrypted_stream_to_seed_EXIT1;
      }

      if (napi_get_value_string_utf8(env, argv[2], p=bip39_buffer+(BIP39_BUFFER_ADJUST>>1), (BIP39_BUFFER_ADJUST>>1)+1, &sz_tmp)!=napi_ok) {
         napi_throw_error(env, ERROR_PARSE_FILE_AND_PATH, ERROR_PARSE_FILE_AND_PATH_MSG);
         goto nanojs_encrypted_stream_to_seed_EXIT2;
      }

      if (sz_tmp==(BIP39_BUFFER_ADJUST>>1)) {
         napi_throw_error(env, ERROR_FILE_PATH_LONG, ERROR_FILE_PATH_TOO_LONG_MSG);
         goto nanojs_encrypted_stream_to_seed_EXIT2;
      }

      p[sz_tmp]=0;

      if ((err=f_nano_seed_to_bip39(bip39_buffer, (BIP39_BUFFER_ADJUST>>1), &sz_tmp, (uint8_t *)q, p))) {
         sprintf(_buf, "%d", err);
         sprintf(_buf+128, "Internal error. Can't parse binary seed to Bip39 %s", _buf);
         napi_throw_error(env, (const char *)_buf, (const char *)_buf+128);
         goto nanojs_encrypted_stream_to_seed_EXIT2;
      }

      if (napi_create_string_utf8(env, bip39_buffer, sz_tmp, &argv[1])!=napi_ok) {
         napi_throw_error(env, "503", "Unable to parse extracted Bip39 to JSON parameter");
         goto nanojs_encrypted_stream_to_seed_EXIT2;
      }

   }

   if (napi_create_string_utf8(env, f_nano_key_to_str(_buf, (unsigned char *)q), 64, &argv[0])!=napi_ok) {
      napi_throw_error(env, "504", "Unable to parse extracted Nano SEED to JSON parameter");
      goto nanojs_encrypted_stream_to_seed_EXIT3;
   }

   if (napi_set_named_property(env, res, "seed", argv[0])!=napi_ok) {
      napi_throw_error(env, "506", "myNanoEmbedded C error. Can't set 'seed' property to 'nanojs_encrypted_stream_to_seed'");
      goto nanojs_encrypted_stream_to_seed_EXIT3;
   }

   if (!bip39_buffer)
      goto nanojs_encrypted_stream_to_seed_EXIT4;

   if (napi_set_named_property(env, res, "bip39", argv[1])!=napi_ok) {
      napi_throw_error(env, "507", "myNanoEmbedded C error. Can't set 'bip39' property to 'nanojs_encrypted_stream_to_seed'");
      res=NULL;
   }

   memset(bip39_buffer, 0, BIP39_BUFFER_ADJUST);
   free(bip39_buffer);

nanojs_encrypted_stream_to_seed_EXIT4:
   memory_flush();
   return res;

nanojs_encrypted_stream_to_seed_EXIT3:
   if (bip39_buffer) {
nanojs_encrypted_stream_to_seed_EXIT2:
      memset(bip39_buffer, 0, BIP39_BUFFER_ADJUST);
      free(bip39_buffer);
   }

nanojs_encrypted_stream_to_seed_EXIT1:
   memory_flush();
   return NULL;
}

napi_value nanojs_gen_seed_to_encrypted_stream(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[2], res;
   size_t argc=2, buffer_sz;
   char *p, *buffer;
   uint32_t entropy;
   uint8_t *encrypted_stream;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>2) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   entropy=0;

   if (napi_get_value_string_utf8(env, argv[0], p=_buf+128, 65+1, &buffer_sz)==napi_ok) {
      if (buffer_sz!=64) {
         napi_throw_error(env, ERROR_WRONG_NANO_SEED_SIZE, ERROR_WRONG_NANO_SEED_SIZE_MSG);
         goto nanojs_gen_seed_to_encrypted_stream_EXIT1;
      }

      p[64]=0;

      if ((err=f_str_to_hex((uint8_t *)(buffer=_buf), p))) {
         sprintf(_buf, "%d", err);
         napi_throw_error(env, (const char *)_buf, ERROR_CANT_CONVERT_NANO_SEED_TO_BINARY);
         goto nanojs_gen_seed_to_encrypted_stream_EXIT1;
      }
   } else if (napi_get_arraybuffer_info(env, argv[0], (void **)&buffer, &buffer_sz)==napi_ok) {
      if (buffer_sz!=32) {
         napi_throw_error(env, "510", ERROR_WRONG_NANO_SEED_SIZE_MSG);
         goto nanojs_gen_seed_to_encrypted_stream_EXIT1;
      }
   } else if (napi_get_value_uint32(env, argv[0], &entropy)!=napi_ok) {
      napi_throw_error(env, "511", "Can't determine SEED|ArrayBuffer binary Seed|Entropy");
      return NULL;
   } else if (!entropy) {
      napi_throw_error(env, "512", "Forbidden entropy value");
      return NULL;
   }

   if (entropy) {

      if (filter_no_entropy_util(entropy)) {
         napi_throw_error(env, NO_ENTROPY_FOUND, NO_ENTROPY_FOUND_MSG);
         return NULL;
      }

      f_random_attach(gen_rand_no_entropy);

      if ((err=f_generate_nano_seed((uint8_t *)(buffer=_buf), entropy))) {
         sprintf(_buf, "%d", err);
         sprintf(p=(_buf+128), ERROR_CANT_GENERATE_ENTROPY, f_get_entropy_name(entropy));
         napi_throw_error(env, (const char *)_buf, (const char *)p);
      }

      f_random_detach();

      if (err)
         return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[1], p=_buf+32, (sizeof(_buf)-32)+1, &buffer_sz)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_PARSE_PASSWORD, ERROR_CANT_PARSE_PASSWORD_MSG);
      goto nanojs_gen_seed_to_encrypted_stream_EXIT1;
   }

   p[buffer_sz]=0;

   if ((err=f_pass_must_have_at_least(p, (sizeof(_buf)-32), MIN_PASSWORD_SZ, (sizeof(_buf)-32)-1, PASS_MUST_HAVE))) {
      sprintf(_buf, "%d", err*=-1);
      napi_throw_error(env, (const char *)_buf, (const char *)verify_password_util(_buf+128, err));
      goto nanojs_gen_seed_to_encrypted_stream_EXIT1;
   }

   if (!(encrypted_stream=malloc(sizeof(F_NANO_CRYPTOWALLET)))) {
      napi_throw_error(env, NULL, ERROR_FATAL_MALLOC);
      goto nanojs_gen_seed_to_encrypted_stream_EXIT1;
   }

   if ((err=f_write_seed(encrypted_stream, WRITE_SEED_TO_STREAM, (uint8_t *)buffer, p))) {
      sprintf(_buf, "%d", err);
      sprintf(p, "Can't write encrypted Nano Seed to memory %s", _buf);
      napi_throw_error(env, (const char *)_buf, (const char *)p);
      goto nanojs_gen_seed_to_encrypted_stream_EXIT2;
   }

   if (napi_create_arraybuffer(env, sizeof(F_NANO_CRYPTOWALLET), (void **)&buffer, &res)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_CREATE_ARRAY_BUFFER_ENC_BLOCK, ERROR_CANT_CREATE_ARRAY_BUFFER_ENC_BLOCK_MSG);
      goto nanojs_gen_seed_to_encrypted_stream_EXIT2;
   }

   if (napi_create_external_arraybuffer(env, memcpy(buffer, encrypted_stream, sizeof(F_NANO_CRYPTOWALLET)),
      sizeof(F_NANO_CRYPTOWALLET), NULL, NULL, &res)!=napi_ok) {

      napi_throw_error(env, ERROR_CANT_PARSE_ENCRIPTED_STREAM_TO_ARRAY_BUFFER, ERROR_CANT_PARSE_ENCRIPTED_STREAM_TO_ARRAY_BUFFER_MSG);
      res=NULL;

   }

   memset(encrypted_stream, 0, sizeof(F_NANO_CRYPTOWALLET));
   free(encrypted_stream);
   memory_flush();
   return res;

nanojs_gen_seed_to_encrypted_stream_EXIT2:
   memset(encrypted_stream, 0, sizeof(F_NANO_CRYPTOWALLET));
   free(encrypted_stream);

nanojs_gen_seed_to_encrypted_stream_EXIT1:
   memory_flush();
   return NULL;
}

napi_value nanojs_bip39_to_encrypted_stream(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp;
   char *p, *bip39_buffer, *q;
   uint8_t *buffer;

// bip39 path password
   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<3) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (!(bip39_buffer=malloc(BIP39_BUFFER_ADJUST))) {
      napi_throw_error(env, NULL, ERROR_FATAL_MALLOC);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[2], p=(bip39_buffer+(BIP39_BUFFER_ADJUST>>1)), (BIP39_BUFFER_ADJUST>>1)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_PARSE_PASSWORD, ERROR_CANT_PARSE_PASSWORD_MSG);
      goto nanojs_bip39_to_encrypted_stream_EXIT1;
   }

   if (sz_tmp==(BIP39_BUFFER_ADJUST>>1)) {
      napi_throw_error(env, ERROR_PASSWORD_TOO_LONG, ERROR_PASSWORD_TOO_LONG_MSG);
      goto nanojs_bip39_to_encrypted_stream_EXIT1;
   }

   p[sz_tmp]=0;

   if ((err=f_pass_must_have_at_least(p, (BIP39_BUFFER_ADJUST>>1), MIN_PASSWORD_SZ, (BIP39_BUFFER_ADJUST>>1)-1, PASS_MUST_HAVE))) {
      sprintf(_buf, "%d", err*=-1);
      napi_throw_error(env, (const char *)_buf, (const char *)verify_password_util(_buf+128, err));
      goto nanojs_bip39_to_encrypted_stream_EXIT1;
   }

   if (napi_get_value_string_utf8(env, argv[0], bip39_buffer, (BIP39_BUFFER_ADJUST>>1)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_PARSE_BIP39_TO_C_STR, ERROR_CANT_PARSE_BIP39_TO_C_STR_MSG);
      goto nanojs_bip39_to_encrypted_stream_EXIT1;
   }

   if (sz_tmp==(BIP39_BUFFER_ADJUST>>1)) {
      napi_throw_error(env, ERROR_BIP39_TOO_LONG, ERROR_BIP39_TOO_LONG_MSG);
      goto nanojs_bip39_to_encrypted_stream_EXIT1;
   }

   bip39_buffer[sz_tmp]=0;

   if (napi_get_value_string_utf8(env, argv[1], q=(_buf+32), (sizeof(_buf)-32)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_PARSE_BIP39_TO_C_STR, ERROR_CANT_PARSE_BIP39_TO_C_STR_MSG);
      goto nanojs_bip39_to_encrypted_stream_EXIT2;
   }

   if (sz_tmp==(sizeof(_buf)-32)) {
      napi_throw_error(env, ERROR_FILE_PATH_LONG, ERROR_FILE_PATH_TOO_LONG_MSG);
      goto nanojs_bip39_to_encrypted_stream_EXIT2;
   }

   q[sz_tmp]=0;

   if ((err=f_bip39_to_nano_seed((uint8_t *)_buf, bip39_buffer, q))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, ERROR_CANT_PARSE_BIP39_TO_SEED, _buf);
      napi_throw_error(env, (const char *)_buf, (const char *)_buf+128);
      goto nanojs_bip39_to_encrypted_stream_EXIT2;
   }

   if ((err=f_write_seed(q, WRITE_SEED_TO_STREAM, (uint8_t *)_buf, p))) {
      sprintf(_buf, "%d", err);
      sprintf(q, "Can't write encrypted Bip39 to memory %s", _buf);
      napi_throw_error(env, (const char *)_buf, (const char *)q);
      goto nanojs_bip39_to_encrypted_stream_EXIT2;
   }

   if (napi_create_arraybuffer(env, sizeof(F_NANO_CRYPTOWALLET), (void **)&buffer, &res)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_CREATE_ARRAY_BUFFER_ENC_BLOCK, ERROR_CANT_CREATE_ARRAY_BUFFER_ENC_BLOCK_MSG);
      goto nanojs_bip39_to_encrypted_stream_EXIT2;
   }

   if (napi_create_external_arraybuffer(env, memcpy(buffer, q, sizeof(F_NANO_CRYPTOWALLET)),
      sizeof(F_NANO_CRYPTOWALLET), NULL, NULL, &res)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_PARSE_ENCRIPTED_STREAM_TO_ARRAY_BUFFER, ERROR_CANT_PARSE_ENCRIPTED_STREAM_TO_ARRAY_BUFFER_MSG);
      res=NULL;

   }

   memory_flush();
   memset(bip39_buffer, 0, BIP39_BUFFER_ADJUST);
   free(bip39_buffer);
   return res;

nanojs_bip39_to_encrypted_stream_EXIT2:
   memory_flush();

nanojs_bip39_to_encrypted_stream_EXIT1:
   memset(bip39_buffer, 0, BIP39_BUFFER_ADJUST);
   free(bip39_buffer);
   return NULL;
}

napi_value nanojs_seed_to_keypair(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp;
   uint32_t wallet_number;
   char *p, *q, *prefix;
   uint8_t *buffer;
//seed(hexstr or arraybuffer), wallet_number, prefix(optional)

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   buffer=NULL;

   if (napi_get_value_string_utf8(env, argv[0], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      if (napi_get_arraybuffer_info(env, argv[0], (void **)&buffer, &sz_tmp)!=napi_ok) {
         napi_throw_error(env, "515", "Can't parse string/array buffer to C binary data");
         goto nanojs_seed_to_keypair_EXIT1;
      }

      if (sz_tmp!=32) {
         napi_throw_error(env, ERROR_WRONG_NANO_SEED_SIZE, ERROR_WRONG_NANO_SEED_SIZE_MSG);
         goto nanojs_seed_to_keypair_EXIT1;
      }

      memcpy(_buf, buffer, 32);
   } else if (sz_tmp!=64) {
      napi_throw_error(env, ERROR_WRONG_NANO_SEED_SIZE, ERROR_WRONG_NANO_SEED_SIZE_MSG);
      goto nanojs_seed_to_keypair_EXIT1;
   } else
      _buf[64]=0;

   if (napi_get_value_uint32(env, argv[1], &wallet_number)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_EXPORT_WALLET_NUMBER, ERROR_CANT_EXPORT_WALLET_NUMBER_MSG);
      goto nanojs_seed_to_keypair_EXIT1;
   }

   if (argc==3) {
      if (napi_get_value_string_utf8(env, argv[2], prefix=(_buf+96), sizeof(NANO_PREFIX)+1, &sz_tmp)!=napi_ok) {
         napi_throw_error(env, ERROR_CANT_PARSE_NANO_XRB_PREFIX, ERROR_CANT_PARSE_NANO_XRB_PREFIX_MSG);
         goto nanojs_seed_to_keypair_EXIT1;
      }

      if (sz_tmp==sizeof(NANO_PREFIX)) {
         napi_throw_error(env, ERROR_WRONG_PREFIX_SIZE, ERROR_WRONG_PREFIX_SIZE_MSG);
         goto nanojs_seed_to_keypair_EXIT1;
      }

      prefix[sz_tmp]=0;
   } else
      prefix=NANO_PREFIX;

   if ((err=seed2keypair_util(_buf, &p, &q, wallet_number, (const char *)prefix, buffer==NULL))) {
      napi_throw_error(env, (const char *)p, (const char *)q);
      goto nanojs_seed_to_keypair_EXIT1;
   }

   if (napi_create_object(env, &res)!=napi_ok) {
      napi_throw_error(env, ERROR_CREATING_OBJECT, ERROR_CREATING_OBJECT_MSG);
      goto nanojs_seed_to_keypair_EXIT1;
   }

   if ((err=create_object_keypair_util(env, res, p, q, wallet_number, _buf))) {
      sprintf(_buf, "%d", err);
      sprintf(p=_buf+128, ERROR_INTERNAL_C_FUNCTION_CREATE_OBJECT_MSG, _buf);
      napi_throw_error(env, (const char *)_buf, (const char *)p);
      res=NULL;
   }

   memory_flush();
   return res;

nanojs_seed_to_keypair_EXIT1:
   memory_flush();
   return NULL;
}

napi_value nanojs_compare(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[4], res;
   size_t argc=4, sz_tmp, sz_tmp2;
   char *a, *b;
   uint32_t mode_compare, compare_result;
   void *buffer;
// a: string|arrayBuffer, b: string|arrayBuffer, inputType: number, condition: number
   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>4) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<4) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_value_uint32(env, argv[2], &mode_compare)!=napi_ok) {
      napi_throw_error(env, "531", "Can't parse type and comparison values");
      return NULL;
   }

   if (mode_compare&(~(F_NANO_A_RAW_128|F_NANO_A_RAW_STRING|F_NANO_A_REAL_STRING|F_NANO_B_RAW_128|F_NANO_B_RAW_STRING|F_NANO_B_REAL_STRING))) {
      napi_throw_error(env, "532", "Invalid compare values A and/or B");
      return NULL;
   }

   if (mode_compare&F_NANO_A_RAW_128) {
      if (napi_get_arraybuffer_info(env, argv[0], &buffer, &sz_tmp)!=napi_ok) {
         napi_throw_error(env, "528", "Can't parse Nano A value ArrayBuffer");
         return NULL;
      }

      if (sz_tmp>sizeof(f_uint128_t)) {
         napi_throw_error(env, "529", "Binary hex A big number exceeds maximum precision");
         return NULL;
      }

      if ((sz_tmp2=(sizeof(f_uint128_t)-sz_tmp))) {
         memset(a=_buf, 0, sz_tmp2);
         memcpy(a+sz_tmp2, buffer, sz_tmp);
      } else
         a=(char *)buffer;
   } else {
      if (napi_get_value_string_utf8(env, argv[0], a=_buf, (F_RAW_STR_MAX_SZ+1), &sz_tmp)!=napi_ok) {
         napi_throw_error(env, "530", "Can't parse Nano A value");
         return NULL;
      }

      if (sz_tmp==F_RAW_STR_MAX_SZ) {
         napi_throw_error(env, "533", "Nano A string exceeds maximum length");
         return NULL;
      }

      a[sz_tmp]=0;
   }

   if (mode_compare&F_NANO_B_RAW_128) {
      if (napi_get_arraybuffer_info(env, argv[1], &buffer, &sz_tmp)!=napi_ok) {
         napi_throw_error(env, "534", "Can't parse Nano B value ArrayBuffer");
         return NULL;
      }

      if (sz_tmp>sizeof(f_uint128_t)) {
         napi_throw_error(env, "535", "Binary hex B big number exceeds maximum precision");
         return NULL;
      }

      if ((sz_tmp2=(sizeof(f_uint128_t)-sz_tmp))) {
         memset(b=_buf, 0, sz_tmp2);
         memcpy(b+sz_tmp2, buffer, sz_tmp);
      } else
         b=(char *)buffer;
   } else {
      if (napi_get_value_string_utf8(env, argv[1], b=(_buf+F_RAW_STR_MAX_SZ), (F_RAW_STR_MAX_SZ+1), &sz_tmp)!=napi_ok) {
         napi_throw_error(env, "536", "Can't parse Nano B value");
         return NULL;
      }

      if (sz_tmp==F_RAW_STR_MAX_SZ) {
         napi_throw_error(env, "537", "Nano B string exceeds maximum length");
         return NULL;
      }

      b[sz_tmp]=0;
   }

   if (napi_get_value_uint32(env, argv[3], &compare_result)!=napi_ok) {
      napi_throw_error(env, "538", "Can't parse comparison");
      return NULL;
   }

   if (compare_result&(~(F_NANO_COMPARE_LT|F_NANO_COMPARE_EQ|F_NANO_COMPARE_GT))) {
      napi_throw_error(env, "539", "Invalid comparison parameter");
      return NULL;
   }

   if ((err=f_nano_value_compare_value(a, b, &mode_compare))) {
      sprintf(a, "%d", err);
      sprintf(b, "Could not perform a Big number compare %s", a);
      napi_throw_error(env, (const char *)a, (const char *)b);
      return NULL;
   }

   if (napi_get_boolean(env, (bool)((mode_compare&compare_result)>0), &res)!=napi_ok) {
      napi_throw_error(env, "540", "Can't determine big number comparison");
      res=NULL;
   }

   return res;
}

napi_value nanojs_encrypted_stream_to_key_pair(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[4], res;
   size_t argc=4, sz_tmp;
   uint32_t wallet_number;
   char *p, *q, *prefix;
   uint8_t *buffer;
// encrypted stream, password, wallet_number, prefix(optional)
   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>4) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<3) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_value_uint32(env, argv[2], &wallet_number)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_EXPORT_WALLET_NUMBER, ERROR_CANT_EXPORT_WALLET_NUMBER_MSG);
      return NULL;
   }

   if (argc==4) {
      if (napi_get_value_string_utf8(env, argv[3], prefix=(_buf+96), sizeof(NANO_PREFIX)+1, &sz_tmp)!=napi_ok) {
         napi_throw_error(env, ERROR_CANT_PARSE_NANO_XRB_PREFIX, ERROR_CANT_PARSE_NANO_XRB_PREFIX_MSG);
         return NULL;
      }

      if (sz_tmp==sizeof(NANO_PREFIX)) {
         napi_throw_error(env, ERROR_WRONG_PREFIX_SIZE, ERROR_WRONG_PREFIX_SIZE_MSG);
         return NULL;
      }

      prefix[sz_tmp]=0;
   } else
      prefix=NANO_PREFIX;

   if (napi_get_arraybuffer_info(env, argv[0], (void **)&buffer, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_UNABLE_TO_PARSE_ARRAY_BUFFER_TO_ENCRTYPTED_STREAM, ERROR_UNABLE_TO_PARSE_ARRAY_BUFFER_TO_ENCRTYPTED_STREAM_MSG);
      return NULL;
   }

   if (sz_tmp!=sizeof(F_NANO_CRYPTOWALLET)) {
      napi_throw_error(env, ERROR_WRONG_ENCRYPTED_STREAM_SIZE, ERROR_WRONG_ENCRYPTED_STREAM_SIZE_MSG);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[1], p=_buf+(sizeof(_buf)>>1), ((sizeof(_buf)>>1)-32)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, ERROR_CANT_PARSE_PASSWORD, ERROR_CANT_PARSE_PASSWORD_MSG);
      goto nanojs_encrypted_stream_to_key_pair_EXIT1;
   }

   if (!sz_tmp) {
      napi_throw_error(env, ERROR_EMPTY_PASSWORD, ERROR_EMPTY_PASSWORD_MSG);
      goto nanojs_encrypted_stream_to_key_pair_EXIT1;
   }

   if (sz_tmp==((sizeof(_buf)>>1)-32)) {
      napi_throw_error(env, ERROR_PASSWORD_TOO_LONG, ERROR_PASSWORD_TOO_LONG_MSG);
      goto nanojs_encrypted_stream_to_key_pair_EXIT1;
   }

   p[sz_tmp]=0;

   if ((err=f_read_seed((uint8_t *)_buf, (const char *)p, buffer, 0, READ_SEED_FROM_STREAM))) {
      sprintf(_buf, "%d", err);
      sprintf(p=(_buf+128), ERROR_CANT_READ_ENCRYPTED_STREAM, _buf);
      napi_throw_error(env, (const char *)_buf, (const char *)p);
      goto nanojs_encrypted_stream_to_key_pair_EXIT1;
   }

   if ((err=seed2keypair_util(_buf, &p, &q, wallet_number, (const char *)prefix, 0))) {
      napi_throw_error(env, (const char *)p, (const char *)q);
      goto nanojs_encrypted_stream_to_key_pair_EXIT1;
   }

   if (napi_create_object(env, &res)!=napi_ok) {
      napi_throw_error(env, ERROR_CREATING_OBJECT, ERROR_CREATING_OBJECT_MSG);
      goto nanojs_encrypted_stream_to_key_pair_EXIT1;
   }

   if ((err=create_object_keypair_util(env, res, p, q, wallet_number, _buf))) {
      sprintf(_buf, "%d", err);
      sprintf(p=(_buf+128), ERROR_INTERNAL_C_FUNCTION_CREATE_OBJECT_MSG, _buf);
      napi_throw_error(env, (const char *)_buf, (const char *)p);
      res=NULL;
   }

   memory_flush();
   return res;

nanojs_encrypted_stream_to_key_pair_EXIT1:
   memory_flush();
   return NULL;
}

MY_NANO_JS_FUNCTION NANO_JS_FUNCTIONS[] = {

   {"nanojs_license", nanojs_license},
   {"nanojs_wallet_to_public_key", nanojs_wallet_to_public_key},
   {"nanojs_seed_to_keypair", nanojs_seed_to_keypair},
   {"nanojs_add_sub", nanojs_add_sub},
   {"nanojs_pow", nanojs_pow},
   {"nanojs_extract_seed_from_brainwallet", nanojs_extract_seed_from_brainwallet},
   {"nanojs_create_block", nanojs_create_block},
   {"nanojs_block_to_JSON", nanojs_block_to_JSON},
   {"nanojs_convert_balance", nanojs_convert_balance},
   {"nanojs_sign_block", nanojs_sign_block},
   {"nanojs_public_key_to_wallet", nanojs_public_key_to_wallet},
   {"nanojs_get_block_hash", nanojs_get_block_hash},
   {"nanojs_verify_message", nanojs_verify_message},
   {"nanojs_generate_seed", nanojs_generate_seed},
   {"nanojs_bip39_to_seed", nanojs_bip39_to_seed},
   {"nanojs_encrypted_stream_to_seed", nanojs_encrypted_stream_to_seed},
   {"nanojs_gen_seed_to_encrypted_stream", nanojs_gen_seed_to_encrypted_stream},
   {"nanojs_bip39_to_encrypted_stream", nanojs_bip39_to_encrypted_stream},
   {"nanojs_compare", nanojs_compare},
   {"nanojs_encrypted_stream_to_key_pair", nanojs_encrypted_stream_to_key_pair},
   {NULL, NULL}

};

MY_NANO_JS_CONST_CHAR NANO_JS_CHAR_CONSTS[] = {

   {"NANO_PREFIX", NANO_PREFIX},
   {"XRB_PREFIX", XRB_PREFIX},
   {NULL, NULL}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_CONST[] = {

   {"VALUE_TO_SEND", VALUE_TO_SEND},
   {"VALUE_TO_RECEIVE", VALUE_TO_RECEIVE},
   {"BALANCE_RAW_128", BALANCE_RAW_128},
   {"BALANCE_REAL_STRING", BALANCE_REAL_STRING},
   {"BALANCE_RAW_STRING", BALANCE_RAW_STRING},
   {"VALUE_SEND_RECEIVE_RAW_128", VALUE_SEND_RECEIVE_RAW_128},
   {"VALUE_SEND_RECEIVE_REAL_STRING", VALUE_SEND_RECEIVE_REAL_STRING},
   {"VALUE_SEND_RECEIVE_RAW_STRING", VALUE_SEND_RECEIVE_RAW_STRING},
   {NULL, 0}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_BIG_NUMBER_CONST[] = {

   {"NANO_ADD_A_B", F_NANO_ADD_A_B},
   {"NANO_SUB_A_B", F_NANO_SUB_A_B},
   {"NANO_RES_RAW_128", F_NANO_RES_RAW_128},
   {"NANO_RES_RAW_STRING", F_NANO_RES_RAW_STRING},
   {"NANO_RES_REAL_STRING", F_NANO_RES_REAL_STRING},
   {"NANO_A_RAW_128", F_NANO_A_RAW_128},
   {"NANO_A_RAW_STRING", F_NANO_A_RAW_STRING},
   {"NANO_A_REAL_STRING", F_NANO_A_REAL_STRING},
   {"NANO_B_RAW_128", F_NANO_B_RAW_128},
   {"NANO_B_RAW_STRING", F_NANO_B_RAW_STRING},
   {"NANO_B_REAL_STRING", F_NANO_B_REAL_STRING},
   {"REAL_TO_RAW", REAL_TO_RAW},
   {"RAW_TO_REAL", RAW_TO_REAL},
   {"REAL_TO_HEX", REAL_TO_HEX},
   {"HEX_TO_REAL", HEX_TO_REAL},
   {"RAW_TO_HEX", RAW_TO_HEX},
   {"HEX_TO_RAW", HEX_TO_RAW},
   {NULL, 0}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_BRAINWALLET_CONST[] = {

   {"BRAIN_WALLET_VERY_POOR", F_BRAIN_WALLET_VERY_POOR},
   {"BRAIN_WALLET_POOR", F_BRAIN_WALLET_POOR},
   {"BRAIN_WALLET_VERY_BAD", F_BRAIN_WALLET_VERY_BAD},
   {"BRAIN_WALLET_BAD", F_BRAIN_WALLET_BAD},
   {"BRAIN_WALLET_VERY_WEAK", F_BRAIN_WALLET_VERY_WEAK},
   {"BRAIN_WALLET_WEAK", F_BRAIN_WALLET_WEAK},
   {"BRAIN_WALLET_STILL_WEAK", F_BRAIN_WALLET_STILL_WEAK},
   {"BRAIN_WALLET_MAYBE_GOOD", F_BRAIN_WALLET_MAYBE_GOOD},
   {"BRAIN_WALLET_GOOD", F_BRAIN_WALLET_GOOD},
   {"BRAIN_WALLET_VERY_GOOD", F_BRAIN_WALLET_VERY_GOOD},
   {"BRAIN_WALLET_NICE", F_BRAIN_WALLET_NICE},
   {"BRAIN_WALLET_PERFECT", F_BRAIN_WALLET_PERFECT},
   {NULL, 0}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_PASSWORD_STRENGTH_CONST[] = {

   {"PASS_MUST_HAVE_AT_LEAST_ONE_NUMBER", F_PASS_MUST_HAVE_AT_LEAST_ONE_NUMBER},
   {"PASS_MUST_HAVE_AT_LEAST_ONE_SYMBOL", F_PASS_MUST_HAVE_AT_LEAST_ONE_SYMBOL},
   {"PASS_MUST_HAVE_AT_LEAST_ONE_UPPER_CASE", F_PASS_MUST_HAVE_AT_LEAST_ONE_UPPER_CASE},
   {"PASS_MUST_HAVE_AT_LEAST_ONE_LOWER_CASE", F_PASS_MUST_HAVE_AT_LEAST_ONE_LOWER_CASE},
   {NULL, 0}

};

MY_NANO_JS_CONST_UINT64_T NANO_CONST_UINT64[] = {

   {"DEFAULT_THRESHOLD", F_DEFAULT_THRESHOLD},
   {NULL, 0}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_ENTROPY_CONST[] = {

   {"ENTROPY_TYPE_PARANOIC", F_ENTROPY_TYPE_PARANOIC},
   {"ENTROPY_TYPE_EXCELENT", F_ENTROPY_TYPE_EXCELENT},
   {"ENTROPY_TYPE_GOOD", F_ENTROPY_TYPE_GOOD},
   {"ENTROPY_TYPE_NOT_ENOUGH", F_ENTROPY_TYPE_NOT_ENOUGH},
   {"ENTROPY_TYPE_NOT_RECOMENDED", F_ENTROPY_TYPE_NOT_RECOMENDED},
   {NULL, 0}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_BIG_NUMBER_CONDITIONAL_CONST[] = {

   {"NANO_COMPARE_EQ", F_NANO_COMPARE_EQ},
   {"NANO_COMPARE_LT", F_NANO_COMPARE_LT},
   {"NANO_COMPARE_LEQ", F_NANO_COMPARE_LEQ},
   {"NANO_COMPARE_GT", F_NANO_COMPARE_GT},
   {"NANO_COMPARE_GEQ", F_NANO_COMPARE_GEQ},
   {NULL, 0}

};

napi_value Init(napi_env env, napi_value exports)
{
   int err;

   if ((err=mynanojs_add_nano_function_util(env, exports, NANO_JS_FUNCTIONS))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_nano_function_util", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_char_constant_util(env, exports, NANO_JS_CHAR_CONSTS))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_char_constant_util", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_uint32_constant_util(env, exports, NANO_UINT32_CONST))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_uint32_constant_util", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_uint64_constant_util(env, exports, NANO_CONST_UINT64))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_uint64_constant_util", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_init_property("PASSWORD_SELECTION", env, exports, mynanojs_add_uint32_constant_util, (void *)NANO_UINT32_PASSWORD_STRENGTH_CONST))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_init_property @ PASSWORD_SELECTION", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_init_property("NANO_BIG_NUMBER_TYPE", env, exports, mynanojs_add_uint32_constant_util, (void *)NANO_UINT32_BIG_NUMBER_CONST))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_init_property @ NANO_BIG_NUMBER_TYPE", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_init_property("NANO_BIG_NUMBER_CONDITIONAL", env, exports, mynanojs_add_uint32_constant_util, 
      (void *)NANO_UINT32_BIG_NUMBER_CONDITIONAL_CONST))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_init_property @ NANO_BIG_NUMBER_CONDITIONAL", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_init_property("BRAINWALLET_TYPE", env, exports, mynanojs_add_uint32_constant_util, (void *)NANO_UINT32_BRAINWALLET_CONST))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_init_property @ BRAINWALLET_TYPE", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_init_property("ENTROPY_TYPE", env, exports, mynanojs_add_uint32_constant_util, (void *)NANO_UINT32_ENTROPY_CONST))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_init_property @ ENTROPY_TYPE", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

