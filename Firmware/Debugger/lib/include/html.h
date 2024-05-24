#ifndef _HTML_H
#define _HTML_H

PROGMEM const char HTML[] = R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <link rel="icon" href="data:image/x-icon;base64,AAABAAEAEBAAAAEAGABoAwAAFgAAACgAAAAQAAAAIAAAAAEAGAAAAAAAaAMAAAAAAAAAAAAAAAAAAAAAAABQUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFBQUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACfn58AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///////////8AAAD///////8AAAD///////8AAAAAAAD///////////8AAAAAAAD///8AAAD///8AAAAAAAD///8AAAD///8AAAD///8AAAD///8AAAAAAAAAAAAAAAD///////////8AAAD///////8AAAD///8AAAD///8AAAD///////////8AAAAAAAD///8AAAAAAAAAAAD///8AAAAAAAD///8AAAD///8AAAAAAAAAAAD///8AAAAAAAD///////8AAAAAAAD///////8AAAAAAAD///////8AAAD///////////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABQUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFBQUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA">
    <title>6502 Debugger</title>
    <style>body{display:flex;flex-direction:column;font-family:monospace;background-color:#000;color:#fff}a{color:#fff}header{display:flex;flex-direction:row;justify-content:center;white-space:pre}main{display:flex;flex-direction:row;justify-content:center;gap:8px}</style>
  </head>
  <body class="bg-slate-950 text-slate-50 p-8">
    <header>
      eeee  eeeee eeeeee eeee   eeeeee                                            
      8  8  8     8    8    8   8    8 eeee eeeee  e   e eeeee eeeee eeee eeeee   
      8     8eeee 8    8    8   8e   8 8    8   8  8   8 8   8 8   8 8    8   8   
      8eeee     8 8    8 eee8   88   8 8eee 8eee8e 8e  8 8e    8e    8eee 8eee8e  
      8   8     8 8    8 8      88   8 88   88   8 88  8 88 e8 88 e8 88   88   8  
      8eee8 eeee8 8eeee8 8eee   88eee8 88ee 88eee8 88ee8 88ee8 88ee8 88ee 88   8  

                          6502 Debugger | Version: 1.00

                        ---------------------------------
                        | Created by A.C. Wright © 2024 |
                        ---------------------------------
    </header>
    <main>
      <a href="https://github.com/acwright/6502">Home Page</a>
      <span>|</span>
      <a href="https://github.com/acwright/6502/wiki">Documentation</a>
    </main>
  </body>
</html>
)=====";

#endif
