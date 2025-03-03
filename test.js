/**
 * Test script for monitorres
 * 
 * This script demonstrates the usage of all functions in the monitorres module.
 * It will temporarily change your screen resolution, so be prepared for that.
 */

const monitorres = require('./index.js');

console.log('=== monitorres Test Script ===');

// Test getScreenResolution
console.log('\n1. Testing getScreenResolution:');
const currentResolution = monitorres.getScreenResolution();
console.log('Current screen resolution:', currentResolution);

// Test getSystemDPI
console.log('\n2. Testing getSystemDPI:');
const dpi = monitorres.getSystemDPI();
console.log('System DPI:', dpi);

// Test getAllMonitors
console.log('\n3. Testing getAllMonitors:');
const monitors = monitorres.getAllMonitors();
console.log(`Found ${monitors.length} monitor(s):`);
monitors.forEach((monitor, index) => {
    console.log(`\nMonitor ${index + 1}:`);
    console.log(`  ID: ${monitor.id}`);
    console.log(`  Name: ${monitor.name}`);
    console.log(`  Primary: ${monitor.primaryDevice}`);
    if (monitor.currentSettings) {
        console.log(`  Current Settings:`);
        console.log(`    Resolution: ${monitor.currentSettings.width}x${monitor.currentSettings.height}`);
        console.log(`    Refresh Rate: ${monitor.currentSettings.refreshRate}Hz`);
        console.log(`    Bits Per Pixel: ${monitor.currentSettings.bitsPerPixel}`);
        console.log(`    Position: (${monitor.currentSettings.position.x}, ${monitor.currentSettings.position.y})`);
    }
});

// Only proceed with resolution changes if monitors were found
if (monitors.length > 0) {
    const primaryMonitor = monitors.find(m => m.primaryDevice) || monitors[0];
    const originalWidth = primaryMonitor.currentSettings.width;
    const originalHeight = primaryMonitor.currentSettings.height;
    const originalRefreshRate = primaryMonitor.currentSettings.refreshRate;
    
    // Test getMonitorResolution
    console.log('\n4. Testing getMonitorResolution:');
    const monitorResolution = monitorres.getMonitorResolution(primaryMonitor.id);
    console.log(`Resolution for monitor ${primaryMonitor.id}:`, monitorResolution);
    
    // Test getAvailableResolutions
    console.log('\n5. Testing getAvailableResolutions:');
    const availableResolutions = monitorres.getAvailableResolutions(primaryMonitor.id);
    console.log(`Available resolutions for monitor ${primaryMonitor.id}:`);
    availableResolutions.slice(0, 5).forEach((res, index) => {
        console.log(`  ${index + 1}. ${res.width}x${res.height} @ ${res.refreshRate}Hz (${res.bitsPerPixel} bits)`);
    });
    if (availableResolutions.length > 5) {
        console.log(`  ... and ${availableResolutions.length - 5} more`);
    }
    
    // Find a test resolution (slightly lower than current if possible)
    let testWidth = originalWidth;
    let testHeight = originalHeight;
    
    // Try to find a resolution that's different but not too different
    const testResolution = availableResolutions.find(res => 
        (res.width < originalWidth && res.width >= originalWidth * 0.8) || 
        (res.height < originalHeight && res.height >= originalHeight * 0.8)
    );
    
    if (testResolution) {
        testWidth = testResolution.width;
        testHeight = testResolution.height;
    }
    
    // Test setMonitorResolution
    console.log('\n6. Testing setMonitorResolution:');
    console.log(`Changing resolution of monitor ${primaryMonitor.id} to ${testWidth}x${testHeight}...`);
    const setResult = monitorres.setMonitorResolution(primaryMonitor.id, testWidth, testHeight);
    console.log('Result:', setResult);
    
    // Wait 3 seconds before changing back
    console.log('Waiting 3 seconds before changing back...');
    setTimeout(() => {
        console.log(`Changing resolution back to ${originalWidth}x${originalHeight}...`);
        const resetResult = monitorres.setMonitorResolution(
            primaryMonitor.id, 
            originalWidth, 
            originalHeight, 
            originalRefreshRate
        );
        console.log('Result:', resetResult);
        
        // Test setAllScreenResolutions (only if we have a single monitor to avoid issues)
        if (monitors.length === 1) {
            console.log('\n7. Testing setAllScreenResolutions:');
            console.log(`Changing all screen resolutions to ${testWidth}x${testHeight}...`);
            const setAllResult = monitorres.setAllScreenResolutions(testWidth, testHeight);
            console.log('Result:', setAllResult);
            
            // Wait 3 seconds before changing back
            console.log('Waiting 3 seconds before changing back...');
            setTimeout(() => {
                console.log(`Changing all screen resolutions back to ${originalWidth}x${originalHeight}...`);
                const resetAllResult = monitorres.setAllScreenResolutions(
                    originalWidth, 
                    originalHeight, 
                    originalRefreshRate
                );
                console.log('Result:', resetAllResult);
                console.log('\nTests completed!');
            }, 3000);
        } else {
            console.log('\nSkipping setAllScreenResolutions test with multiple monitors');
            console.log('\nTests completed!');
        }
    }, 3000);
} else {
    console.log('\nNo monitors found, skipping resolution tests');
    console.log('\nTests completed!');
}