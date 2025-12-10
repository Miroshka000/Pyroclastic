# Pyroclastic

> [!CAUTION]
> **Only supported on Windows x64!**

An alternative bootstrapper for Minecraft: Bedrock Edition on Windows.

## Features

- Easy to install & uninstall without replacing any game files.

- Directly launches Minecraft: Bedrock Edition (Release & Preview).

- Bypasses the [PC Bootstrapper provided by Gaming Runtime Services.](https://learn.microsoft.com/gaming/gdk/docs/gdk-dev/pc-dev/overviews/gr-pc-bootstrapper)

## Usage
- [Download](https://github.com/Aetopia/MCBE.GDK.Bootstrapper/releases/latest/download/gamelaunchhelper.dll) the latest release of MCBE GDK Bootstrapper.

- Run the following command(s) in PowerShell to find where the game is located:

    - Minecraft

        ```powershell
        & "$ENV:SystemRoot\explorer.exe" "$((Get-AppxPackage "Microsoft.MinecraftUWP").InstallLocation)"
        ```

    - Minecraft Preview

        ```powershell
        & "$ENV:SystemRoot\explorer.exe" "$((Get-AppxPackage "Microsoft.MinecraftWindowsBeta").InstallLocation)"
        ```   

- Place the dynamic link library in the opened folder & launch the game.

## Build
1. Install & update [MSYS2](https://www.msys2.org):

    ```bash
    pacman -Syu --noconfirm
    ```

3. Install [GCC](https://gcc.gnu.org) & [MinHook](https://github.com/TsudaKageyu/minhook):

    ```bash
    pacman -Syu mingw-w64-ucrt-x86_64-gcc --noconfirm
    ```


3. Start MSYS2's `UCRT64` environment & run `Build.cmd`.
