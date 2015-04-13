Card-Game
=========
## Requirements
+ Platform: Linux or Windows
+ OpenSSL 1.0.1e (included for Windows)
+ Sqlite 3.7.13 (included for Windows)
+ GCC = 4.7.2 (Linux only)
+ MS Visual Studio = 12 (2013) (Windows only)

##How to build and run:
  <b>Windows:</b>
  
    Server:
      Copy DLL files from directory Server/DLL to directory where you have builded executable (by default Server/Build/RealeaseOrDebug)
      Copy directory Server/Database to directory above an executable (by default Server/Build)
    Client:
      Copy directory Client/Assets to directory where you have builded an executable (by default Client/Build/RealeaseOrDebug)
  <b>UNIX-like systems:</b>
  
    Server:
      Build application via script BUILD or BUILD_REALEASE
      Copy directory Server/Database to directory above an executable (by default Server)
    Client:
      Not officially supported yet
      
## Copyright
<b>License:</b> GPL 3.0

Read file [LICENSE](LICENSE.md)
