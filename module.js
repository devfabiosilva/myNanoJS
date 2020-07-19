
const MY_NANO_JS = require('./build/Release/mynanojs');

function stringToArrayBuffer(value) {
   let uint8;
   let buffer;
   let arrayBuffer;

   buffer = Buffer.from(value);
   arrayBuffer = new ArrayBuffer(buffer.length);
   uint8 = new Uint8Array(arrayBuffer);
   uint8.set(buffer);

   return arrayBuffer;
}

function hexToArrayBuffer(value) {
   let uint8;
   let buffer;
   let arrayBuffer;

   buffer = Buffer.from(value, 'hex');
   arrayBuffer = new ArrayBuffer(buffer.length);
   uint8 = new Uint8Array(arrayBuffer);
   uint8.set(buffer);

   return arrayBuffer;
}

console.log(
MY_NANO_JS.nanojs_compare(
hexToArrayBuffer('00009d632382e95f9ed7ce322c800000'), //3192.1928861 Big Number in hex 
'3182', // Real value
MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_A_RAW_128+MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_B_REAL_STRING,
MY_NANO_JS.NANO_BIG_NUMBER_CONDITIONAL.NANO_COMPARE_LT)); // (3192.1928861 <= 3182) = false

console.log(
MY_NANO_JS.nanojs_compare(
'001.0000', // Real value
'1000000000000000000000000000000', // 1.0 (Raw Value)
MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_A_REAL_STRING+MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_B_RAW_STRING,
MY_NANO_JS.NANO_BIG_NUMBER_CONDITIONAL.NANO_COMPARE_EQ)); //(1.0 === 1.0) = true


console.log(MY_NANO_JS.nanojs_compare(
hexToArrayBuffer('10'), //0.000000000000000000000000000016 Big Number in hex 
'0.000000000000000000000000000016', // Real value
MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_A_RAW_128+MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_B_REAL_STRING,
MY_NANO_JS.NANO_BIG_NUMBER_CONDITIONAL.NANO_COMPARE_LEQ)); // (0.000000000000000000000000000016 <= 0.000000000000000000000000000016) = true

console.log(
MY_NANO_JS.nanojs_compare(
'001.0001', // Real value
'1000000000000000000000000000000', // 1.0 (Raw Value)
MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_A_REAL_STRING+MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_B_RAW_STRING,
MY_NANO_JS.NANO_BIG_NUMBER_CONDITIONAL.NANO_COMPARE_EQ)); //(1.0001 === 1.0) = false
console.log('a');
console.log(MY_NANO_JS.nanojs_convert_balance('1000000000000000000000000000000')); //1.0
console.log('b');
console.log(MY_NANO_JS.nanojs_convert_balance(hexToArrayBuffer('1f'), MY_NANO_JS.NANO_BIG_NUMBER_TYPE.HEX_TO_RAW)); //256
console.log('c');
console.log(MY_NANO_JS.nanojs_convert_balance(hexToArrayBuffer('1F'), MY_NANO_JS.NANO_BIG_NUMBER_TYPE.HEX_TO_REAL)); //0.000000000000000000000000000496
console.log('d');
console.log(MY_NANO_JS.nanojs_convert_balance(hexToArrayBuffer('00009d632382e95f9ed7ce322c800000'), MY_NANO_JS.NANO_BIG_NUMBER_TYPE.HEX_TO_REAL)); //3192.1928861
console.log('e');
console.log(MY_NANO_JS.nanojs_convert_balance('1.2', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.REAL_TO_HEX)); //0000000f2568bc2d21591d7f80000000
console.log('f');
console.log(MY_NANO_JS.nanojs_convert_balance('2020150000000000000000000000000000', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.RAW_TO_HEX)); //2020.15
console.log('g');
console.log(MY_NANO_JS.nanojs_convert_balance('35.18', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.REAL_TO_HEX)); //000001bc08b5f31a0096b1d9e0000000
console.log('h');
console.log(MY_NANO_JS.nanojs_convert_balance('1200000000000000000000000000000', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.RAW_TO_REAL)); //1.0
console.log('i');
console.log(MY_NANO_JS.nanojs_convert_balance('1527.212981661', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.REAL_TO_RAW)); //1.0

