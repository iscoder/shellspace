# Shellspace

This is the source code repository for Shellspace, my entry into the Oculus VR Jam 2015.

<a href="http://www.youtube.com/watch?feature=player_embedded&v=_mL8QvhKvOA" target="_blank"><img src="http://img.youtube.com/vi/_mL8QvhKvOA/0.jpg" alt="YouTube" width="560" height="315" border="10" /></a>

http://challengepost.com/software/shellspace

It's a prototypical VR desktop environment, starting with a VNC client and growing from there.

# Compiling

After getting the source code, add a section for your computer to `GearVR/bin/dev.sh`, setting various environment variables to the paths where your Android SDK parts are installed.

# Running

To run, connect your phone using `adb`, change directory to `GearVR/ShellspaceApp/` and type `./build.sh && ./run.sh`.
