# relativ-core

### Important ! ⚠ BEFORE LOOKING AT ANYTHING CHECK <a href="https://github.com/relativty/Relativ">Relativ</a>, the VR headset that relativ-core is designed for ⚠

# Getting Started ⚡️
## Installation
You can either install it using [Git](https://git-scm.com/) or direct [Download](https://github.com/relativty/relativ-core/archive/master.zip). Or from the <strong>command line</strong>:

```bash
# Clone this repository
$ git clone https://github.com/relativty/relativ-core
```


## Download OSVR
You can download the <strong>latest version</strong> [HERE](http://access.osvr.com/binary/osvr-runtime-installer)


## SteamVR Setup
<p>⚠ SteamVR must be installed on your computer ⚠<br />
Go into <b>relativ-core\SteamVR</b><br />
Take All the files and paste them in the <b><i>drivers</i></b> folder in your Steam directory <br />
(<b><i>PATH_TO_STEAM\Steam\steamapps\common\SteamVR\drivers</i></b>)</p>

<img src="/img/steamVR_folder.png">


## OSVR Setup
### 1. osvr_server_config.json
<p>Go into <b>relativ-core\OSVR\Configs</b><br />
  Drag and Drop or Copy the osvr_server_config.json into the <b><i>bin</i></b> folder of OSVR<br />
(<b><i>PATH_TO_OSVR\OSVR\Runtime\bin</i></b>)</p>

<img src="/img/osvr_server_config.json.png">

### 2. Displays
<p>Go into <b>relativ-core\OSVR\Displays</b><br />
Drag and Drop or Copy the files into the <b><i>displays</i></b>in the OSVR directory<br />
(<b><i>PATH_TO_OSVR\OSVR\Runtime\bin\displays</i></b>)</p>

<img src="/img/relativ_hmd.png">

### 3. Plugins
<p>Go into <b>relativ-core\OSVR\osvr-plugin-0</b><br />
Drag and Drop or Copy the files in the <b><i>osvr-plugin-0 folder</i></b>in the OSVR directory<br />
(<b><i>PATH_TO_OSVR\OSVR\Runtime\bin\osvr-plugin-0</i></b>)</p>

<img src="/img/plugins.png">


## Change the port of osvr_server_config.json
<p>Plug your arduino and check the port <br />
Copy the port’s name and paste it in the port variable of the <b><i>osvr_server_config.json</i></b> file and save it</p>

<img src="/img/port.png">


## Screen Configuration 💻
<p><b>You must have two screens (your main and the headset)</b><br />
⚠ The second screen must be at the right of the main screen and must be aligned with the top border of the main screen. ⚠</p>

<img src="/img/screen_order.png">

You can also change the resolution by changing the name of the display in the osvr_server_config file,

<img src="/img/displays.png">

or else, make a new resolution by duplicating **Relativ_HMD** and changing the height and width lines in the OSVR ***displays*** folder.

<img src="/img/resolution.png">


## Relativ and OSVR 🤖
<p>Now, you can power on your [Relativ Headset](https://github.com/relativty/Relativ)<br />
Start the <b><i>osvr_server.exe</i></b><br />
<strong><i> Path : FOLDER_WHERE_OSVR_IS\OSVR\bin\osvr_server.exe</i></strong></p>

<img src="/img/osvr_server.png">

 This window should appear :

<img src="/img/osvr_server_terminal.png">


## Start SteamVR
A little SteamVR Window should appear :

<img src="/img/steamVR_checkwindow.png">

<b>Have fun playing SteamVR games with your headset !</b>🚀

<img src="/img/steamVR_game.png">



## Thank you for using our product, let us know if there is any issue and feel free to contribute ! 🤝 Any feedback is deeply appreciated.
