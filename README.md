# MicaClient++
A simple client for Micasend written in C++
![MicaClient++ banner](./images/banner.jpg)<br>
Website : [https://mcpp.softplus.fr](https://mcpp.softplus.fr)
# Installation
## Required dependencies
To compile it you will need to first install the dependencies:<br>
- Ubuntu: `sudo apt install libcurl4-openssl-dev`
- Arch Linux: `sudo pacman -S curl`
<br><br>

## Compilation via CMake (recommanded)
- Ubuntu: `sudo apt install cmake`
- Arch Linux: `sudo pacman -S cmake`
Then you just need to run `./cmakecompile` in your local folder.
>- You can add --release argument to optimize the program (it will be a bit slower)<br>
>- You can also add --fastcompile to compile on multiple cores<br>
>- Finally, you can also add --reset to reset compilation data from previous compilations

# How to use it
## Setup
First, set settings like username, token... in `config.json`.<br>
## Basic usage
Then, you can run `build/MicaClient`, and now you should see the chat.<br>
<br>
To send a message: just type it, and press Enter !<br>
![MicaClient++ theme](./images/mcpp.jpg)<br>

## Command list
- `/help, /h` to print a message containing all the commands available.<br>
- `/exit, /q` to exit MicaClient++
- [`/r, /ru`](#reply-to-a-message) to reply to a message (ru for unsafe mode)
- [`/u`](#secure-unsecure-mode) to send a message in unsafe mode.
- [`/p`](#send-a-message-in-private-group) to send a message in a group
- [`/g`](#filter-messages-from-group) to change group vision (channel)
<br>

## Reply to a message
To reply to a message type `/r x` *(with x the message ID to reply)*.<br>
If you don't know it, only type `/r`, then choose the ID of the message you want to reply<br>
Then, press [ENTER], and you will be able to write and send your answer !

## Secure/Unsecure mode
By defalut, encryption message is active (you can see that messages are encrypted when there is an **S** on green background), but if you want to send a message in unsecure mode (message will be sent without encryption), you can type `/u Unsecure message`.
<br>
To reply in unsafe mode, type `/ru x` or `/ru`<br>
<br>
You can also disable it in config.json, but it is not recommanded.<br>
If you disable "enableEncryption" you won't be able to read Secured messages nor using private groups.<br>

## Private groups
You can create private groups that will only be readable by some users.
### Create a group
To create a group, in config.json just add a new object to discussionGroupKeys section like in the example below:<br>
```json
"discussionGroupKeys": {
    "a":{
        "users": [ "MagicTest", "Michel" ],
        "key":"thisIsAPrivateGroup"
    }
},
```
In this example the group is named "a" and its associated key is "thisIsAPrivateGroup".<br>
**Groups should not have spaces in their name !**
<br>
> **/!\\ Several users can create groups with the same name /!\\**<br>
As you might not have the same key, you will see encrypted message from users that are not in your group.<br>
> - So to be sure private message are coming from the right group you must **add** the users that belong to your group.
> - To receive messages from everyone in this group, add "*" in "users"<br>
<br>

### Filter messages from group
Then, just communicate the name and the key of the group you created to your members, and ask them to complete the "users" section too.<br>
<br>
To filter message by group type `/g groupname`, and to go back to general `/g`.<br>
Be careful, if you are in a group channel, and you send a message, it will only be sent to the group.
<br>
![Group filter in MC++](./images/groups.jpg)<br>

### Send a message in private group
To send a message in a private group, just type `/p groupname` or `/p` and then type the group name.
Then just type your message and press [ENTER].
If you reply to a message of a group, it will automatically reply in the group.

## Launch MicaClient with options
You can start MicaClient++ with arguments !
- `-m, --moderator` enables **moderator view** to get messages ID (no you won't have rights to delete messages :p)
- `--cfg path/to/a/config/file.json` to load other config (usefull when you have several accounts)
- `--all` to print all the message history contained in backup.json (overwrites the theme "maxMessages" setting)

## Settings in config.json 
There are many more settings available in config.json

### Theme and language
You can change MicaClient++ theme and language !
```json
// configure a special path for your configuration,
"mcppConfigPath":"./",
// themes/defaultTheme.json by default
"themeFile":"themes/oldMicasendTheme.json",
// languages/en.json by default
"languageFile":"languages/fr.json",
```
> You can contribute by adding translations and/or creating new themes ! It's only json files !

### Block users
```json
// to only see messages sent by verified users
"blockUnVerifiedUsers":false,
// to block specific users
"blockedUsers": [ "blockedPseudo", "blockedPseudo2" ],
```
You can also block users in discussion groups (for instance in public groups where someone tries to chat in your group with a wrong key)
```json
"public":{
    "users": [ "*" ],
    "blocked": [ "blockedPseudoInPublicGroup" ],
    "key":"thisIsaPublicGroup"
}
```

### Keep messages offline !
In config.json you have a section named "settings" :
```json
"settings": {
    // to memorise old messages that are not available on the server anymore
    "backupMessages":true,
}
```

## Settings in your themes !
If you are using Default theme (or a theme that supports it), you can modify some simple settings in your theme. For instance in Default theme, here are the settings :
```json
"settings": {
    "showMessageID":true,
    "showDate":false,
    "showDeletedMessages":true,
    "showOfflineMessages":true
},
// To print only a max quantity of messages, if you doesn't want to, put it to -1
"maxMessages":100,
```

---
# Futures features : 
- add config importer (upgrade compatibility version)
- add /game with megamorpion implementation (submenus with /showgame /play /addgame /delgame)
- add /adduser for admins (with /showuser /upuser)
- fix last message deleted → the message before appears twice
- add /jg || /joingroup to add group in your configuration
- add /leavegroup to remove group from your configuration
- add a command to delete message from memory (maybe /delmsg on offline/deleted message → /forget)
- Enable token protection by default (password that encrypt token)
- add signature (normal/usecure) ?