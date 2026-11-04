# We The People

## Readme (Seriously, read this.)

Welcome to **We The People** (WTP), a mod for *Sid Meier’s Civilization IV: Colonization*.

Detailed information is available on the [We The People forum at CivFanatics](https://forums.civfanatics.com/forums/civ4col-we-the-people.591/). We also recommend joining the [WTP Discord](https://forums.civfanatics.com/threads/we-the-people-discord-chat.665974/) for discussion and community support.

> This README presents the English instructions in GitHub-friendly Markdown. The original [HTML readme](readme.html) remains available, including its German and Russian sections.

## Table of contents

- [About We The People](#about-we-the-people)
- [WTP team members](#wtp-team-members)
- [How to install the mod](#how-to-install-the-mod)
  - [Install the game](#1-install-the-game)
  - [Install the mod manually](#2-install-the-mod-manually)
  - [Load the mod](#3-load-the-mod)
  - [Unload the mod](#unload-the-mod)
  - [Graphics on modern computers](#graphics-on-modern-computers)
  - [Create a desktop shortcut](#create-a-desktop-shortcut)
- [Multiplayer setup](#multiplayer-setup)
- [Common issues](#common-issues)
- [What is the mod about?](#what-is-the-mod-about)
- [German instructions](readme.html#InstructionGerman)
- [Russian instructions](readme.html#InstructionRussian)

## About We The People

### Foreword

“We The People” is one of the most extensive and complex mods for *Sid Meier’s Civilization IV: Colonization*. Whether it is the best is for each player to decide. There is a great deal of history behind WTP, and a small but passionate team continues to improve and expand it with help from the community.

### Origins

WTP is the successful continuation of **Religion and Revolution** (RaR), carrying on its tradition with a new team that includes former RaR members. WTP retains RaR’s features and adds more: improved graphics and AI, bug fixes, text corrections, balance changes, and performance improvements.

RaR was based on **The Authentic Colonization** (TAC), version 2.03_final. WTP would not exist without TAC, and the team is grateful for its contributions.

### Why the project name changed

The RaR core team had become too small, but members of the community and former team members did not want the mod to end. The team and its philosophy had changed enough that a new name was appropriate. Over time, former RaR team members returned and joined the new team.

### Conclusion

We are proud to be part of this community’s history and hope the mod will continue for a long time. We cannot list every supporter and modder, but we are grateful to everyone who has helped.

Have fun playing! Visit the [CivFanatics forum](https://forums.civfanatics.com/forums/civ4col-we-the-people.591/) from time to time.

## WTP team members

**Current team:** raystuttgart, Devolution, Nightinggale, Schmiddie, Mr.ZorG, Kendon, Raubwürger, Liet, Barthoze, sneed, jooe, FlaviusBelisarius, Fürstbischof, Tucker8, victorxucp, Aemon, Dyllin Black (Razonatair/DwarfMurdered), Monticello.

**Special thanks:**

- f1rpo — continual coding support
- karadoc — pathfinding code

**Former (now inactive) RaR members:** Robert Surcouf, colonialfan, KJ Jansson, Androrc the Orc, orlanth.

**Credits:** The TAC team and the many modders whose work has contributed to the project. Please let the team know if a contribution has not been credited.

Special thanks to Mod DB for hosting the mod download, and to other forums that report on the project. Please also read the in-game Colopedia credits for detailed information. If you reuse any part of TAC, RaR, or WTP, credit the people involved and, ideally, ask beforehand.

### Music credits

René Osmanczyk composed most of the soundtrack. Thanks also to Gregoire Lourme for “The Seven Seas Journey” and the *Cinematic Volume 7 Animation Movies* ([listen on Jamendo](https://www.jamendo.com/track/1119930/the-seven-seas-journey?language=ru)).

Additional music credits: Efisio Cross, Adrian Ziegler, Luke Richards, James Horner, Hans Zimmer, Greg Dombrowski, Michael Ortega, David Chappell, Kim Planert, Thomas Bergersen, Steven Jablonsk, Mattia Cupeli, Really Slow Motion & Giant Apes, R. Armando Morabito, Music for the Fearless, Lewis Capaldi, Eurielle, HDSoundi, Rajiw Seewollal, Abel Korzeniowski, RS Soundtracks, Brand X Music, Brunuh Ville, Arn Sanderson, Elias Nilsson, Patrick Doyle, Onur Tarcin, Pandora Journey, Zack Hemsey, Trevor Da Maere, and Dirk Maassen.

Credits and thanks also go to the original game’s creators and publishers, Firaxis and 2K Games.

## How to install the mod

### 1. Install the game

Install *Civilization IV: Colonization*. Depending on your version, you may need an additional step:

- **Steam:** switch to the **Original release** beta. See [Steam instructions](#steam-needs-the-original-unsupported-release).
- **GOG:** no extra steps.
- **DVD:** no extra steps.
- **CD:** install the [official 1.01f patch](https://forums.civfanatics.com/resources/civilization-iv-colonization-patch-v1-01f.11915/).

For Linux/Steam Deck, see the [Proton setup forum post](https://forums.civfanatics.com/threads/has-anyone-gotten-we-the-people-working-on-steam-deck.681177/#post-16419389).

### 2. Install the mod manually

These instructions use `WeThePeople` as the mod folder name. You can use another name, and install multiple versions, but a consistent name helps with [savegames](#savegame-issues) and [multiplayer](#multiplayer-setup).

1. Open the Colonization installation directory (the directory containing `Colonization.exe`).
2. Open or create the `Mods` folder. Capitalization does not matter on Windows, but it does on Linux; the folder may be named `MODS`.
3. Extract We The People into `Mods`.
4. Check that the result includes `Mods/WeThePeople/Assets`. Avoid an extra nested folder such as `Mods/WeThePeople/WeThePeople/Assets`.
5. Install the DLL files into the Colonization directory by either:
   - double-clicking `Mods/WeThePeople/setup.bat`, or
   - copying the contents of `Mods/WeThePeople/Copy to Colonization.exe` into the game directory (the one containing `Mods` and `Colonization.exe`).

### 3. Load the mod

1. Open Colonization.
2. Select **ADVANCED**.
3. Select **LOAD A MOD**.
4. Choose the mod and confirm the restart.

After the game starts with the mod loaded, it remembers that mod and loads it by default. This behavior is specific to WTP.

### Unload the mod

To stop Colonization from loading the mod by default, clear the mod setting:

1. Open `Documents/My Games`.
2. Find the Colonization folder. Common names include `Civ4Colonization` and `Civilization IV Colonization`.
3. Open `CivilizationIV.ini` (not `CivilizationIV.ini.bak`).
4. Set the mod line to `Mod = 0`.

Alternatively, delete the INI file to reset all settings to their defaults.

### Graphics on modern computers

The Civilization IV engine was released in 2005. It supports modern resolutions, but some setup may help:

- Enable all graphical details; even older integrated graphics should be sufficient.
- Parts of the interface use fixed pixel sizes, so popups can look tiny on 4K displays. **1920×1080** is recommended. For higher-resolution monitors, use an upscaler such as Radeon Super Resolution.
- The game was designed for 4:3 displays. Some screens may look unusual on ultrawide displays; improving them requires complete screen rewrites.
- At 3840×2160, a globe-map bug can prevent some map plots from being clicked. The affected code cannot be changed by mods, so avoid that in-game resolution.

### Create a desktop shortcut

The easiest option is to create a normal shortcut and let the mod set itself as the default. If you need to configure it manually, edit the shortcut target and append a space followed by `mod="WeThePeople"` (replace the name with the exact mod folder name). The original readme includes an example Windows path and additional details.

## Multiplayer setup

Every player must use an identical mod folder name; otherwise the Civ4 engine will not allow the players to join the same game.

The team works to reduce desynchronization, but network-safe code for Civ4 multiplayer is difficult and desyncs can still occur. If you encounter one, please report it on the [Discord](https://forums.civfanatics.com/threads/we-the-people-discord-chat.665974/) or [CivFanatics forum](https://forums.civfanatics.com/forums/civ4col-we-the-people.591/).

## Common issues

### Steam needs the original “unsupported” release

This applies only to the Steam version. A newer Colonization release broke mod support. In Steam, right-click Colonization, open **Properties → Betas**, and select `original_release_unsupported`. Ignore the “unsupported” label. This version matches the GOG version but does not include Steam-specific multiplayer.

See the [forum guide with screenshots](https://forums.civfanatics.com/threads/important-for-all-steam-users-follow-these-instructions-to-play-wtp-without-bugs.674675/).

### “More siblings than memory allocated”

This usually means `setup.bat` was not run. WTP needs files copied into the main Colonization directory. See [manual installation](#2-install-the-mod-manually).

### Savegame issues

A savegame requires the mod folder name to match the name stored in the save. You can change the stored name with the [Savegame Mod Renaming Tool](https://we-the-people-civ4col-mod.github.io/DeveloperNotes/SavegameModRenamingTool/index.html). The team tries to preserve compatibility between releases, but some changes are unavoidable to improve memory use or fix mechanics.

### Low memory warning

This mainly affects CD versions and possibly DVD versions; Steam and GOG versions are pre-patched. It is a legacy issue related to 32-bit Windows. The [4 GB patching tool](https://ntcore.com/?page_id=371) can be used to patch `Colonization.exe`. The original readme also links to Microsoft’s explanation of the PE large-address-aware flag.

### “Python Failed Initializing?”

This can happen when Python exceptions are not enabled in an INI file, hiding the actual error. The original readme’s [DLL troubleshooting guide](https://github.com/We-the-People-civ4col-mod/Mod/wiki/How-to-play-the-development-version) has more information.

### `CvGameCoreDLL.dll` not loaded

Check that `WeThePeople\Assets\CvGameCoreDLL.dll` exists. If it is missing from a release, report the issue; advanced users may need to compile the development version. See the [development version guide](https://github.com/We-the-People-civ4col-mod/Mod/wiki/How-to-play-the-development-version).

## What is the mod about?

WTP brings together many of the best ideas from the CivCol modding community and restores features from the original Colonization. Features are carefully adjusted and improved to offer more variety, surprises, and challenge across the economy, exploration, diplomacy, trade, events, and wars. The mod balances simulation and action while considering usability, AI, performance, and historical authenticity.

### Is using the work allowed?

In general, the source code, graphics, and images may be used, but please give proper credit and ask if you are unsure.

**We The People is a non-commercial mod project.** Use and distribution are allowed as long as WTP remains non-commercial and copyright is respected. If you use WTP in your own mod, say so and identify your changes to the original WTP mod.

### Can I support the project financially?

No. The team cannot accept donations; the project exists for the enjoyment of modding.

### Can I participate?

Yes. You can give feedback, report bugs, share ideas, or join the team and collaborate. For current plans and community discussion, see the [forum](https://forums.civfanatics.com/forums/civ4col-we-the-people.591/).

### A little advice

When playing WTP for the first time, read the Colopedia and visit the [forum](https://forums.civfanatics.com/forums/civ4col-we-the-people.591/). Ask questions if anything is unclear—the team and community are happy to help.

Have fun playing!

— Your WTP Team

---

[Deutsche Anleitung (German)](readme.html#InstructionGerman) · [Инструкция на русском (Russian)](readme.html#InstructionRussian) · [Original HTML readme](readme.html)
