#include <Windows.h>
#include <msclr/marshal_cppstd.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <cstdlib>
#include <fstream>
#include <regex>
#include <string>
#include <thread>

#include "../includes/json/json.hpp"
#include "../includes/lazy_importer/lazy_importer.hpp"
#include "Injector.hpp"
#include "R3nzUI.hpp"
#include "Utils.hpp"

using json = nlohmann::json;

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Threading;
using namespace System::Globalization;
using namespace System::Net;

proclist_t WINAPI Injector::findProcesses(const std::wstring &name) noexcept {
  auto process_snap{LI_FN(CreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0)};
  proclist_t list;

  if (process_snap == INVALID_HANDLE_VALUE) {
    return list;
  }

  PROCESSENTRY32W pe32{};
  pe32.dwSize = sizeof(PROCESSENTRY32W);

  if (LI_FN(Process32FirstW).get()(process_snap, &pe32)) {
    if (pe32.szExeFile == name) {
      list.push_back(pe32.th32ProcessID);
    }

    while (LI_FN(Process32NextW).get()(process_snap, &pe32)) {
      if (pe32.szExeFile == name) {
        list.push_back(pe32.th32ProcessID);
      }
    }
  }

  LI_FN(CloseHandle)(process_snap);
  return list;
}

bool WINAPI Injector::isInjected(const std::uint32_t pid) noexcept {
  auto hProcess{LI_FN(OpenProcess)(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid)};

  if (nullptr == hProcess) {
    return false;
  }

  HMODULE hMods[1024];
  DWORD cbNeeded{};

  if (LI_FN(K32EnumProcessModules)(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
    for (auto i{0u}; i < (cbNeeded / sizeof(HMODULE)); ++i) {
      TCHAR szModName[MAX_PATH];
      if (LI_FN(K32GetModuleBaseNameW)(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
        if (std::wcscmp(szModName, L"R3nzSkin.dll") == 0) {
          LI_FN(CloseHandle)(hProcess);
          return true;
        }
      }
    }
  }
  LI_FN(CloseHandle)(hProcess);
  return false;
}

bool WINAPI Injector::inject(const std::uint32_t pid) noexcept {
  TCHAR current_dir[MAX_PATH];
  LI_FN(GetCurrentDirectoryW)(MAX_PATH, current_dir);
  const auto handle{LI_FN(OpenProcess)(PROCESS_ALL_ACCESS, false, pid)};

  if (!handle || handle == INVALID_HANDLE_VALUE) {
    return false;
  }

  FILETIME ft{};
  SYSTEMTIME st{};
  LI_FN(GetSystemTime)(&st);
  LI_FN(SystemTimeToFileTime)(&st, &ft);
  FILETIME create{}, exit{}, kernel{}, user{};
  LI_FN(GetProcessTimes)(handle, &create, &exit, &kernel, &user);

  const auto delta{
    10 -
    static_cast<std::int32_t>(
      (*reinterpret_cast<std::uint64_t *>(&ft) - *reinterpret_cast<std::uint64_t *>(&create.dwLowDateTime)) / 10000000U
    )};

  if (delta > 0) {
    std::this_thread::sleep_for(std::chrono::seconds(delta));
  }

  const auto dll_path{std::wstring(current_dir) + L"\\R3nzSkin.dll"};

  if (const auto f{std::ifstream(dll_path)}; !f.is_open()) {
    LI_FN(MessageBoxW)
    (nullptr, L"R3nzSkin.dll file could not be found.\nTry reinstalling the cheat.", L"R3nzSkin", MB_ICONERROR | MB_OK);
    LI_FN(CloseHandle)(handle);
    return false;
  }

  const auto dll_path_remote{
    LI_FN(VirtualAllocEx)
      .get()(handle, nullptr, (dll_path.size() + 1) * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)};

  if (!dll_path_remote) {
    LI_FN(CloseHandle)(handle);
    return false;
  }

  if (!LI_FN(WriteProcessMemory)
         .get()(handle, dll_path_remote, dll_path.data(), (dll_path.size() + 1) * sizeof(wchar_t), nullptr)) {
    LI_FN(VirtualFreeEx).get()(handle, dll_path_remote, 0u, MEM_RELEASE);
    LI_FN(CloseHandle)(handle);
    return false;
  }

  HANDLE thread{};
  LI_FN(NtCreateThreadEx)
    .nt_cached(
    )(&thread, GENERIC_ALL, nullptr, handle,
      reinterpret_cast<LPTHREAD_START_ROUTINE>(
        LI_FN(GetProcAddress).get()(LI_FN(GetModuleHandleW).get()(L"kernel32.dll"), "LoadLibraryW")
      ),
      dll_path_remote, FALSE, NULL, NULL, NULL, NULL);

  if (!thread || thread == INVALID_HANDLE_VALUE) {
    LI_FN(VirtualFreeEx).get()(handle, dll_path_remote, 0u, MEM_RELEASE);
    LI_FN(CloseHandle)(handle);
    return false;
  }

  LI_FN(WaitForSingleObject)(thread, INFINITE);
  LI_FN(CloseHandle)(thread);
  LI_FN(VirtualFreeEx).get()(handle, dll_path_remote, 0u, MEM_RELEASE);
  LI_FN(CloseHandle)(handle);
  return true;
}

void WINAPI Injector::enableDebugPrivilege() noexcept {
  HANDLE token{};
  if (OpenProcessToken(LI_FN(GetCurrentProcess).get()(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
    LUID value;
    if (LookupPrivilegeValueW(nullptr, SE_DEBUG_NAME, &value)) {
      TOKEN_PRIVILEGES tp{};
      tp.PrivilegeCount           = 1;
      tp.Privileges[0].Luid       = value;
      tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
      if (AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), nullptr, nullptr)) {
        LI_FN(CloseHandle)(token);
      }
    }
  }
}

void Injector::autoUpdate() noexcept {
  WebClient ^ client = gcnew WebClient();
  client->Headers->Add(
    L"User-Agent", L"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/115.0"
  );

  try {
    System::String ^ display_message = L"New version is available on GitHub\nWould you like to download it now?";

    bool dll_missing = !System::IO::File::Exists(L"R3nzSkin.dll");
    if (dll_missing) {
      display_message = L"You are missing important files.\nWould you like to attempt to re-download them?";
    }

    std::string jsonStr = msclr::interop::marshal_as<std::string>(
      client->DownloadString(L"https://api.github.com/repos/NiceAesth/R3nzSkin/releases/latest")
    );

    json jsonData         = json::parse(jsonStr);
    std::string tagname   = jsonData["tag_name"];
    std::string createdAt = jsonData["created_at"];
    std::regex tagnameRegex("v([\\d.]+)");
    std::smatch versionMatch;
    std::string version;

    if (std::regex_search(tagname, versionMatch, tagnameRegex)) {
      version = versionMatch[1];
    }

    System::String ^ newVersion = gcnew System::String(version.c_str());

    DateTime date_of_github_release =
      DateTime::ParseExact(gcnew String(createdAt.c_str()), L"yyyy-MM-dd'T'HH:mm:ss'Z'", CultureInfo::InvariantCulture);
    DateTime date_of_current_release = System::IO::File::GetLastWriteTime(L"R3nzSkin.dll");

    if (date_of_current_release >= date_of_github_release) {
      return;
    }

    std::string buildProfile;
#ifdef _RIOT
#ifdef __AVX2__
    buildProfile = "R3nzSkin-Riot-AVX2";
#else
    buildProfile = "R3nzSkin-Riot-SSE2";
#endif
#else
#ifdef __AVX2__
    buildProfile = "R3nzSkin-China-AVX2";
#else
    buildProfile = "R3nzSkin-China-SSE2";
#endif
#endif

    auto result = MessageBox::Show(
      display_message, gcnew String(buildProfile.c_str()), MessageBoxButtons::YesNo, MessageBoxIcon::Information
    );

    if (result == DialogResult::No) {
      if (dll_missing) {
        System::Environment::Exit(0);
      }
      return;
    }

    std::string downloadUrl;
    std::string file;

    for (const auto &asset : jsonData["assets"]) {
      std::string name = asset["name"];
      if (name.find(buildProfile) != std::string::npos) {
        downloadUrl = asset["browser_download_url"];
        file        = buildProfile + ".zip";
        break;
      }
    }

    if (downloadUrl.empty()) {
      MessageBox::Show(
        L"Download URL not found for the specified build profile.", L"R3nzSkin", MessageBoxButtons::OK,
        MessageBoxIcon::Error
      );
      return;
    }

    auto url     = gcnew String(downloadUrl.c_str());
    auto fileStr = gcnew String(file.c_str());
    client->DownloadFile(url, fileStr);

    System::IO::Compression::ZipFile::ExtractToDirectory(fileStr, L"R3nzSkin");
    System::IO::File::Delete(fileStr);
    if (!dll_missing) {
      System::IO::File::Delete(L"R3nzSkin.dll");
    }
    System::IO::File::Move(
      L"R3nzSkin\\R3nzSkin_Injector.exe", String::Format(L"R3nzSkin_Injector_{0}.exe", newVersion)
    );
    System::IO::File::Move(L"R3nzSkin\\R3nzSkin.dll", L"R3nzSkin.dll");
    System::IO::Directory::Delete(L"R3nzSkin");

    auto process_info       = gcnew System::Diagnostics::ProcessStartInfo();
    process_info->Arguments = L"/C choice /C Y /N /D Y /T 1 & del \"" +
                              System::Diagnostics::Process::GetCurrentProcess()->MainModule->FileName + L"\"";
    process_info->CreateNoWindow = true;
    process_info->FileName       = L"cmd.exe";
    process_info->WindowStyle    = System::Diagnostics::ProcessWindowStyle::Hidden;
    System::Diagnostics::Process::Start(process_info);
    System::Diagnostics::Process::Start(String::Format(L"R3nzSkin_Injector_{0}.exe", newVersion));

    Environment::Exit(0);
  } catch (Exception ^ e) {
    MessageBox::Show(e->Message, L"R3nzSkin", MessageBoxButtons::OK, MessageBoxIcon::Error);
  }
}

void Injector::renameExe() noexcept {
  char szExeFileName[MAX_PATH];
  LI_FN(GetModuleFileNameA)(nullptr, szExeFileName, MAX_PATH);

  const auto path{std::string(szExeFileName)};
  const auto exe{path.substr(path.find_last_of("\\") + 1, path.size())};
  const auto newName{Utils::randomString(std::rand() % (10 - 7 + 1) + 7) + ".exe"};

  std::rename(exe.c_str(), newName.c_str());
}

void Injector::run() noexcept {
  enableDebugPrivilege();
  while (true) {
    const auto &league_client_processes{Injector::findProcesses(L"LeagueClient.exe")};
    const auto &league_processes{Injector::findProcesses(L"League of Legends.exe")};

    R3nzSkinInjector::gameState   = (league_processes.size() > 0) ? true : false;
    R3nzSkinInjector::clientState = (league_client_processes.size() > 0) ? true : false;

    // antiviruses don't like endless loops, show them that this loop is a breaking point. (technically still an
    // infinite loop :D)
    if (league_processes.size() > 0xff) {
      break;
    }

    for (auto &pid : league_processes) {
      if (!Injector::isInjected(pid)) {
        R3nzSkinInjector::cheatState = false;
        if (R3nzSkinInjector::btnState) {
          std::this_thread::sleep_for(1.5s);
          if (Injector::inject(pid)) {
            R3nzSkinInjector::cheatState = true;
          } else {
            R3nzSkinInjector::cheatState = false;
          }
        }
        std::this_thread::sleep_for(1s);
      } else {
        R3nzSkinInjector::cheatState = true;
      }
    }
    std::this_thread::sleep_for(1s);
  }
}
