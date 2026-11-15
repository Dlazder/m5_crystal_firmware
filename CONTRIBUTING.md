# Contributing to M5 Crystal Firmware

Thank you for your interest in contributing! This document describes how to get started.

## Ways to contribute

- **Report bugs** - open an Issue with steps to reproduce, device model, and firmware version
- **Suggest features** - open an Issue or Discussion with a clear use case
- **Add a new function** - follow the guide below
- **Improve documentation** - fix errors, add examples, translate
- **Test on hardware** - report compatibility with devices not listed in the README

## Development workflow

All development happens in the `develop` branch, `main` is reserved for stable releases, so please do not open pull requests against it.

1. **Fork** the repository on GitHub: [Dlazder/m5_crystal_firmware](https://github.com/Dlazder/m5_crystal_firmware/fork)

2. **Clone your fork** and switch to `develop`:

```bash
git clone https://github.com/<your-username>/m5_crystal_firmware.git
cd m5_crystal_firmware
git checkout develop
```

3. **Add the upstream repository** so you can keep your `develop` up to date:

```bash
git remote add upstream https://github.com/Dlazder/m5_crystal_firmware.git
git fetch upstream
git merge upstream/develop
```

4. **Make your changes in `develop`** and commit them:

```bash
git add .
git commit -m "feat: short description of the change"
git push origin develop
```

If you are working on several changes at once, create a separate branch from `develop` (for example `feat/my-feature`) - the pull request still targets `develop`.

5. **Open a pull request** from `<your-username>:develop` into `Dlazder:develop`. Check that the base branch is `develop`, not `main`.

Before opening a pull request, sync your fork with upstream again to avoid merge conflicts. If the upstream `develop` has moved on, resolve conflicts in your fork and push the result.

## Development setup

1. Install the [PlatformIO](https://platformio.org/) extension for VS Code
2. Open the project folder in VS Code - PlatformIO will install dependencies automatically
3. Select your device/port (through the bottom panel or command palette) and build/upload the firmware

See [build instructions](./docs/build.md) for full details and alternative methods.

## Adding a new feature

The firmware is designed to make adding new functions straightforward. Read the [how to add a feature](./docs/add_feature.md) before starting

## Develop guide

Read the [develop guide](docs/develop.md) to understand the basic structure of the firmware and its individual parts.
This file is not finished, but perhaps you will find something useful there.

## Documentation

- [Build firmware and flash (all methods)](./docs/build.md)
- [Module connection guide (PN532, SD, IR)](./docs/modules.md)
- [Developer guide (far from complete)](./docs/develop.md)
- [How to add a new feature](./docs/add_feature.md)
- [Description of utilities](./docs/utils.md)

## Pull request checklist

- [ ] PR is opened from your fork's `develop` into `Dlazder:develop` (not `main`)
- [ ] Your `develop` is up to date with the upstream `develop` - this avoids merge conflicts and keeps the PR free of unrelated commits
- [ ] Tested on real hardware
- [ ] No hardcoded strings - use the language system
- [ ] UI uses existing utility wrappers, not raw M5 calls (except where necessary)
- [ ] No unnecessary includes or global variables
- [ ] PR description explains what the feature does and why

## Code style

- Follow the existing patterns - look at similar functions in `functions/`
- Keep function files focused: one feature per file
- Short, descriptive names - no abbreviations that aren't obvious
- Use tabs for indentation, not spaces

## Questions?

Open an [Issue](https://github.com/Dlazder/m5_crystal_firmware/issues) - questions are welcome.
