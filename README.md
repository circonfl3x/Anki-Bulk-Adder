# Anki-Bulk-Adder
A command line utility to easily add many anki cards at once.

# About
Anki-Bulk-Adder is a small project that is written in vanilla C with no calls to any external libraries apart from C's standard libs. It's only requirements is Anki and the AnkiConnect extension. The only problem right now is probably how windows defers from dealing with pretty output in the same way with *nix systems. 

# Usage
```./{executable-name} -f {filepath} [--json]```
##### --json tag will give you unformatted json output compared to the more readable output provided by the application

# Syntax
```front,back,name of deck```
that's all!
**Note:** after you add the name of a deck, the next lines don't need to have the deck name if you are planning to add to the same deck e.g:
    ```
    Bonjour,Hello,greetings
    Здравствуйте,Hello
    ```
In this case Bonjour and Здравствуйте will both be added to the deck 'greetings'

# Notes
This is just something I've put together in a few hours so it might be buggy and have a few memory related leaks and errors here and there (I will run it through asan later). It is also worth noting the input line buffer for now is limited to 1024 bytes (about a kilobyte or just 1023 characters) I will certainly implement one that allows much much more without having to have so much unused memory just floating in the stack. The return buffer (gotten from Anki's server) is also limited to 4096 bytes (about 4kb or 4095 characters), so this could be problematic for debug purposes if you want to add like 1000 cards at once. It is not the neatest code, the variable names aren't the most sensical and there are a lot of optimizations that can be made, since I was primarily programming this at like the middle of the night