const ENCRYPTED_BLOCK = hexToArrayBuffer('5f6e616e6f77616c6c657466696c655f000001004e414e4f205365656420456e637279707465642066696c652f73747265616d2e204b656570206974207361666520616e64206261636b75702069742e20546869732066696c652069732070726f7465637465642062792070617373776f72642e2042555920424954434f494e20616e64204e414e4f2021212100a6e2e890abc0f898203aa12a5e0e83307365c473c58c82184b204feb68bd47ebac15ae55e8f97bf005443a2e0dd5f6486c5f8c76605ac95169f05d45c3f36901ac1fa2bc11a694cd07895f016f768910e7ed53f7a22a62767eb6dabdae16ef3d9592b8f518b802566779a94ce73b36c8cb388d648d8cea56de005eee999f6d1e01dc4dc81317dfd49f66a28cc9515c8aa1a15194a0fb3824ed8a3554c897bad29bd49eeaa84558fc4d49adc68ef7a6767d317835a48a4c45c276bb7283ac3054a2e1708d8ccf8188532cd909ff5352a38377');
const ENCRYPTED_BLOCK_PASSWORD = 'MyPasswordHere@1';

console.log(MY_NANO_JS.nanojs_encrypted_stream_to_key_pair(ENCRYPTED_BLOCK, ENCRYPTED_BLOCK_PASSWORD, 320));
/*
{
  private_key: 'BBE0D5AD30C2E8B1EFC3299E5C30FDD168CFE692F5C9401FBAD47C8EECCA2334',
  public_key: 'C4B8BBCE2151B3C8F0BEA28C62BF4593A0034240E1ED9A32DC1C3EF64337BB5F',
  wallet_number: 320,
  wallet: 'nano_3j7rqh944nfms5rdxaneecznd6x11f363rhfmasfr93yys3mhgtz5s57unn3'
}
*/

console.log(MY_NANO_JS.nanojs_encrypted_stream_to_key_pair(ENCRYPTED_BLOCK, ENCRYPTED_BLOCK_PASSWORD, 0, MY_NANO_JS.NANO_PREFIX));
/*
{
  private_key: 'E7F9FAC6BEE76D7F13A9D047210B7154EA6C6B64817D3DF52E28B6F1A3095B11',
  public_key: '94D086DC3C29F3C6AA6ED2D10EDA87D923EC8B7BC08A72C2F0E965CA7488A9D1',
  wallet_number: 0,
  wallet: 'nano_378iiug5rchmrto8xnpj3ufahpb5xk7qqi6cgd3h3td7sbtajcgjbp6nu5zh'
}
*/
console.log(MY_NANO_JS.nanojs_encrypted_stream_to_key_pair(ENCRYPTED_BLOCK, ENCRYPTED_BLOCK_PASSWORD, 1, MY_NANO_JS.XRB_PREFIX));
/*
{
  private_key: '14E8B9DD512BC3AA9787D8CFED2BF586D6C92376C7F120E10962AF7E0E0F1C9B',
  public_key: 'D36563C9E85A3738FDDD6330505DEEABEA27318AFB654157DD7C4A579EAB3106',
  wallet_number: 1,
  wallet: 'xrb_3nu7eh6yipjq95yxtrsic3gyxczc6wrroyu7a7dxtz4ccyhcpea83bnkh4rf'
}
*/

//console.log(MY_NANO_JS.nanojs_pow('de0c84215a6b7429d3d2836f54b6b917c9301103134904457a928c56580cf5a4', 4));
console.log("Fixed 1");
//console.log(MY_NANO_JS.nanojs_pow('fa0c8b217a6b7429d3d2136f54b6b917c9301103134904457a928c56580cf5a3', 4));
console.log('Fixed 2');
console.log(MY_NANO_JS);
console.log(MY_NANO_JS.nanojs_wallet_to_public_key('xrb_3tta9pdxr4djdcm6r3c7969syoirj3dunrtynmmi8n1qtxzk9iksoz1gxdrh'));
const PRIVATE_KEY='86A4EBE059446FF84F8C4FAC7C56EF97D16DA7369845051414A0A6DA5EB99FCD';
const PRIVATE_KEY_ARRAY_BUFFER=hexToArrayBuffer(PRIVATE_KEY);
KEY_PAIR2=MY_NANO_JS.nanojs_seed_to_keypair(PRIVATE_KEY_ARRAY_BUFFER, 1728);
KEY_PAIR=MY_NANO_JS.nanojs_seed_to_keypair(PRIVATE_KEY, 1728);
console.log(KEY_PAIR);
console.log(KEY_PAIR2);
console.log(MY_NANO_JS.nanojs_add_sub(

 "521.1", 
 "11",
 MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_SUB_A_B+
 MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_A_REAL_STRING+
 MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_B_REAL_STRING

));
//console.log(MY_NANO_JS.nanojs_pow('de0c84215a6b7429d3d2836f54b6b917c9301103134904457a928c56580cf5a4', 3, 4n));

