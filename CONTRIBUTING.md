# Contributing to M5 Crystal Firmware

Thank you for your interest in contributing! This document describes how to get started.

## Ways to contribute

- **Report bugs** — open an Issue with steps to reproduce, device model, and firmware version
- **Suggest features** — open an Issue or Discussion with a clear use case
- **Add a new function** — follow the guide below
- **Improve documentation** — fix errors, add examples, translate
- **Test on hardware** — report compatibility with devices not listed in the README

## Development setup

1. Clone the repository:

```bash
git clone https://github.com/Dlazder/m5_crystal_firmware.git
cd m5stick_crystal_firmware
```

2. Install [PlatformIO](https://platformio.org/) VS code extension
3. Open the project in VS Code — PlatformIO will install dependencies automatically
4. Select your device/port (through bottom panel or command palete) and build/upload firmware

See [build instructions](./docs/build.md) for full details and alternative methods.

## Adding a new feature

The firmware is designed to make adding new functions straightforward. Read the [how to add a feature](./docs/add_feature.md) before starting

## Develop guide

Read the [develop guide](docs/develop.md) to understand the basic structure of the firmware and its individual parts.

## Documentation

- [Build firmware and flash (all methods)](./docs/build.md)
- [Module connection guide (PN532, SD, IR)](./docs/modules.md)
- [Developer guide (far from complete)](./docs/develop.md)
- [How to add a new feature](./docs/add_feature.md)
- [Description of utilities](./docs/utils.md)

## Pull request checklist

- [ ] Tested on real hardware
- [ ] No hardcoded strings — use the language system
- [ ] UI uses existing utility wrappers, not raw M5 calls (except where necessary)
- [ ] No unnecessary includes or global variables
- [ ] PR description explains what the feature does and why

## Code style

- Follow the existing patterns — look at similar functions in `functions/`
- Keep function files focused: one feature per file
- Short, descriptive names — no abbreviations that aren't obvious
- Use tabs for indentation, not spaces

## Questions?

Open a [Discussion](https://github.com/Dlazder/m5stick_crystal_firmware/discussions) — questions are welcome.
