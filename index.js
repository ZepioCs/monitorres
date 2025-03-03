/**
 * monitorres - A Node.js native addon for managing monitor resolutions on Windows
 * 
 * This module provides functions to get and set monitor resolutions on Windows systems.
 * It allows you to query available resolutions, get information about connected monitors,
 * and change display settings.
 */

// Load the binary module
const binary = require('./build/Release/monitorres.node');

// Export the API with documentation
module.exports = {
  /**
   * Get the current screen resolution of the primary display
   * @returns {Object} Object containing width, height, refreshRate, and bitsPerPixel
   */
  getScreenResolution: binary.getScreenResolution,

  /**
   * Set the resolution for all screens
   * @param {number} width - The width in pixels
   * @param {number} height - The height in pixels
   * @param {number} [refreshRate=60] - The refresh rate in Hz (optional)
   * @returns {boolean|Object} True if successful, or error object with details if failed
   */
  setAllScreenResolutions: binary.setAllScreenResolutions,

  /**
   * Get information about all connected monitors
   * @returns {Array} Array of monitor objects with details
   */
  getAllMonitors: binary.getAllMonitors,

  /**
   * Set the resolution for a specific monitor
   * @param {string} monitorId - The monitor ID (from getAllMonitors)
   * @param {number} width - The width in pixels
   * @param {number} height - The height in pixels
   * @param {number} [refreshRate=60] - The refresh rate in Hz (optional)
   * @returns {boolean|Object} True if successful, or error object with details if failed
   */
  setMonitorResolution: binary.setMonitorResolution,

  /**
   * Get the current resolution of a specific monitor
   * @param {string} monitorId - The monitor ID (from getAllMonitors)
   * @returns {Object} Object containing width, height, refreshRate, and bitsPerPixel
   */
  getMonitorResolution: binary.getMonitorResolution,

  /**
   * Get all available resolutions for a specific monitor
   * @param {string} monitorId - The monitor ID (from getAllMonitors)
   * @returns {Array} Array of resolution objects with width, height, refreshRate, and bitsPerPixel
   */
  getAvailableResolutions: binary.getAvailableResolutions,

  /**
   * Get the system DPI settings
   * @returns {Object} Object containing x and y DPI values
   */
  getSystemDPI: binary.getSystemDPI
};