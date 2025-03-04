#include <napi.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <algorithm>

// Helper function to get detailed error information
Napi::Value GetDetailedError(Napi::Env env, long errorCode)
{
    Napi::Object error = Napi::Object::New(env);
    error.Set("code", Napi::Number::New(env, errorCode));

    switch (errorCode)
    {
    case DISP_CHANGE_SUCCESSFUL:
        error.Set("message", Napi::String::New(env, "The display settings change was successful"));
        break;
    case DISP_CHANGE_BADDUALVIEW:
        error.Set("message", Napi::String::New(env, "The settings change was unsuccessful because the system is DualView capable"));
        break;
    case DISP_CHANGE_BADFLAGS:
        error.Set("message", Napi::String::New(env, "An invalid set of flags was passed"));
        break;
    case DISP_CHANGE_BADMODE:
        error.Set("message", Napi::String::New(env, "The graphics mode is not supported"));
        break;
    case DISP_CHANGE_BADPARAM:
        error.Set("message", Napi::String::New(env, "An invalid parameter was passed"));
        break;
    case DISP_CHANGE_FAILED:
        error.Set("message", Napi::String::New(env, "The display driver failed the specified graphics mode"));
        break;
    case DISP_CHANGE_NOTUPDATED:
        error.Set("message", Napi::String::New(env, "Unable to write settings to the registry"));
        break;
    case DISP_CHANGE_RESTART:
        error.Set("message", Napi::String::New(env, "The computer must be restarted for the graphics mode to work"));
        break;
    default:
        error.Set("message", Napi::String::New(env, "Unknown error"));
    }

    return error;
}

