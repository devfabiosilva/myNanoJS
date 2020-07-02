const MY_NANO_JS = require('./build/Release/mynanojs');

console.log(MY_NANO_JS);
console.log(MY_NANO_JS.nanojs_wallet_to_public_key('xrb_3tta9pdxr4djdcm6r3c7969syoirj3dunrtynmmi8n1qtxzk9iksoz1gxdrh'));
console.log(MY_NANO_JS.nanojs_seed_to_nano_wallet('86A4EBE059446FF84F8C4FAC7C56EF97D16DA7369845051414A0A6DA5EB99FCA', 1728));
console.log(MY_NANO_JS.nanojs_add_sub(

 "521.1", 
 "11",
 MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_SUB_A_B+
 MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_A_REAL_STRING+
 MY_NANO_JS.NANO_BIG_NUMBER_TYPE.NANO_B_REAL_STRING

));
console.log(MY_NANO_JS.nanojs_pow('de0c84215a6b7429d3d2836f54b6b917c9301103134904457a928c56580cf5a4', 3, 4n));
//5213761273712736712637162361278631273178362236163251536136516

console.log(MY_NANO_JS.nanojs_extract_seed_from_brainwallet('Regina caeli laetare, Alleluia, Quia quem meruisti portare, Alleluia, Resurrexit sicut dixit, Alleluia. Ora pro nobis Deum. Alleluia','MyEmailHere+MyPhoneNumberHere+MyIdHere@124+AndEtcToSaltWholeMessageToAvoidBruteForceAttack;)'));

A=MY_NANO_JS.nanojs_create_block(
'nano_1brainb3zz81wmhxndsbrjb94hx3fhr1fyydmg6iresyk76f3k7y7jiazoji',
null,
'xrb_1xckpezrhg56nuokqh6t1stjca67h37jmrp9qnejjkfgimx1msm9ehuaieuq',
'0',
MY_NANO_JS.BALANCE_REAL_STRING,
'5',
MY_NANO_JS.VALUE_SEND_RECEIVE_REAL_STRING,
'2A435EA809381A15A743C57DBF2E7BB06371AEA75792F9416F357C4C1A19AE3F',
MY_NANO_JS.VALUE_TO_RECEIVE
);

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

