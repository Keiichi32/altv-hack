#include "Hacks.h"
#include "Utils.h"

typedef alt::IResource* (*_LoadResource)(__int64, unsigned __int64*);
typedef alt::IPackage::File* (*_OpenFile)(alt::IPackage*, alt::StringView);
typedef __int64(*_NewString)(__int64*, __int64, const char*, int, int);
typedef __int64(*_SocialId)(__int64);
typedef __int64(*_MachineGuid)();
typedef __int64(*_ProductId)();
typedef void* (*_LicenseHash)(__int64, void*);
typedef __int64(*_Connect)(__int64, __int64);

_LoadResource LoadResourcePtr = nullptr;
_OpenFile OpenFilePtr = nullptr;
_NewString NewStringPtr = nullptr;
_SocialId SocialIdPtr = nullptr;
_MachineGuid MachineGuidPtr = nullptr;
_ProductId ProductIdPtr = nullptr;
_LicenseHash LicenseHashPtr = nullptr;
_Connect ConnectPtr = nullptr;

std::vector<alt::IResource*> resources;
std::vector<std::string> paths;
std::vector<std::string> removes;

std::string licenseHash;
std::string execstr;
bool active = true;
__int64 serial = 0;

alt::IResource* DetourLoadResource(__int64 a1, unsigned __int64* a2)
{
    alt::IResource* resource = LoadResourcePtr(a1, a2);
    resources.push_back(resource);

    return resource;
}

alt::IPackage::File* DetourOpenFile(alt::IPackage* pkg, alt::StringView path)
{
    if (std::find(paths.begin(), paths.end(), path.ToString()) == paths.end())
        paths.push_back(path.ToString());

    return OpenFilePtr(pkg, path);
}

__int64 DetourNewString(__int64* a1, __int64 a2, const char* a3, int a4, int a5)
{
    std::string tmpstr = a3;

    for (auto rmstr = removes.begin(); rmstr != removes.end(); ++rmstr)
        if (tmpstr.find(*rmstr))
        {
            size_t start_pos = 0;
            while ((start_pos = tmpstr.find(*rmstr, start_pos)) != std::string::npos) {
                tmpstr.replace(start_pos, rmstr->length(), "");
                start_pos += 1;
            }
        }

    if (active)
    {
        active = false;
        execstr += tmpstr;
        return NewStringPtr(a1, a2, execstr.c_str(), a4, -1);
    }

    return NewStringPtr(a1, a2, tmpstr.c_str(), a4, -1);
}

__int64 DetourSocialId(__int64 a1) { return SocialIdPtr(a1) + serial; }
__int64 DetourMachineGuid() { return MachineGuidPtr() + serial; }
__int64 DetourProductId() { return ProductIdPtr() + serial; }

void* DetourLicenseHash(__int64 a1, void* a2)
{
    void* result = LicenseHashPtr(a1, a2);

    if (serial)
        strcpy_s(*(char**)a2, 65, licenseHash.c_str());

    return result;
}

__int64 DetourConnect(__int64 a1, __int64 a2)
{
    strcpy_s((char*)(a2 + 0xF0), 5, "2.23");

    return ConnectPtr(a1, a2);
}

void Hacks::Initialize(LPVOID LoadResource, LPVOID OpenFile, LPVOID NewString, LPVOID SocialId, LPVOID MachineGuid, LPVOID ProductId, LPVOID LicenseHash, LPVOID Connect)
{
    MH_Initialize();

    MH_CreateHook(LoadResource, DetourLoadResource, (LPVOID*)&LoadResourcePtr);
    MH_CreateHook(OpenFile, DetourOpenFile, (LPVOID*)&OpenFilePtr);
    MH_CreateHook(NewString, DetourNewString, (LPVOID*)&NewStringPtr);
    MH_CreateHook(SocialId, DetourSocialId, (LPVOID*)(&SocialIdPtr));
    MH_CreateHook(MachineGuid, DetourMachineGuid, (LPVOID*)(&MachineGuidPtr));
    MH_CreateHook(ProductId, DetourProductId, (LPVOID*)(&ProductIdPtr));
    MH_CreateHook(LicenseHash, DetourLicenseHash, (LPVOID*)(&LicenseHashPtr));
    MH_CreateHook(Connect, DetourConnect, (LPVOID*)(&ConnectPtr));

    MH_EnableHook(LoadResource);
    MH_EnableHook(OpenFile);
    MH_EnableHook(NewString);
    MH_EnableHook(SocialId);
    MH_EnableHook(MachineGuid);
    MH_EnableHook(ProductId);
    MH_EnableHook(LicenseHash);
    MH_EnableHook(Connect);

    CreateDirectoryA("C:\\altv-hack", NULL);
}

void Hacks::ChangeSerial(const std::string& value)
{
    strcpy_s((char*)((uintptr_t)GetModuleHandleA(NULL) + 0x2BE24DF), 16, Utils::RandomString(15).c_str());
    licenseHash = Utils::RandomString(64);

    serial = std::stoi(value);
    std::cout << "Successfully changed the serial!" << std::endl;
}

void Hacks::LoadFile(const std::string& name)
{
    std::ifstream file("C:/altv-hack/" + name);

    if (file)
    {
        std::getline(file, execstr, '\0');
        std::cout << "Successfully loaded file!" << std::endl;
    }
    else
        std::cout << "Couldn't open file!" << std::endl;

    file.close();
}

void Hacks::Remove(const std::string& name)
{
    removes.push_back(name);
    std::cout << "Successfully added remove string!" << std::endl;
}

void Hacks::BlockWeaponCheck()
{
    memset(Utils::FindPattern("GTA5.exe", "\x89\x8b\x00\x00\x00\x00\x41\x8a\x86", "xx????xxx"), 0x90, 6);
    std::cout << "Successfully blocked weapon check!" << std::endl;
}

void Hacks::Dump()
{
    auto start = std::chrono::high_resolution_clock::now();

    CreateDirectoryA("C:\\altv-hack\\dumper", NULL);

    for (auto res = resources.begin(); res != resources.end(); ++res)
        for (auto path = paths.begin(); path != paths.end(); ++path)
        {
            alt::IPackage* pkg = (*res)->GetPackage();

            if (!pkg->FileExists(*path))
                continue;

            alt::IPackage::File* file = OpenFilePtr(pkg, *path);

            std::string resName = (*res)->GetName().ToString();
            std::string workPath = *path;
            std::string dir = "C:\\altv-hack\\dumper\\" + resName;

            CreateDirectoryA(dir.c_str(), NULL);

            while (workPath.find("/") != std::string::npos)
            {
                std::size_t pos = workPath.find("/");
                dir += "\\" + workPath.substr(0, pos);
                workPath = workPath.substr(pos + 1);

                CreateDirectoryA(dir.c_str(), NULL);
            }

            alt::String src{ pkg->GetFileSize(file) };
            pkg->ReadFile(file, src.GetData(), src.GetSize());

            std::ofstream script("C:/altv-hack/dumper/" + resName + "/" + *path, std::ios::out | std::ios::binary);
            script.write(src.GetData(), src.GetSize());
            pkg->CloseFile(file);

            std::cout << "Created file: " << resName << "/" << *path << std::endl;
        }

    std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Dumping finished in " << duration.count() * 1000.f << "ms!" << std::endl;
}