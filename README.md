Card-Game
=========
## Requirements
+ Platform: Linux or Windows
+ OpenSSL 1.1.0i (x64 DLL/lib included for Windows)
+ PostgreSQL 11 (x64 DLL/lib included for Windows)
+ C++17
+ C# 7.3

## Build Status
[![Build Status](https://travis-ci.org/Arcidev/Card-Game.svg)](https://travis-ci.org/Arcidev/Card-Game) (Ubuntu Xenial with G++ 7.1 - Server only)

[![Build status](https://ci.appveyor.com/api/projects/status/0610759ka3nks9mb?svg=true)](https://ci.appveyor.com/project/Arcidev/card-game) (Visual Studio 16.1)

## How to build and run:
  <b>Windows (Automated for MS Visual Studio 15):</b>
  
    Server:
      Copy DLL files from directory Server/DLL to directory where you have built executable
      Initialize PostgreSQL Database from Server/SQL and provide config to CardGame.conf file
    Client:
      Copy directory Client/Assets to directory where you have built an executable
  <b>UNIX-like systems:</b>
  
    Server:
      Build application via script BUILD or BUILD_REALEASE
      Initialize PostgreSQL Database from Server/SQL and provide config to CardGame.conf file (must be copied from Server folder)
    Client:
      Not officially supported yet
      
## Copyright
[![license](https://img.shields.io/github/license/Arcidev/Card-Game.svg?maxAge=2592000?style=plastic)](LICENSE.md)

## Links
[Wiki](https://github.com/Arcidev/Card-Game/wiki)
