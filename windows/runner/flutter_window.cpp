#include <iostream>
#include <string>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.ApplicationModel.h>
#include <Shlobj.h>
#include <Shellapi.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Web::Http;
using namespace Windows::ApplicationModel;

class UpdateManager {
public:
    static IAsyncAction CheckForUpdate(const winrt::hstring& version);
    static IAsyncAction DownloadAndInstallUpdate();
    static void InstallUpdate(const std::wstring& downloadedFilePath);
    static void RestartApplication();
    static IAsyncAction ShowMessageDialog(const std::wstring& message);
};

IAsyncAction UpdateManager::CheckForUpdate(const winrt::hstring& version) {
    co_await resume_background();

    try {
        // Version number received from Flutter
        std::wstring flutterVersion = version.c_str();

        // URL to fetch the version number of the latest release
        std::wstring url = L"https://fetcher-in-4rmyxecbaa-el.a.run.app/tally/app_version";

        // Using HttpClient to make GET request to fetch the version number
        HttpClient httpClient;
        auto response = co_await httpClient.GetAsync(Uri(url));

        if (response.IsSuccessStatusCode()) {
            std::wstring jsonResponse = co_await response.Content().ReadAsStringAsync();
            
            // Parse JSON to extract minimum build number
            int minBuildNumber = ParseMinBuildNumberFromJson(jsonResponse);

            // Compare version number with the minimum build number
            if (minBuildNumber <= std::stoi(flutterVersion)) {
                co_return;
            }
            else {
                co_await DownloadAndInstallUpdate();
            }
        }
        else {
            // Failed to retrieve version number
            co_await ShowMessageDialog(L"Failed to retrieve version number. Status code: " + std::to_wstring(response.StatusCode()));
        }
    }
    catch (hresult_error const& ex) {
        // Error checking for update
        co_await ShowMessageDialog(L"Error checking for update: " + ex.message().c_str());
    }
}

int UpdateManager::ParseMinBuildNumberFromJson(const std::wstring& jsonResponse) {
    // Parse JSON string to extract minimum build number
    // Assuming jsonResponse is in the format {"data":{"tally_min_build_number":23}}
    try {
        // Parse JSON
        auto json = json::parse(jsonResponse);

        // Extract min build number from JSON
        int minBuildNumber = json["data"]["tally_min_build_number"];

        return minBuildNumber;
    }
    catch (const std::exception& e) {
        // Handle parsing error
        // You might want to log the error or handle it in some other way
        return -1; // Return an error code or default value
    }
}


IAsyncAction UpdateManager::DownloadAndInstallUpdate() {
    co_await resume_background();

    try {
        auto localFolder = ApplicationData::Current().LocalFolder();
        auto downloadedFilePath = localFolder.Path() + L"\\new_app.exe";

        HttpClient httpClient;
        auto response = co_await httpClient.GetAsync(Uri(L"https://firebasestorage.googleapis.com/v0/b/rootfi2022.appspot.com/o/Softwares%2Frootfi_integrate.exe?alt=media"));

        if (response.IsSuccessStatusCode()) {
            auto buffer = co_await response.Content().ReadAsBufferAsync();
            auto file = co_await localFolder.CreateFileAsync(L"new_app.exe", CreationCollisionOption::ReplaceExisting);
            co_await FileIO::WriteBufferAsync(file, buffer);

            // Install the downloaded update
            InstallUpdate(downloadedFilePath);

            // Restart the application after installation
            RestartApplication();
        }
        else {
            // Failed to download update
            co_await ShowMessageDialog(L"Failed to download update. Status code: " + std::to_wstring(response.StatusCode()));
        }
    }
    catch (hresult_error const& ex) {
        // Error downloading or installing update
        co_await ShowMessageDialog(L"Error downloading or installing update: " + ex.message().c_str());
    }
}

void UpdateManager::InstallUpdate(const std::wstring& downloadedFilePath) {
    // Run the installer silently
    SHELLEXECUTEINFO info = { sizeof(info) };
    info.lpFile = downloadedFilePath.c_str();
    info.lpVerb = L"runas"; // Run as administrator
    info.lpParameters = L"/S"; // Silent installation
    info.nShow = SW_HIDE; // Hide the installer window

    if (!ShellExecuteEx(&info)) {
        // Failed to start installer
        // Handle error
    }
}

void UpdateManager::RestartApplication() {
    // Get the path to the current executable
    wchar_t currentExecutablePath[MAX_PATH];
    GetModuleFileName(nullptr, currentExecutablePath, MAX_PATH);

    // Restart the application
    ShellExecute(nullptr, L"open", currentExecutablePath, nullptr, nullptr, SW_SHOWNORMAL);
}

IAsyncAction UpdateManager::ShowMessageDialog(const std::wstring& message) {
    MessageDialog dialog(message.c_str());
    co_await dialog.ShowAsync();
}

int main() {
    try {
        Application::Start([](auto&&) {
            UpdateManager::CheckForUpdate(L"1.0.0"); // Provide default version here
        });
    }
    catch (hresult_error const& ex) {
        MessageBoxW(nullptr, ex.message().c_str(), L"Error", MB_OK | MB_ICONERROR);
        return ex.code().value;
    }
    return 0;
}