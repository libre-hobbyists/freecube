# FreeCube
> *A free and open-source GameCube emulator built on principles of preservation, transparency, and community.*

## What *is* FreeCube?

freecube is a clean-room GameCube emulator developed entirely through legal reverse engineering and hardware observation. We're building this to preserve gaming history and ensure that beloved games remain playable for generations to come.

> [!NOTE]
> Currently, this project is **in very early developement.** This project is brand new and things are still being setup. We don't yet have an established roadmap, but do keep an eye out for one!

## Why FreeCube?

Gaming history deserves preservation. As official support for older consoles fades, as hardware degrades and replacements become harder to make, emulation becomes crucial for:

- *Preservation*: Keeping games playable as hardware ages and fails
- *Accessibility*: Making gaming history available to new generations
- *Research*: Enabling academic study of game design and technology
- *Enhancement*: Providing modern features like higher resolutions and save states

We believe preservation should be **free** (as in freedom), **legal**, and **transparent**.

---

## Legal & Ethical Commitment

**Clean Room Development**: FreeCube is built entirely through legitimate reverse engineering. We do not use leaked source code, proprietary SDKs, or any copyrighted materials from Nintendo or any of its related companies. Contributors to this project are expected to do the same when contributing code.

**Respecting Rights**: We do not condone or facilitate piracy. Users are responsible for legally obtaining their own game files from legitimately purchased copies. This repository will not ever direct users on where to locate or otherwise attain such content.  This also extends to confidential or otherwise proprietary source code from Nintendo or other GameCube publishers/developers. **Do not contribute source code that is still considered confidential!**

**Transparency**: All development happens in the open. Every commit, every decision, every design choice is documented and available for audit. All of our used sources are also cited where possible in a [markdown file](./docs/sources.md)

**License**: [GPLv3](./LICENSE) - ensuring this work remains free and open forever.

---

## Getting Started

### For Users

Unfortunately, FreeCube is not yet in a state that it can be used. As of 7 December, 2025; there is no **emulator** source code. No code, no emulator. This section will be updated as needed once this changes!

### For Developers

C++ developers, we need you! Whether you're experienced in emulator development or just passionate about preservation, there's room for you here. We need all the hands and eyes we can get on this project to make it happen.

**What we're looking for:**
- CPU emulation (PowerPC 750CL/"Gecko")
- Graphics (Flipper GPU, TEV pipeline)
- Audio subsystem
- Input handling
- Testing and documentation
- Build system and CI/CD

**Tech Stack:**
- **Language**: C++17 / C17 (C++ is preferred but C is accepted)
- **Build System**: CMake (at least 4.1.0)
- **Testing**: (TBD - suggestions welcome and encouraged!)

See [CONTRIBUTING.md](./.github/CONTRIBUTING.md) for detailed guidelines on how to get involved.

---

## Architecture

(Coming soon - we're still designing the core architecture!)

Planned components:
- **Core**: CPU interpreter/JIT, memory management
- **Graphics**: GPU emulation and rendering backend
- **Audio**: DSP emulation and audio output
- **Frontend**: User interface (platform TBD)
- **Debugging**: Developer tools for testing and reverse engineering

---

## Building from Source

Building from source can be a decently long process, check out the **[Building / Installing Documentation](./docs/build.md)** for all the info.

---

## Disclaimer

freecube is an independent project and is not affiliated with, endorsed by, or connected to Nintendo in any way. GameCube is a trademark of Nintendo. We respect Nintendo's intellectual property rights.

This software is provided for educational and preservation purposes. Users are responsible for ensuring their use complies with applicable laws in their jurisdiction.

---

> With <3 by [Libre Hobbyists](https://github.com/libre-hobbyists) and its many contributors.