console.log(MY_NANO_JS.nanojs_extract_seed_from_brainwallet('Regina caeli laetare, Alleluia, Quia quem meruisti portare, Alleluia, Resurrexit sicut dixit, Alleluia. Ora pro nobis Deum. Alleluia','MyEmailHere+MyPhoneNumberHere+MyIdHere@124+AndEtcToSaltWholeMessageToAvoidBruteForceAttack;)'));

A=MY_NANO_JS.nanojs_create_block(
//'nano_1brainb3zz81wmhxndsbrjb94hx3fhr1fyydmg6iresyk76f3k7y7jiazoji',
KEY_PAIR.public_key,
null,
'xrb_1xckpezrhg56nuokqh6t1stjca67h37jmrp9qnejjkfgimx1msm9ehuaieuq',
'0',
MY_NANO_JS.BALANCE_REAL_STRING,
'5',
MY_NANO_JS.VALUE_SEND_RECEIVE_REAL_STRING,
'2A435EA809381A15A743C57DBF2E7BB06371AEA75792F9416F357C4C1A19AE3F',
MY_NANO_JS.VALUE_TO_RECEIVE
);
console.log("test");
console.log(A);
console.log(MY_NANO_JS.nanojs_block_to_JSON(A));
console.log(MY_NANO_JS.nanojs_convert_balance('1000000000000000000000000000000')); //1.0
console.log(MY_NANO_JS.nanojs_convert_balance(hexToArrayBuffer('100'), MY_NANO_JS.NANO_BIG_NUMBER_TYPE.HEX_TO_RAW)); //256
console.log(MY_NANO_JS.nanojs_convert_balance(hexToArrayBuffer('1F0'), MY_NANO_JS.NANO_BIG_NUMBER_TYPE.HEX_TO_REAL)); //0.000000000000000000000000000496
console.log(MY_NANO_JS.nanojs_convert_balance(hexToArrayBuffer('00009d632382e95f9ed7ce322c800000'), MY_NANO_JS.NANO_BIG_NUMBER_TYPE.HEX_TO_REAL)); //3192.1928861
console.log(MY_NANO_JS.nanojs_convert_balance('1.2', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.REAL_TO_HEX)); //0000000f2568bc2d21591d7f80000000
console.log(MY_NANO_JS.nanojs_convert_balance('2020150000000000000000000000000000', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.RAW_TO_HEX)); //2020.15
console.log(MY_NANO_JS.nanojs_convert_balance('35.18', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.REAL_TO_HEX)); //000001bc08b5f31a0096b1d9e0000000
console.log(typeof MY_NANO_JS.nanojs_block_to_JSON);
B=MY_NANO_JS.nanojs_sign_block(A, `${KEY_PAIR.private_key}${KEY_PAIR.public_key}`);
console.log(MY_NANO_JS.nanojs_block_to_JSON(B));
console.log(MY_NANO_JS.nanojs_get_block_hash(B));
console.log(MY_NANO_JS.nanojs_public_key_to_wallet(hexToArrayBuffer(KEY_PAIR.public_key)));
console.log(MY_NANO_JS.nanojs_public_key_to_wallet(KEY_PAIR.public_key));
console.log(MY_NANO_JS.nanojs_public_key_to_wallet(KEY_PAIR.public_key, MY_NANO_JS.XRB_PREFIX));
console.log(MY_NANO_JS.nanojs_public_key_to_wallet(KEY_PAIR.public_key, MY_NANO_JS.NANO_PREFIX));
console.log(KEY_PAIR.publicKey);

HASH='3FF8EC2F80C4082376C8CB771361F98133546AAC70B3AF877DA286E80BC9A7BE';
wallet='nano_3xinwsdt57qo5bcysock15do87r9fuepq84erab5udm6wekymq9e9tiin8hw';
wallet_public_key='F614E657A196F51A55ECD55200D75317076ED96B984CC2123DAE64E325E9DCEC';
signature='276B9F53A3DB55A64FAA381EBE1BF7FD51F7E1DD1368189A1AA2E84147AD1AF83A506ED6CCBFC0CCCB69D819B93B9B31D909F89141FE73070B0F253A79467505';

hashBuffer = Buffer.from(HASH, 'hex');
hashArrayBuffer = new ArrayBuffer(hashBuffer.length);
hashUint8Array = new Uint8Array(hashArrayBuffer);
hashUint8Array.set(hashBuffer);

