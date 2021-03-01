# rtcpp
Library for RTP dump decryption. Based on libsrtp https://github.com/cisco/libsrtp
Reads rtpdump and decrypts packets, then generates rptdump with the same packet headers, but decrypted content.

Has bindings for web assembly and java.

In order to build for webassembly you need to:
1. Configure emscripten enviroment source 
2. run buildjs.sh script

Otherwise follow standard CMake procedure. 
