{
  "targets": [
    {
      "target_name": "mynanojs",
      "sources": [ "./src/mynanojs.c", "./src/utility.c" ],
      "include_dirs":[ "./include", "./include/sodium" ],
      "libraries": [
              "../lib/libnanocrypto1.a", "../lib/libsodium.a"
          ],
    }
  ]
}

