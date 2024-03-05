using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Flutter;
using System.Net.Http; 
using Newtonsoft.Json.Linq; 

public sealed partial class MainPage : Page
{
    public MainPage()
    {
        this.InitializeComponent();
    }

    protected override void OnNavigatedTo(NavigationEventArgs e)
    {
        base.OnNavigatedTo(e);
        UpdateManager.RegisterChannel();
    }
}

public static class UpdateManager
{
    // URL of the new version of the application to download and install
    private static string downloadUrl = "https://firebasestorage.googleapis.com/v0/b/rootfi2022.appspot.com/o/Softwares%2Frootfi_integrate.exe?alt=media";

    // Registers a method channel for updating the application
    public static async void RegisterChannel()
    {
        var channel = new MethodChannel("update_manager");
        channel.SetMethodCallHandler(async (call) =>
        {
            switch (call.Method)
            {
                case "checkForUpdate":
                    await CheckForUpdate();
                    break;
                default:
                    break;
            }
        });
    }

    // Checks for an available update by comparing version numbers
    private static async Task CheckForUpdate()
    {
        try
        {
            // URL to fetch the version number of the latest release
            string url = "https://fetcher-in-4rmyxecbaa-el.a.run.app/tally/app_version";
            
            // Using HttpClient to make GET request to fetch the version number
            using (var httpClient = new HttpClient())
            {
                using (var response = await httpClient.GetAsync(url))
                {
                    if (response.IsSuccessStatusCode)
                    {
                        string jsonResponse = await response.Content.ReadAsStringAsync();
                        JObject jsonObject = JObject.Parse(jsonResponse);
                        string versionNumber = jsonObject["version"].ToString();
                        Console.WriteLine("Version Number: " + versionNumber);

                        // Compare version number with the current version
                        if (versionNumber == "your_current_version_here")
                        {
                            Console.WriteLine("Already up to date.");
                        }
                        else
                        {
                            await DownloadAndInstallUpdate();
                        }
                    }
                    else
                    {
                        Console.WriteLine("Failed to retrieve version number. Status code: " + response.StatusCode);
                    }
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine("Error checking for update: " + ex.Message);
        }
    }

    // Downloads and installs the update
    private static async Task DownloadAndInstallUpdate()
    {
        var localFolder = ApplicationData.Current.LocalFolder;
        var downloadedFilePath = Path.Combine(localFolder.Path, "new_app.exe");

        try
        {
            using (var webClient = new WebClient())
            {
                // Download the new EXE file
                await webClient.DownloadFileTaskAsync(new Uri(downloadUrl), downloadedFilePath);

                // Install the downloaded EXE file
                Process.Start(downloadedFilePath);

                // Restart the app
                await Task.Delay(3000); // Delay for 3 seconds
                Application.Current.Exit(); // Exit the app
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine("Error downloading or installing update: " + ex.Message);
        }
    }
}
