# Card-Game

How to build and run:
  Windows:
    Requirements:
      MS Visual Studio 2013
    How to run:
      Server:
        Copy DLL files from directory Server/DLL to directory where you have builded executable (by default Server/Build/RealeaseOrDebug)
        Copy directory Server/Database to directory above an executable (by default Server/Build)
      Client:
        Copy directory Client/Assets to directory where you have builded an executable (by default Client/Build/RealeaseOrDebug)
  UNIX-like systems:
    Server:
      Minimal requirements:
        OpenSSL 1.0.1e
        GCC 4.7.2
        Sqlite 3.7.13
      How to run:
        Build application via script BUILD or BUILD_REALEASE
        Copy directory Server/Database to directory above an executable (by default Server)
    Client:
      Not officially supported yet