console.log(hashUint8Array);
console.log(hashArrayBuffer);
K=stringToArrayBuffer('Mensagem Aqui á `à~ ~');
console.log(K);
const SIGNATURE = MY_NANO_JS.nanojs_sign_message(K, `${KEY_PAIR.private_key}${KEY_PAIR.public_key}`);
console.log(SIGNATURE);
console.log(KEY_PAIR);

console.log(MY_NANO_JS.nanojs_verify_message(SIGNATURE, K, KEY_PAIR.public_key));
console.log(MY_NANO_JS.nanojs_verify_message(SIGNATURE, K, KEY_PAIR.wallet));

console.log(MY_NANO_JS.nanojs_verify_message(SIGNATURE, K, 'xrb_3tta9pdxr4djdcm6r3c7969syoirj3dunrtynmmi8n1qtxzk9iksoz1gxdrh'));

console.log(MY_NANO_JS.nanojs_bip39_to_key_pair('moral history analyst regular resemble belt exercise motor hungry pizza purity convince narrow ancient arrange hard affair cloth pigeon board grain all story kite', 'dictionary.dic', 159, MY_NANO_JS.XRB_PREFIX));

const ACCOUNT = 'nano_3jgio5fnti1t4pmhjtdycudo8g15m3yxmdmu1zu639aqkntntqxqz7nfez1g';
const BALANCE = '10.179';
const DESTINATION_WALLET = 'nano_1pqzs6rei13ynj9uicwi8ru1sucjke41ogjmp19gaj8yq7r1rm6hd4ga3u4b';
const VALUE_TO_SEND = '1.179';
const REPRESENTATIVE = 'xrb_3h1mmkdintyoh5r87g1gkrx6tm1ip5ond3df8mseh6pfomyc613tekt88yqw';
const PREVIOUS = 'c773b9703f5d92c4a1d1c22d9183237c236f8a2fcd3d735ef89de9930f6bbf05';

const NANO_BLOCK = MY_NANO_JS.nanojs_create_block(
	ACCOUNT,
	PREVIOUS,
	REPRESENTATIVE,
	BALANCE,
	MY_NANO_JS.BALANCE_REAL_STRING,
	VALUE_TO_SEND,
	MY_NANO_JS.VALUE_SEND_RECEIVE_REAL_STRING,
	DESTINATION_WALLET,
	MY_NANO_JS.VALUE_TO_SEND
);

console.log(NANO_BLOCK);

const WORKER_FEE = '0.0002';
const WORKER_REPRESENTATIVE = 'nano_3xa1xbpcrp3fqh7135aer4xjy6ug5ekst4fnakpz8o6u4eri1jss87ochr3j';
const WORKER_WALLET = 'xrb_3cha18iwzqrao194aayjt5gw6ozk1eq46pmasrrpkzjuguuieqy1p1cpieez';

const P2POW_BLOCK = MY_NANO_JS.nanojs_block_to_p2pow(
	NANO_BLOCK,
	WORKER_WALLET,
	WORKER_REPRESENTATIVE,
	WORKER_FEE
);

console.log(P2POW_BLOCK);

