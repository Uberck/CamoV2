# Camo

Camo is a lightweight Windows desktop application written in C++ using the Win32 API. It features a simple, non-resizable window with a central button that launches a PowerShell script and Notepad. The app supports toggling between dark and light modes via the menu, and can terminate all running Notepad instances with a single click.

## Features

- Launches a PowerShell script and Notepad from the main window
- Central button toggles between "Start" and "Stop" actions
- Pins the app window to the bottom-right of the screen after starting
- Dark mode and light mode toggle via the menu
- Closes all Notepad windows when "Stop" is pressed
- Custom application icon

## Requirements

- Windows 10 or later
- Visual Studio 2022 (or compatible C++ compiler)
- PowerShell installed

## Usage

1. Build and run the application.
2. Click the main button to start the PowerShell script and Notepad.
3. Use the menu to toggle dark mode.
4. Click "Stop" to terminate the script and all Notepad windows.

## License

MIT License
