const MY_NANO_JS = require('./build/Release/mynanojs');

console.log(MY_NANO_JS);
console.log(MY_NANO_JS.nanojs_wallet_to_public_key('xrb_3tta9pdxr4djdcm6r3c7969syoirj3dunrtynmmi8n1qtxzk9iksoz1gxdrh'));
KEY_PAIR=MY_NANO_JS.nanojs_seed_to_keypair('86A4EBE059446FF84F8C4FAC7C56EF97D16DA7369845051414A0A6DA5EB99FCB', 1728);
console.log(KEY_PAIR);
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
KEY_PAIR.publicKey,
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
console.log(MY_NANO_JS.nanojs_convert_balance('100', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.HEX_TO_RAW)); //256
console.log(MY_NANO_JS.nanojs_convert_balance('1F0', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.HEX_TO_REAL)); //0.000000000000000000000000000496
console.log(MY_NANO_JS.nanojs_convert_balance('00009d632382e95f9ed7ce322c800000', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.HEX_TO_REAL)); //3192.1928861
console.log(MY_NANO_JS.nanojs_convert_balance('1.2', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.REAL_TO_HEX)); //0000000f2568bc2d21591d7f80000000
console.log(MY_NANO_JS.nanojs_convert_balance('2020150000000000000000000000000000', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.RAW_TO_HEX)); //2020.15
console.log(MY_NANO_JS.nanojs_convert_balance('35.18', MY_NANO_JS.NANO_BIG_NUMBER_TYPE.REAL_TO_HEX)); //000001bc08b5f31a0096b1d9e0000000
console.log(typeof MY_NANO_JS.nanojs_block_to_JSON);
B=MY_NANO_JS.nanojs_sign_block(A, `${KEY_PAIR.privateKey}${KEY_PAIR.publicKey}`);
console.log(MY_NANO_JS.nanojs_block_to_JSON(B));
console.log(MY_NANO_JS.nanojs_get_block_hash(B));
console.log(MY_NANO_JS.nanojs_public_key_to_wallet(KEY_PAIR.publicKey));
console.log(MY_NANO_JS.nanojs_public_key_to_wallet(KEY_PAIR.publicKey, MY_NANO_JS.XRB_PREFIX));
console.log(MY_NANO_JS.nanojs_public_key_to_wallet(KEY_PAIR.publicKey, MY_NANO_JS.NANO_PREFIX));
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

console.log(MY_NANO_JS.nanojs_verify_message(signature, hashArrayBuffer, wallet)); // return true
//console.log(MY_NANO_JS.nanojs_wallet_to_public_key('nano_3xinwsdt57qo5bcysock15do87r9fuepq84erab5udm6wekymq9e9tiin8hw'));
console.log(MY_NANO_JS.nanojs_verify_message(signature, hashArrayBuffer, wallet_public_key)); // return true
console.log(MY_NANO_JS.nanojs_generate_seed(MY_NANO_JS.ENTROPY_TYPE.ENTROPY_TYPE_NOT_RECOMENDED)); // Worst but fast within milliseconds to generate Random SEED
console.log(MY_NANO_JS.nanojs_generate_seed(MY_NANO_JS.ENTROPY_TYPE.ENTROPY_TYPE_PARANOIC)); // Very good but slow 30 s to 2 min to generate Nano SEED
// Random SEED was designed and implemented using topic 7.12 - Z. S. Spakovszky - 2005 -(https://web.mit.edu/16.unified/www/FALL/thermodynamics/notes/node56.html)[https://web.mit.edu/16.unified/www/FALL/thermodynamics/notes/node56.html]