/*
console.log(MY_NANO_JS.nanojs_verify_message(signature, hashArrayBuffer, wallet)); // return true
//console.log(MY_NANO_JS.nanojs_wallet_to_public_key('nano_3xinwsdt57qo5bcysock15do87r9fuepq84erab5udm6wekymq9e9tiin8hw'));
console.log(MY_NANO_JS.nanojs_verify_message(signature, hashArrayBuffer, wallet_public_key)); // return true
//console.log(MY_NANO_JS.nanojs_generate_seed(MY_NANO_JS.ENTROPY_TYPE.ENTROPY_TYPE_NOT_RECOMENDED)); // Worst but fast within milliseconds to generate Random SEED
//console.log(MY_NANO_JS.nanojs_generate_seed(MY_NANO_JS.ENTROPY_TYPE.ENTROPY_TYPE_PARANOIC)); // Very good but slow 30 s to 2 min to generate Nano SEED
// Random SEED was designed and implemented using topic 7.12 - Z. S. Spakovszky - 2005 -(https://web.mit.edu/16.unified/www/FALL/thermodynamics/notes/node56.html)[https://web.mit.edu/16.unified/www/FALL/thermodynamics/notes/node56.html]
console.log('dry umbrella analyst regular resemble belt exercise motor hungry pizza purity convince narrow ancient arrange hard affair cloth pigeon board grain all story income');
console.log(MY_NANO_JS.nanojs_bip39_to_seed('dry umbrella analyst regular resemble belt exercise motor hungry pizza purity convince narrow ancient arrange hard affair cloth pigeon board grain all story income', 'dictionary.dic')); //43bd84215a6b7429d3d4836f54b6b917c9301103134904457a928c56580cf5a3

encrypted_stream_password = 'myPasswordHere@12345';
encrypted_stream = "5f6e616e6f77616c6c657466696c655f000001004e414e4f205365656420456e637279707465642066696c652f73747265616d2e204b656570206974207361666520616e64206261636b75702069742e20546869732066696c652069732070726f7465637465642062792070617373776f72642e2042555920424954434f494e20616e64204e414e4f2021212100d3dfc3ef437c2667f3ddf5a7365718fba12e7521b4b36ff1382d5b361b8ab3000c745242238bb1d66a5f32993452c8b74e3fadb1e2dc1c2a0a5dc28f745558d475160780ade7e46721c2a32e157937382eba1128c554329846af28de53f55c6134ebd45ef5e75710ab2ba44a6bb8e9af576dcead03294b22552380821075c7f73d6aa325806a2754ca128d642f16c21a8f9656d703fe15dbf065295b5f117591eda3b9ddbab5e4759c813168ffca11501a642cd5b69dfe36bf453d39ef0ebe3a53b19699501cbfbc3adc9dce4b3ae1bf42bd";

encrypted_stream_buffer = Buffer.from(encrypted_stream, 'hex');
encryptedArrayBuffer = new ArrayBuffer(encrypted_stream_buffer.length);
encryptedUint8Array = new Uint8Array(encryptedArrayBuffer);
encryptedUint8Array.set(encrypted_stream_buffer);

console.log(encrypted_stream);
console.log(encryptedArrayBuffer);

console.log(MY_NANO_JS.nanojs_encrypted_stream_to_seed(encryptedArrayBuffer, encrypted_stream_password, 'dictionary.dic'));

const SEED_TEST = 'A614E6573190751A55ECD55200D75B17076ED46B984CC2123DAE64E32518DC2A';
const password_test = 'mypasswordherewithallcharhereA@1';
console.log(MY_NANO_JS.nanojs_gen_seed_to_encrypted_stream(MY_NANO_JS.ENTROPY_TYPE.ENTROPY_TYPE_GOOD, password_test));

SEED_TEST_BUFFER = Buffer.from(SEED_TEST, 'hex');
SEED_TEST_ARRAY_BUFFER = new ArrayBuffer(SEED_TEST_BUFFER.length);
SEED_UINT8_ARRAY = new Uint8Array(SEED_TEST_ARRAY_BUFFER);
SEED_UINT8_ARRAY.set(SEED_TEST_BUFFER);
console.log(SEED_TEST);
console.log(SEED_TEST_ARRAY_BUFFER);
const encrypted_result = MY_NANO_JS.nanojs_gen_seed_to_encrypted_stream(SEED_TEST_ARRAY_BUFFER, password_test);
console.log(encrypted_result);
console.log(MY_NANO_JS.nanojs_encrypted_stream_to_seed(encrypted_result, password_test, 'dictionary.dic'));
console.log(MY_NANO_JS.nanojs_encrypted_stream_to_seed(encrypted_result, password_test));
///const K = MY_NANO_JS.nanojs_gen_seed_to_encrypted_stream(MY_NANO_JS.ENTROPY_TYPE.ENTROPY_TYPE_PARANOIC, password_test);
//console.log(K);

//console.log(MY_NANO_JS.nanojs_encrypted_stream_to_seed(K, password_test, 'dictionary.dic'));
//console.log(MY_NANO_JS.nanojs_encrypted_stream_to_seed(K, password_test));

//const password_test = 'mypasswordherewithallcharhereA@1';
const bip39str = "bargain method analyst radio resemble belt exercise motor hungry pizza purity convince narrow ancient arrange hard affair fortune gorilla allow grace major stock guess";
console.log(bip39str);
const enc_bip39str = MY_NANO_JS.nanojs_bip39_to_encrypted_stream(bip39str, 'dictionary.dic', 'mypasswordherewithallcharhereA@1');

console.log(enc_bip39str);

console.log(MY_NANO_JS.nanojs_encrypted_stream_to_seed(enc_bip39str, 'mypasswordherewithallcharhereA@1', 'dictionary.dic'));

*/

