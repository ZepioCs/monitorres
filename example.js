/**
 * monitorres example
 * 
 * This example demonstrates how to use the monitorres library to get information
 * about connected monitors and their resolutions.
 */

const monitorres = require('./index.js');

// Get system DPI
const dpi = monitorres.getSystemDPI();
console.log('System DPI:', dpi);

// Get current screen resolution
const resolution = monitorres.getScreenResolution();
console.log('Current screen resolution:', resolution);

// Get all connected monitors
const monitors = monitorres.getAllMonitors();
console.log(`Found ${monitors.length} monitor(s):`);

// Display information about each monitor
monitors.forEach((monitor, index) => {
    console.log(`\nMonitor ${index + 1}:`);
    console.log(`  ID: ${monitor.id}`);
    console.log(`  Name: ${monitor.name}`);
    console.log(`  Primary: ${monitor.primaryDevice ? 'Yes' : 'No'}`);
    
    if (monitor.currentSettings) {
        console.log(`  Current Settings:`);
        console.log(`    Resolution: ${monitor.currentSettings.width}x${monitor.currentSettings.height}`);
        console.log(`    Refresh Rate: ${monitor.currentSettings.refreshRate}Hz`);
        console.log(`    Bits Per Pixel: ${monitor.currentSettings.bitsPerPixel}`);
        console.log(`    Position: (${monitor.currentSettings.position.x}, ${monitor.currentSettings.position.y})`);
    }
    
    // Get available resolutions for this monitor
    const availableResolutions = monitorres.getAvailableResolutions(monitor.id);
    console.log(`  Available Resolutions: ${availableResolutions.length} total`);
    
    // Display the first 5 available resolutions
    availableResolutions.slice(0, 5).forEach((res, i) => {
        console.log(`    ${i + 1}. ${res.width}x${res.height} @ ${res.refreshRate}Hz (${res.bitsPerPixel} bits)`);
    });
    
    if (availableResolutions.length > 5) {
        console.log(`    ... and ${availableResolutions.length - 5} more`);
    }
});

// Example of how to change resolution (commented out for safety)

if (monitors.length > 0) {
    const primaryMonitor = monitors.find(m => m.primaryDevice) || monitors[0];
    
    // Save original resolution
    const originalWidth = primaryMonitor.currentSettings.width;
    const originalHeight = primaryMonitor.currentSettings.height;
    const originalRefreshRate = primaryMonitor.currentSettings.refreshRate;
    
    // Change to a new resolution (e.g., 1280x720)
    console.log(`\nChanging resolution of monitor ${primaryMonitor.id} to 1280x720...`);
    const result = monitorres.setMonitorResolution(primaryMonitor.id, 1280, 720);
    console.log('Result:', result);
    
    // Wait 5 seconds before changing back
    setTimeout(() => {
        console.log(`Changing resolution back to ${originalWidth}x${originalHeight}...`);
        monitorres.setMonitorResolution(
            primaryMonitor.id, 
            originalWidth, 
            originalHeight, 
            originalRefreshRate
        );
    }, 5000);
}
