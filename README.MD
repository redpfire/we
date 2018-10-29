# Motivation
I've recently bought the Wallpaper Engine and it bugged me that I can't do much with workshop contents. I then reverse engineered the pkg file and I've managed to develop somewhat stable tool to extract, list and repack the contents of a, so called, `PKGV0001` filesystem/package structure. I'm happy to accept feedback and pull requests/issues on this tool and to keep working on it if it gains some interest.
### Disclaimer
Any content that you download from the Steam Workshop belongs to it's respective owners. Do not by any means use this tool to redistribute any content that you do not own or do not have rights to redistribute in any way. I as the creator of the tool am not responsible for any illegal usage of this tool. Just, please, do what you think is right.
# Building
## Windows
*ugh* MinGW didn't do the job. If you have a way to build it, feel free to create an issue!<br>
~~You can also use WCL~~
## Linux
*Or Windows with WCL*<br>
libraries required:<br>
* libboost
* libstdc++
<br><br>Building it:<br>`$ make linux`
# Usage
![image](https://i.imgur.com/r3YyzZo.png)
Bring up workshop item and locate scene.pkg<br>
![image](https://i.imgur.com/h9fKjsO.png)
Drag and drop the file into tool directory (copy, not move)<br>
![image](https://i.imgur.com/XFdaLht.png)
Extract it!<br>
![image](https://i.imgur.com/0rsDzlD.png)
It should extract to a tmp folder. Don't touch it! Edit contents or do whatever you like inside of it.<br>
![image](https://i.imgur.com/aM7c4jz.png)
Inside are the standard files of a Wallpaper Engine project. You can paste them into a new project and edit them. Don't forget to place everything into the tmp folder after you finish!<br><br>
Now simply repack it! `./we r scene.pkg`<br>
![image](https://i.imgur.com/4lFS1K1.png)
Now there's the freshly repacked file!<br>
![image](https://i.imgur.com/r3YyzZo.png)
Copy all of the old files except scene.pkg into a freshly created project and paste in your repacked `scene.pkg`<br><br>
## MAC
Required:<br>
* Macport or Brew (for install libboost)
* Xcode and the Xcode Command Line Tools
* libboost
# Usage
1. install Xcode and the Xcode Command Line Tools

2. install libboost : `brew install boost`(When you installed Brew) or `sudo port install boost`(When you installed Macport)

2-1. If you don't have brew : `ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" < /dev/null 2> /dev/null`

2-2. If you don't have Macport : Install [Macport](https://www.macports.org/) and reboot.

3. `make linux`

4. `./we x scene.pkg`

5. `./we r scene.pkg`
## That's it!
Now you've successfully modified and repacked the workshop item. <br>You can now, after all of this, use it as a your wallpaper.<br>
Thanks for tuning in!
