<!doctype html>
<html lang="en-us">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no, minimal-ui">
  <title>FTE Engine</title>
  <style>
    html, body {
      background-color: #000000;
      color: #808080;
      height: 100%;
      width: 100%;
      margin: 0;
      padding: 0;
      overflow: hidden;
    }

    #status {
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      font-size: 20px;
      color: #ffffff;
      visibility: hidden;
    }

    canvas.emscripten {
      border: 0;
      display: block;
      width: 100%;
      height: 100%;
    }
  </style>
</head>
<body>
  <div id="status">Loading...</div>
  <canvas class="emscripten" id="canvas" oncontextmenu="event.preventDefault()" tabindex="0"></canvas>

  <script type="text/javascript">
    if ("serviceWorker" in navigator) {
      navigator.serviceWorker.register("fte_pwa_sw.js", { scope: "./" })
        .catch((error) => console.error(`Service worker registration failed: ${error}`));
    }

    function str2ab(str) {
      var buf = new ArrayBuffer(str.length);
      var bufView = new Uint8Array(buf);
      for (var i = 0, strLen = str.length; i < strLen; i++) {
        bufView[i] = str.charCodeAt(i);
      }
      return buf;
    }

    // Array of predefined files to load (no dependency on filelist.json)
    const predefinedFiles = {
      "maps": [
        "map1.map",
        "map2.map",
        "level1.map",
        "ambient.lit",
        "lights.lit"
      ],
      "textures": [
        "texture1.png",
        "texture2.png"
      ]
    };

    var Module = {
      files: {},
      autostart: true,
      print: function(msg) { console.log(msg); },
      printErr: function(text) { console.error(text); },
      canvas: document.getElementById('canvas'),
      setStatus: function(text) {
        var statusElement = document.getElementById('status');
        if (text) {
          statusElement.innerHTML = text;
          statusElement.style.visibility = 'visible';
        } else {
          statusElement.style.visibility = 'hidden';
        }
      },
      postRun: [
        function() {
          console.log("Engine initialized successfully.");
          Module.setStatus("");
        }
      ]
    };

    async function loadFiles() {
      Module.setStatus('Loading files...');

      for (const [folder, files] of Object.entries(predefinedFiles)) {
        for (const file of files) {
          const path = `${folder}/${file}`;
          try {
            const response = await fetch(path);
            if (!response.ok) throw new Error(`Failed to load ${path}`);

            const arrayBuffer = await response.arrayBuffer();
            Module.files[path] = new Uint8Array(arrayBuffer);
            console.log(`Loaded: ${path}`);
          } catch (error) {
            console.error(`Error loading file ${path}:`, error);
          }
        }
      }

      begin();
    }

    function adjustCanvasSize() {
      var canvas = document.getElementById('canvas');
      var width = window.innerWidth;
      var height = window.innerHeight;

      canvas.width = width * window.devicePixelRatio;
      canvas.height = height * window.devicePixelRatio;
      canvas.style.width = width + 'px';
      canvas.style.height = height + 'px';
    }

    window.addEventListener('resize', adjustCanvasSize);
    adjustCanvasSize();

    function begin() {
      if (Module.began) return;
      Module.began = true;
      Module.setStatus('Downloading...');

      var script = document.createElement('script');
      script.setAttribute('src', "ftewebgl.js");
      script.setAttribute('type', "text/javascript");
      script.setAttribute('charset', "utf-8");
      script.addEventListener('error', function() {
        alert("Error downloading engine script.");
        Module.setStatus("Unable to download engine JavaScript");
      }, false);
      document.head.appendChild(script);
    }

    if (Module.autostart) {
      loadFiles();
    }
  </script>
</body>
</html>

