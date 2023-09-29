# MicaClient++
A simple client for Micasend written in C++
![MicaClient++ banner](./images/banner.jpg)<br>
# Installation
To compile it you will need to first install the dependencies:<br>
- Ubuntu: `sudo apt install g++ libcurl4-openssl-dev`
- Arch Linux: `sudo pacman -S curl`
<br>
Then you juste need to run `./compile` in your local folder.

# How to use it
## Setup
First, set settings like username, token... in `config.json`.<br>
## Basic usage
Then, you can run `./MicaClient`, and now you should see the chat.<br>
<br>
To send a message: just type it, and press Enter !<br>
## Secure/Unsecure mode
By defalut, encryption message is active (you can see that messages are encrypted when there is a **S** on green background), but if you want to send a message in unsecure mode (message will be sent without encryption), you can type `/u Unsecure message`.
You can also disable it in config.json, but it is not recommanded.<br>
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
> /!\\ Several users can create groups with the same name /!\\<br>
As you might not have the same key, you will see encrypted message from users that are not in your group.<br>
> - So to be sure private message are coming from the right group you must **add** the users that belong to your group.
> - To receive messages from everyone in this group, add "*" in "users"<br>


Then, just communicate the name and the key of the group you created to your members, and ask them to complete the "users" section too.<br>
### Send a message in private group
To send a message in a private group, just type `/p groupname Message only encrypted for group members`

## Command list
- `/help` to print a message containing all the commands available.<br>
- `/exit` to exit MicaClient++

## Arguments
You can start MicaClient++ with arguments !
- `--cfg path/to/a/config/file.json` to load other config (usefull when you have several accounts)