  ___ ___ ___ _  _   _____         _ _
 | _ ) __| _ \ \| | |_   _|__  ___| | |__  _____ __
 | _ \__ \   / .` |   | |/ _ \/ _ \ | '_ \/ _ \ \ /
 |___/___/_|_\_|\_|   |_|\___/\___/_|_.__/\___/_\_\

REMOVE QUARANTINE ATTRIBUTE
-------------------------------------------------------------------
macOS has malware protection with Gatekeeper, which blocks the execution
of apps from unsafe sources. Any unknown downloaded app will receive this
an quarantine flag. Also the collector app.
You can delete this flag as follows. First open Terminal. Goto the folder
with critterbase app and do this:

sudo xattr -r -d com.apple.quarantine critterbase.app

Or you can deactivate the whole mechanism. To change these settings,
choose Apple menu > System Settings, then click Privacy & Security in
the sideba, allow to start all apps.

RESET A PROBLEMATIC PYTHON (MODULES) INSTALLATION
-------------------------------------------------------------------
You can remove all installed poython modules this way:

pip3 freeze | xargs pip3 uninstall -y

INSTALLATION OF THE APP
-------------------------------------------------------------------
Move the app into the application folder.

INSTALLATION OF DEPENDENCIES / START
-------------------------------------------------------------------
Start via application folder or spotlight
and follow the instructions.