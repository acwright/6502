#ifndef _HTML_H
#define _HTML_H

PROGMEM const char HTML[] = R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <link rel="icon" href="data:image/x-icon;base64,AAABAAEAEBAAAAEAGABoAwAAFgAAACgAAAAQAAAAIAAAAAEAGAAAAAAAaAMAAAAAAAAAAAAAAAAAAAAAAABQUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFBQUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACfn58AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///////////8AAAD///////8AAAD///////8AAAAAAAD///////////8AAAAAAAD///8AAAD///8AAAAAAAD///8AAAD///8AAAD///8AAAD///8AAAAAAAAAAAAAAAD///////////8AAAD///////8AAAD///8AAAD///8AAAD///////////8AAAAAAAD///8AAAAAAAAAAAD///8AAAAAAAD///8AAAD///8AAAAAAAAAAAD///8AAAAAAAD///////8AAAAAAAD///////8AAAAAAAD///////8AAAD///////////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABQUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFBQUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFCfn5+fn59QUFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA">
    <title>DB Emulator</title>
    <style>body{display:flex;flex-direction:column;justify-content:center;align-items:center;height:100vh;margin:0;font-family:monospace;background-color:#000;color:#fff}a{color:#fff}header{display:flex;flex-direction:row;justify-content:center;white-space:pre}main{display:flex;flex-direction:column}div{display:flex;flex-direction:row;justify-content:center}div#links{gap:8px}</style>
  </head>
  <body class="bg-slate-950 text-slate-50 p-8">
    <header>
888888 888888     88888                                               
8    8 8    8     8     eeeeeee e   e e     eeeee eeeee eeeee eeeee 
8e   8 8eeee8ee   8eeee 8  8  8 8   8 8     8   8   8   8  88 8   8 
88   8 88     8   88    8e 8  8 8e  8 8e    8eee8   8e  8   8 8eee8e
88   8 88     8   88    88 8  8 88  8 88    88  8   88  8   8 88   8
88eee8 88eeeee8   88eee 88 8  8 88ee8 88eee 88  8   88  8eee8 88   8
    </header>
    <main>
      <div>DB Emulator | Version: 1.1</div>
      <div>
        ---------------------------------<br>
        | Created by A.C. Wright © 2024 |<br>
        ---------------------------------
      </div>
      <div id="links">
        <a href="https://github.com/acwright/6502">Home Page</a>
        <span>|</span>
        <a href="https://6502.acwrightdesign.com">Documentation</a>
      </div>
    </main>
  </body>
</html>
)=====";

#endif
