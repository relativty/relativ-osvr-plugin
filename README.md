# relativ-core

### Important ! ⚠ Before having a look at the repo you should check <a href="https://github.com/relativty/Relativ">Relativ</a>, the VR headset relativ-core is designed for ⚠

# Getting Started ⚡️
## How to Install
You can either install it using [Git](https://git-scm.com/) or direct [Download](https://github.com/relativty/relativ-core/archive/master.zip). Or from the <strong>command line</strong>:

```bash
# Clone this repository
$ git clone https://github.com/relativty/relativ-core
```


## Download OSVR
You can download the latest version (**build 377**) [HERE](http://access.osvr.com/binary/osvr-runtime-installer)


## SteamVR Setup
<p>⚠ SteamVR must be install on your computer ⚠<br />
Go into <b>relativ-core\SteamVR</b><br />
Take All the files and paste it in the <b><i>drivers</i></b> folder in your Steam folder <br />
(<b><i>PATH_TO_STEAM\Steam\steamapps\common\SteamVR\drivers</i></b>)</p>

<img src="/img/steamVR_folder.png">


## OSVR Setup
### 1. osvr_server_config.json
<p>Go into <b>relativ-core\OSVR\Configs</b><br />
  Drag and Drop the osvr_server_config.json in the <b><i>bin</i></b> folder of OSVR<br />
(<b><i>PATH_TO_OSVR\OSVR\Runtime\bin</i></b>)</p>

<img src="/img/osvr_server_config.json.png">

### 2. Displays
<p>Go into <b>relativ-core\OSVR\Displays</b><br />
Drag and Drop the files in the <b><i>displays</i></b> folder of OSVR<br />
(<b><i>PATH_TO_OSVR\OSVR\Runtime\bin\displays</i></b>)</p>

<img src="/img/relativ_hmd.png">

### 3. Plugins
<p>Go into <b>relativ-core\OSVR\osvr-plugin-0</b><br />
Drag and Drop the files in the <b><i>osvr-plugin-0 folder</i></b> of OSVR<br />
(<b><i>PATH_TO_OSVR\OSVR\Runtime\bin\osvr-plugin-0</i></b>)</p>

<img src="/img/plugins.png">


## Change the port of osvr_server_config.json
<p>Plug your arduino and check its port <br />
Copy the port’s name and paste it in port variable of the <b><i>osvr_server_config.json</i></b> file and save</p>

<img src="/img/port.png">


## Screen Configuration 💻
<p><b>You must have two screens (your main and the headset)</b><br />
⚠ The second screen must be on the right of the main screen and aligned with the top border of the main screen. ⚠</p>

<img src="/img/screen_order.png">

You can also change the resolution by changing the name of the display in osvr_server_config,

<img src="/img/displays.png">

or else, make a new resolution by duplicating a **Relativ_HMD** and changing the height and width lines in the OSVR ***displays*** folder.

<img src="/img/résolution.png">


## Relativ and OSVR 🤖
<p>Now, you can start your [Relativ Headset](https://github.com/relativty/Relativ)<br />
Start the <b><i>osvr_server.exe</i></b><br />
<strong><i> Path : FOLDER_WHERE_OSVR_IS\OSVR\bin\osvr_server.exe</i></strong></p>

<img src="/img/osvr_server.png">

A windows like this appear :

<img src="/img/osvr_server_terminal.png">


## Start SteamVR
A little SteamVR's window appear:

<img src="/img/steamVR_checkwindow.png">

<b>Finally, you can play with Relativ's headset on any SteamVR games !</b>🚀

<img src="/img/steamVR_game.png">