// Get the current screen resolution
Napi::Value GetScreenResolution(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        DEVMODE devMode;
        ZeroMemory(&devMode, sizeof(DEVMODE));
        devMode.dmSize = sizeof(DEVMODE);

        // Get current screen settings
        if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode))
        {
            Napi::Error::New(env, "Failed to get display settings").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::Object result = Napi::Object::New(env);
        result.Set("width", Napi::Number::New(env, devMode.dmPelsWidth));
        result.Set("height", Napi::Number::New(env, devMode.dmPelsHeight));
        result.Set("refreshRate", Napi::Number::New(env, devMode.dmDisplayFrequency));
        result.Set("bitsPerPixel", Napi::Number::New(env, devMode.dmBitsPerPel));

        return result;
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// Get the resolution of a specific monitor
Napi::Value GetMonitorResolution(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "Wrong number of arguments. Expected monitor ID").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "Monitor ID must be a string").ThrowAsJavaScriptException();
            return env.Null();
        }

        std::string id = info[0].As<Napi::String>().Utf8Value();

        DEVMODE devMode;
        ZeroMemory(&devMode, sizeof(DEVMODE));
        devMode.dmSize = sizeof(DEVMODE);

        // Get current screen settings for the specified monitor
        if (!EnumDisplaySettings(id.c_str(), ENUM_CURRENT_SETTINGS, &devMode))
        {
            Napi::Error::New(env, "Failed to get display settings for the specified monitor").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::Object result = Napi::Object::New(env);
        result.Set("width", Napi::Number::New(env, devMode.dmPelsWidth));
        result.Set("height", Napi::Number::New(env, devMode.dmPelsHeight));
        result.Set("refreshRate", Napi::Number::New(env, devMode.dmDisplayFrequency));
        result.Set("bitsPerPixel", Napi::Number::New(env, devMode.dmBitsPerPel));

        return result;
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// Set the resolution for all screens
Napi::Value SetAllScreenResolutions(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        if (info.Length() < 2)
        {
            Napi::TypeError::New(env, "Wrong number of arguments. Expected width and height").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsNumber() || !info[1].IsNumber())
        {
            Napi::TypeError::New(env, "Width and height must be numbers").ThrowAsJavaScriptException();
            return env.Null();
        }

        int width = info[0].As<Napi::Number>().Int32Value();
        int height = info[1].As<Napi::Number>().Int32Value();

        // First get the current settings to preserve other values
        DEVMODE currentDevMode;
        ZeroMemory(&currentDevMode, sizeof(DEVMODE));
        currentDevMode.dmSize = sizeof(DEVMODE);

        if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &currentDevMode))
        {
            Napi::Error::New(env, "Failed to get current display settings").ThrowAsJavaScriptException();
            return env.Null();
        }

        // Default to current refresh rate if not specified
        int refreshRate = currentDevMode.dmDisplayFrequency;
        bool customRefreshRate = false;

        // Override with user-specified refresh rate if provided
        if (info.Length() >= 3 && !info[2].IsUndefined() && info[2].IsNumber())
        {
            refreshRate = info[2].As<Napi::Number>().Int32Value();
            customRefreshRate = true;
        }

        // Validate that the requested mode is supported
        bool isModeSupported = false;
        bool isResolutionSupported = false;
        int closestRefreshRate = 0;
        std::vector<int> availableRefreshRates;

        DEVMODE testMode;
        ZeroMemory(&testMode, sizeof(DEVMODE));
        testMode.dmSize = sizeof(DEVMODE);

        int modeIndex = 0;
        while (EnumDisplaySettings(NULL, modeIndex++, &testMode))
        {
            // Check if the resolution matches
            if (testMode.dmPelsWidth == width && testMode.dmPelsHeight == height)
            {
                isResolutionSupported = true;
                availableRefreshRates.push_back(testMode.dmDisplayFrequency);

                // Check if the refresh rate matches exactly
                if (testMode.dmDisplayFrequency == refreshRate)
                {
                    isModeSupported = true;
                    break;
                }

                // Keep track of the closest refresh rate
                if (closestRefreshRate == 0 ||
                    abs(static_cast<int>(testMode.dmDisplayFrequency) - static_cast<int>(refreshRate)) <
                        abs(static_cast<int>(closestRefreshRate) - static_cast<int>(refreshRate)))
                {
                    closestRefreshRate = testMode.dmDisplayFrequency;
                }
            }
        }

        // If custom refresh rate was specified but not supported, try with the closest available refresh rate
        if (customRefreshRate && !isModeSupported && isResolutionSupported)
        {
            // Create new settings based on current ones with the closest refresh rate
            DEVMODE newDevMode = currentDevMode;
            newDevMode.dmPelsWidth = width;
            newDevMode.dmPelsHeight = height;
            newDevMode.dmDisplayFrequency = closestRefreshRate;
            newDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

            // Sort refresh rates for better error message
            std::sort(availableRefreshRates.begin(), availableRefreshRates.end());

            // Create a string of available refresh rates
            std::string availableRatesStr = "";
            for (size_t i = 0; i < availableRefreshRates.size(); i++)
            {
                availableRatesStr += std::to_string(availableRefreshRates[i]);
                if (i < availableRefreshRates.size() - 1)
                    availableRatesStr += ", ";
            }

            // Apply the settings with the closest refresh rate
            long result = ChangeDisplaySettings(&newDevMode, 0);

            if (result != DISP_CHANGE_SUCCESSFUL)
            {
                Napi::Object error = Napi::Object::New(env);
                error.Set("code", Napi::Number::New(env, result));
                error.Set("message", Napi::String::New(env,
                                                       "The requested refresh rate (" + std::to_string(refreshRate) + "Hz) is not supported for resolution " +
                                                           std::to_string(width) + "x" + std::to_string(height) + ". " +
                                                           "Available refresh rates: " + availableRatesStr + ". " +
                                                           "Attempted to use closest rate (" + std::to_string(closestRefreshRate) + "Hz) but failed."));
                return error;
            }

            // Success with closest refresh rate
            Napi::Object result_obj = Napi::Object::New(env);
            result_obj.Set("success", Napi::Boolean::New(env, true));
            result_obj.Set("message", Napi::String::New(env,
                                                        "Used closest available refresh rate: " + std::to_string(closestRefreshRate) + "Hz instead of requested " +
                                                            std::to_string(refreshRate) + "Hz. Available rates: " + availableRatesStr));
            result_obj.Set("actualRefreshRate", Napi::Number::New(env, closestRefreshRate));
            return result_obj;
        }

        if (!isResolutionSupported)
        {
            Napi::Object error = Napi::Object::New(env);
            error.Set("code", Napi::Number::New(env, DISP_CHANGE_BADMODE));
            error.Set("message", Napi::String::New(env,
                                                   "The requested resolution is not supported. "
                                                   "Width: " +
                                                       std::to_string(width) +
                                                       ", Height: " + std::to_string(height)));
            return error;
        }

        if (!isModeSupported && customRefreshRate)
        {
            Napi::Object error = Napi::Object::New(env);
            error.Set("code", Napi::Number::New(env, DISP_CHANGE_BADMODE));
            error.Set("message", Napi::String::New(env,
                                                   "The requested refresh rate is not supported for this resolution. "
                                                   "Width: " +
                                                       std::to_string(width) +
                                                       ", Height: " + std::to_string(height) +
                                                       ", Refresh Rate: " + std::to_string(refreshRate)));
            return error;
        }

        // Create new settings based on current ones
        DEVMODE newDevMode = currentDevMode;
        newDevMode.dmPelsWidth = width;
        newDevMode.dmPelsHeight = height;
        newDevMode.dmDisplayFrequency = refreshRate;
        newDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

        long result = ChangeDisplaySettings(&newDevMode, 0);

        if (result != DISP_CHANGE_SUCCESSFUL)
        {
            return GetDetailedError(env, result);
        }

        return Napi::Boolean::New(env, true);
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// Get information about all connected monitors
Napi::Value GetAllMonitors(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        Napi::Array monitors = Napi::Array::New(env);

        // First, enumerate all display devices
        DISPLAY_DEVICE displayDevice;
        ZeroMemory(&displayDevice, sizeof(DISPLAY_DEVICE));
        displayDevice.cb = sizeof(DISPLAY_DEVICE);

        int monitorIndex = 0;
        for (DWORD i = 0; EnumDisplayDevices(NULL, i, &displayDevice, 0); i++)
        {
            // Only include active devices
            if (displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE)
            {
                Napi::Object monitor = Napi::Object::New(env);
                monitor.Set("id", Napi::String::New(env, displayDevice.DeviceName));
                monitor.Set("name", Napi::String::New(env, displayDevice.DeviceString));
                monitor.Set("deviceId", Napi::String::New(env, displayDevice.DeviceID));
                monitor.Set("deviceKey", Napi::String::New(env, displayDevice.DeviceKey));
                monitor.Set("stateFlags", Napi::Number::New(env, displayDevice.StateFlags));
                monitor.Set("attachedToDesktop", Napi::Boolean::New(env, displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP));
                monitor.Set("primaryDevice", Napi::Boolean::New(env, displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE));

                // Get current settings for this device
                DEVMODE devMode;
                ZeroMemory(&devMode, sizeof(DEVMODE));
                devMode.dmSize = sizeof(DEVMODE);

                if (EnumDisplaySettings(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &devMode))
                {
                    Napi::Object settings = Napi::Object::New(env);
                    settings.Set("width", Napi::Number::New(env, devMode.dmPelsWidth));
                    settings.Set("height", Napi::Number::New(env, devMode.dmPelsHeight));
                    settings.Set("refreshRate", Napi::Number::New(env, devMode.dmDisplayFrequency));
                    settings.Set("bitsPerPixel", Napi::Number::New(env, devMode.dmBitsPerPel));
                    settings.Set("orientation", Napi::Number::New(env, devMode.dmDisplayOrientation));

                    // Get position information
                    Napi::Object position = Napi::Object::New(env);
                    position.Set("x", Napi::Number::New(env, devMode.dmPosition.x));
                    position.Set("y", Napi::Number::New(env, devMode.dmPosition.y));
                    settings.Set("position", position);

                    monitor.Set("currentSettings", settings);
                }

                monitors.Set(monitorIndex++, monitor);
            }
        }

        return monitors;
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// Set the resolution for a specific monitor
Napi::Value SetMonitorResolution(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        if (info.Length() < 3)
        {
            Napi::TypeError::New(env, "Wrong number of arguments. Expected monitor ID, width, and height").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber())
        {
            Napi::TypeError::New(env, "Invalid argument types. Expected string, number, number").ThrowAsJavaScriptException();
            return env.Null();
        }

        std::string id = info[0].As<Napi::String>().Utf8Value();
        int width = info[1].As<Napi::Number>().Int32Value();
        int height = info[2].As<Napi::Number>().Int32Value();

        // First get the current settings to preserve other values
        DEVMODE currentDevMode;
        ZeroMemory(&currentDevMode, sizeof(DEVMODE));
        currentDevMode.dmSize = sizeof(DEVMODE);

        if (!EnumDisplaySettings(id.c_str(), ENUM_CURRENT_SETTINGS, &currentDevMode))
        {
            Napi::Error::New(env, "Failed to get current display settings").ThrowAsJavaScriptException();
            return env.Null();
        }

        // Default to current refresh rate if not specified
        int refreshRate = currentDevMode.dmDisplayFrequency;
        bool customRefreshRate = false;

        // Override with user-specified refresh rate if provided
        if (info.Length() >= 4 && !info[3].IsUndefined() && info[3].IsNumber())
        {
            refreshRate = info[3].As<Napi::Number>().Int32Value();
            customRefreshRate = true;
        }

        // Create new settings based on current ones
        DEVMODE newDevMode = currentDevMode;
        newDevMode.dmPelsWidth = width;
        newDevMode.dmPelsHeight = height;
        newDevMode.dmDisplayFrequency = refreshRate;
        newDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

        // Validate that the requested mode is supported
        bool isModeSupported = false;
        bool isResolutionSupported = false;
        int closestRefreshRate = 0;
        std::vector<int> availableRefreshRates;

        DEVMODE testMode;
        ZeroMemory(&testMode, sizeof(DEVMODE));
        testMode.dmSize = sizeof(DEVMODE);

        int modeIndex = 0;
        while (EnumDisplaySettings(id.c_str(), modeIndex++, &testMode))
        {
            // Check if the resolution matches
            if (testMode.dmPelsWidth == width && testMode.dmPelsHeight == height)
            {
                isResolutionSupported = true;
                availableRefreshRates.push_back(testMode.dmDisplayFrequency);

                // Check if the refresh rate matches exactly
                if (testMode.dmDisplayFrequency == refreshRate)
                {
                    isModeSupported = true;
                    break;
                }

                // Keep track of the closest refresh rate
                if (closestRefreshRate == 0 ||
                    abs(static_cast<int>(testMode.dmDisplayFrequency) - static_cast<int>(refreshRate)) <
                        abs(static_cast<int>(closestRefreshRate) - static_cast<int>(refreshRate)))
                {
                    closestRefreshRate = testMode.dmDisplayFrequency;
                }
            }
        }

        // If custom refresh rate was specified but not supported, try with the closest available refresh rate
        if (customRefreshRate && !isModeSupported && isResolutionSupported)
        {
            // Use the closest refresh rate instead
            newDevMode.dmDisplayFrequency = closestRefreshRate;

            // Sort refresh rates for better error message
            std::sort(availableRefreshRates.begin(), availableRefreshRates.end());

            // Create a string of available refresh rates
            std::string availableRatesStr = "";
            for (size_t i = 0; i < availableRefreshRates.size(); i++)
            {
                availableRatesStr += std::to_string(availableRefreshRates[i]);
                if (i < availableRefreshRates.size() - 1)
                    availableRatesStr += ", ";
            }

            // Apply the settings with the closest refresh rate
            long result = ChangeDisplaySettingsEx(id.c_str(), &newDevMode, NULL, CDS_UPDATEREGISTRY, NULL);

            if (result != DISP_CHANGE_SUCCESSFUL)
            {
                Napi::Object error = Napi::Object::New(env);
                error.Set("code", Napi::Number::New(env, result));
                error.Set("message", Napi::String::New(env,
                                                       "The requested refresh rate (" + std::to_string(refreshRate) + "Hz) is not supported for resolution " +
                                                           std::to_string(width) + "x" + std::to_string(height) + ". " +
                                                           "Available refresh rates: " + availableRatesStr + ". " +
                                                           "Attempted to use closest rate (" + std::to_string(closestRefreshRate) + "Hz) but failed."));
                return error;
            }

            // Success with closest refresh rate
            Napi::Object result_obj = Napi::Object::New(env);
            result_obj.Set("success", Napi::Boolean::New(env, true));
            result_obj.Set("message", Napi::String::New(env,
                                                        "Used closest available refresh rate: " + std::to_string(closestRefreshRate) + "Hz instead of requested " +
                                                            std::to_string(refreshRate) + "Hz. Available rates: " + availableRatesStr));
            result_obj.Set("actualRefreshRate", Napi::Number::New(env, closestRefreshRate));
            return result_obj;
        }

        if (!isResolutionSupported)
        {
            Napi::Object error = Napi::Object::New(env);
            error.Set("code", Napi::Number::New(env, DISP_CHANGE_BADMODE));
            error.Set("message", Napi::String::New(env,
                                                   "The requested resolution is not supported. "
                                                   "Width: " +
                                                       std::to_string(width) +
                                                       ", Height: " + std::to_string(height)));
            return error;
        }

        if (!isModeSupported && customRefreshRate)
        {
            Napi::Object error = Napi::Object::New(env);
            error.Set("code", Napi::Number::New(env, DISP_CHANGE_BADMODE));
            error.Set("message", Napi::String::New(env,
                                                   "The requested refresh rate is not supported for this resolution. "
                                                   "Width: " +
                                                       std::to_string(width) +
                                                       ", Height: " + std::to_string(height) +
                                                       ", Refresh Rate: " + std::to_string(refreshRate)));
            return error;
        }

        // Apply the settings
        long result = ChangeDisplaySettingsEx(id.c_str(), &newDevMode, NULL, CDS_UPDATEREGISTRY, NULL);

        if (result != DISP_CHANGE_SUCCESSFUL)
        {
            return GetDetailedError(env, result);
        }

        return Napi::Boolean::New(env, true);
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// Get all available resolutions for a specific monitor
Napi::Value GetAvailableResolutions(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "Wrong number of arguments. Expected monitor ID").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "Monitor ID must be a string").ThrowAsJavaScriptException();
            return env.Null();
        }

        std::string id = info[0].As<Napi::String>().Utf8Value();
        Napi::Array resolutions = Napi::Array::New(env);

        DEVMODE devMode;
        ZeroMemory(&devMode, sizeof(DEVMODE));
        devMode.dmSize = sizeof(DEVMODE);

        int modeIndex = 0;
        int resIndex = 0;
        std::set<std::string> uniqueResolutions;                             // To avoid duplicates
        std::vector<std::tuple<int, int, int>> uniqueResolutionsWithRefresh; // width, height, refresh rate

        while (EnumDisplaySettings(id.c_str(), modeIndex++, &devMode))
        {
            std::string resolution = std::to_string(devMode.dmPelsWidth) + "x" + std::to_string(devMode.dmPelsHeight);
            std::tuple<int, int, int> resWithRefresh = std::make_tuple(
                devMode.dmPelsWidth,
                devMode.dmPelsHeight,
                devMode.dmDisplayFrequency);

            // Check if this exact resolution+refresh rate combination is already in our list
            bool isDuplicate = false;
            for (const auto &existing : uniqueResolutionsWithRefresh)
            {
                if (std::get<0>(existing) == devMode.dmPelsWidth &&
                    std::get<1>(existing) == devMode.dmPelsHeight &&
                    std::get<2>(existing) == devMode.dmDisplayFrequency)
                {
                    isDuplicate = true;
                    break;
                }
            }

            if (!isDuplicate)
            {
                Napi::Object resObj = Napi::Object::New(env);
                resObj.Set("width", Napi::Number::New(env, devMode.dmPelsWidth));
                resObj.Set("height", Napi::Number::New(env, devMode.dmPelsHeight));
                resObj.Set("refreshRate", Napi::Number::New(env, devMode.dmDisplayFrequency));
                resObj.Set("bitsPerPixel", Napi::Number::New(env, devMode.dmBitsPerPel));
                resolutions.Set(resIndex++, resObj);

                uniqueResolutionsWithRefresh.push_back(resWithRefresh);
            }
        }

        return resolutions;
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// Get system DPI settings
Napi::Value GetSystemDPI(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        HDC hdc = GetDC(NULL);
        if (hdc == NULL)
        {
            Napi::Error::New(env, "Failed to get device context").ThrowAsJavaScriptException();
            return env.Null();
        }

        int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
        int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);

        ReleaseDC(NULL, hdc);

        Napi::Object result = Napi::Object::New(env);
        result.Set("x", Napi::Number::New(env, dpiX));
        result.Set("y", Napi::Number::New(env, dpiY));

        return result;
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// Initialize the module
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(
        Napi::String::New(env, "getScreenResolution"),
        Napi::Function::New(env, GetScreenResolution));
    exports.Set(
        Napi::String::New(env, "setAllScreenResolutions"),
        Napi::Function::New(env, SetAllScreenResolutions));
    exports.Set(
        Napi::String::New(env, "getAllMonitors"),
        Napi::Function::New(env, GetAllMonitors));
    exports.Set(
        Napi::String::New(env, "setMonitorResolution"),
        Napi::Function::New(env, SetMonitorResolution));
    exports.Set(
        Napi::String::New(env, "getMonitorResolution"),
        Napi::Function::New(env, GetMonitorResolution));
    exports.Set(
        Napi::String::New(env, "getAvailableResolutions"),
        Napi::Function::New(env, GetAvailableResolutions));
    exports.Set(
        Napi::String::New(env, "getSystemDPI"),
        Napi::Function::New(env, GetSystemDPI));

    return exports;
}

NODE_API_MODULE(monitorres, Init)