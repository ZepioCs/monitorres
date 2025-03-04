# monitorres

A Node.js native addon for managing monitor resolutions on Windows systems.

## Features

- Get the current screen resolution of the primary display
- Set the resolution for all screens
- Get information about all connected monitors
- Set the resolution for a specific monitor
- Get the current resolution of a specific monitor
- Get all available resolutions for a specific monitor
- Get the system DPI settings

## Changelog

### Version 1.0.2

- Fixed issue with resolution and refresh rate settings not being properly applied

### Version 1.0.1

- Added validation to ensure requested resolution and refresh rate combinations are supported
- Improved handling of undefined refresh rate parameter (now defaults to current refresh rate)
- Enhanced error reporting with more detailed messages

### Version 1.0.0

- Initial release

## Installation

```bash
# Using npm
npm install monitorres

# Using bun
bun add monitorres
```

## Usage

```javascript
const monitorres = require("monitorres");

// Get the current screen resolution
const resolution = monitorres.getScreenResolution();
console.log("Current resolution:", resolution);
// Output: { width: 1920, height: 1080, refreshRate: 60, bitsPerPixel: 32 }

// Get all connected monitors
const monitors = monitorres.getAllMonitors();
console.log("Connected monitors:", monitors);
// Output: Array of monitor objects with details

// Get available resolutions for the first monitor
if (monitors.length > 0) {
  const availableResolutions = monitorres.getAvailableResolutions(
    monitors[0].id
  );
  console.log("Available resolutions:", availableResolutions);
  // Output: Array of resolution objects with width, height, refreshRate, and bitsPerPixel
}

// Set resolution for the first monitor
if (monitors.length > 0) {
  const result = monitorres.setMonitorResolution(
    monitors[0].id,
    1920,
    1080,
    60
  );
  console.log("Resolution change result:", result);
  // Output: true if successful, or error object with details if failed
}

// Get system DPI settings
const dpi = monitorres.getSystemDPI();
console.log("System DPI:", dpi);
// Output: { x: 96, y: 96 }
```

## API

### getScreenResolution()

Get the current screen resolution of the primary display.

**Returns**: `Object` - Object containing width, height, refreshRate, and bitsPerPixel

### setAllScreenResolutions(width, height, [refreshRate])

Set the resolution for all screens.

**Parameters**:

- `width` (number): The width in pixels
- `height` (number): The height in pixels
- `refreshRate` (number, optional): The refresh rate in Hz (defaults to current refresh rate if not specified)

**Returns**: `boolean|Object` - True if successful, or error object with details if failed

**Note**: The function validates that the requested resolution and refresh rate combination is supported before applying it.

### getAllMonitors()

Get information about all connected monitors.

**Returns**: `Array` - Array of monitor objects with details

### setMonitorResolution(monitorId, width, height, [refreshRate])

Set the resolution for a specific monitor.

**Parameters**:

- `monitorId` (string): The monitor ID (from getAllMonitors)
- `width` (number): The width in pixels
- `height` (number): The height in pixels
- `refreshRate` (number, optional): The refresh rate in Hz (defaults to current refresh rate if not specified)

**Returns**: `boolean|Object` - True if successful, or error object with details if failed

**Note**: The function validates that the requested resolution and refresh rate combination is supported before applying it.

### getMonitorResolution(monitorId)

Get the current resolution of a specific monitor.

**Parameters**:

- `monitorId` (string): The monitor ID (from getAllMonitors)

**Returns**: `Object` - Object containing width, height, refreshRate, and bitsPerPixel

### getAvailableResolutions(monitorId)

Get all available resolutions for a specific monitor.

**Parameters**:

- `monitorId` (string): The monitor ID (from getAllMonitors)

**Returns**: `Array` - Array of resolution objects with width, height, refreshRate, and bitsPerPixel

### getSystemDPI()

Get the system DPI settings.

**Returns**: `Object` - Object containing x and y DPI values

## Building from Source

To build this module from source, you need:

1. Node.js development environment
2. Windows build tools (Visual Studio or Build Tools for Visual Studio)
3. node-gyp installed globally

```bash
# Clone the repository
git clone <repository-url>
cd monitorres

# Install dependencies
npm install

# Build the module
npm run build
# or
node-gyp rebuild
```

## License

ISC

## Platform Support

This module is designed for Windows systems only.
