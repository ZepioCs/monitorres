/**
 * Type definitions for monitorres
 */

export enum DisplayOrientation {
    /** Landscape orientation (0 degrees) */
    LANDSCAPE = 0,
    /** Portrait orientation (90 degrees) */
    PORTRAIT = 1,
    /** Landscape orientation (180 degrees) */
    LANDSCAPE_FLIPPED = 2,
    /** Portrait orientation (270 degrees) */
    PORTRAIT_FLIPPED = 3
}


/**
 * Display change result codes
 */
export enum DisplayChangeResult {
    /** The display settings change was successful */
    SUCCESSFUL = 0,
    /** The settings change was unsuccessful because the system is DualView capable */
    BADDUALVIEW = 1,
    /** An invalid set of flags was passed */
    BADFLAGS = 2,
    /** The graphics mode is not supported */
    BADMODE = 3,
    /** An invalid parameter was passed */
    BADPARAM = 4,
    /** The display driver failed the specified graphics mode */
    FAILED = 5,
    /** Unable to write settings to the registry */
    NOTUPDATED = 6,
    /** The computer must be restarted for the graphics mode to work */
    RESTART = 7
}

  /**
 * State flags for display devices
 */
export enum DisplayStateFlags {
    /** The device is part of the desktop */
    ATTACHED_TO_DESKTOP = 0x1,
    /** The primary desktop is on the device */
    PRIMARY_DEVICE = 0x4,
    /** The device is mirroring another display */
    MIRRORING_DRIVER = 0x8,
    /** The device is VGA compatible */
    VGA_COMPATIBLE = 0x10,
    /** The device is removable; it cannot be the primary display */
    REMOVABLE = 0x20,
    /** The device has more display modes than its output devices support */
    MODESPRUNED = 0x8000000,
    /** The device is a remote network display */
    REMOTE = 0x4000000
}

/**
 * Resolution information
 */
export interface Resolution {
  /** Width in pixels */
  width: number;
  /** Height in pixels */
  height: number;
  /** Refresh rate in Hz */
  refreshRate: number;
  /** Bits per pixel */
  bitsPerPixel: number;
}

/**
 * Position information
 */
export interface Position {
  /** X coordinate */
  x: number;
  /** Y coordinate */
  y: number;
}

/**
 * Monitor settings
 */
export interface MonitorSettings {
  /** Width in pixels */
  width: number;
  /** Height in pixels */
  height: number;
  /** Refresh rate in Hz */
  refreshRate: number;
  /** Bits per pixel */
  bitsPerPixel: number;
  /** Orientation */
  orientation: DisplayOrientation;
  /** Position information */
  position: Position;
}

/**
 * Monitor information
 */
export interface Monitor {
  /** Monitor ID (used for other API calls) */
  id: string;
  /** Monitor name */
  name: string;
  /** Monitor device ID */
  deviceId: string;
  /** Monitor device key */
  deviceKey: string;
  /** State flags */
  stateFlags: DisplayStateFlags;
  /** Whether the monitor is attached to the desktop */
  attachedToDesktop: boolean;
  /** Whether this is the primary monitor */
  primaryDevice: boolean;
  /** Current monitor settings */
  currentSettings: MonitorSettings;
}

/**
 * DPI information
 */
export interface DPI {
  /** Horizontal DPI */
  x: number;
  /** Vertical DPI */
  y: number;
}

/**
 * Error information
 */
export interface ErrorInfo {
  /** Error code */
  code: DisplayChangeResult;
  /** Error message */
  message: string;
}

/**
 * Get the current screen resolution of the primary display
 * @returns Object containing width, height, refreshRate, and bitsPerPixel
 */
export function getScreenResolution(): Resolution;

/**
 * Set the resolution for all screens
 * @param width - The width in pixels
 * @param height - The height in pixels
 * @param refreshRate - The refresh rate in Hz (optional, default: 60)
 * @returns True if successful, or error object with details if failed
 */
export function setAllScreenResolutions(
  width: number,
  height: number,
  refreshRate?: number
): boolean | ErrorInfo;

/**
 * Get information about all connected monitors
 * @returns Array of monitor objects with details
 */
export function getAllMonitors(): Monitor[];

/**
 * Set the resolution for a specific monitor
 * @param monitorId - The monitor ID (from getAllMonitors)
 * @param width - The width in pixels
 * @param height - The height in pixels
 * @param refreshRate - The refresh rate in Hz (optional, default: 60)
 * @returns True if successful, or error object with details if failed
 */
export function setMonitorResolution(
  monitorId: string,
  width: number,
  height: number,
  refreshRate?: number
): boolean | ErrorInfo;

/**
 * Get the current resolution of a specific monitor
 * @param monitorId - The monitor ID (from getAllMonitors)
 * @returns Object containing width, height, refreshRate, and bitsPerPixel
 */
export function getMonitorResolution(monitorId: string): Resolution;

/**
 * Get all available resolutions for a specific monitor
 * @param monitorId - The monitor ID (from getAllMonitors)
 * @returns Array of resolution objects with width, height, refreshRate, and bitsPerPixel
 */
export function getAvailableResolutions(monitorId: string): Resolution[];

/**
 * Get the system DPI settings
 * @returns Object containing x and y DPI values
 */
export function getSystemDPI(): DPI; 