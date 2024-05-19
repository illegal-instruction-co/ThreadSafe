#include "minhook/include/MinHook.h"

#include <iostream>
#include <thread>
#include <functional>
#include <string>
#include <chrono>
#include <vector>

#include <windows.h>

class SafeThread final : public std::thread {
public:
	typedef LRESULT(WINAPI* SleepExType)(DWORD dwMilliseconds, BOOL bAlertable);
	static inline SleepExType SleepExOriginal = nullptr;

	SafeThread(const std::function<void(const std::function<void()>&)>& func) : std::thread([this, func]() {
		_threadIds.push_back(GetCurrentThreadId());

		MH_Initialize();
		MH_CreateHookApi(L"kernelbase.dll", "SleepEx", &SleepExHook, reinterpret_cast<LPVOID*>(&SleepExOriginal));
		MH_EnableHook(MH_ALL_HOOKS);

		func([this]() {CheckRDTSC(); FetchRDTSC(); });

	}) {}

	static LRESULT SleepExHook(DWORD dwMilliseconds, BOOL bAlertable) {
		for (const auto& threadId : _threadIds) {
			HANDLE handle = OpenThread(THREAD_ALL_ACCESS, FALSE, threadId);

			if (!handle)
				return React("Thread is not found!");

			DWORD exitCode = 0;
			GetExitCodeThread(handle, &exitCode);
			CloseHandle(handle);

			if (exitCode != 259)
				return React("Thread is terminated!");
		}

		return SleepExOriginal(dwMilliseconds, bAlertable);
	}
private:
	static constexpr __int64 _maxDiff = 5000000000;
	__int64 _rdtsc = 0;
	static inline std::vector<DWORD> _threadIds;

	__forceinline void FetchRDTSC() {
		_rdtsc = __rdtsc();
	}

	__forceinline void CheckRDTSC() {
		std::cout << "Current: " << __rdtsc() << " | Previous: " << _rdtsc << std::endl;

		if (_rdtsc && __rdtsc() > _rdtsc + _maxDiff)
			React("Thread is not safe!");
	}

	static __forceinline DWORD React(const std::string& message) {
		std::thread([]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));

				for (long long int i = 0; ++i; (&i)[i] = i)
					*((unsigned int*)0) = 0xDEADBEEF;

		}).detach();

		MessageBoxA(nullptr, message.c_str(), "Error", MB_OK | MB_ICONERROR);

		return 0xFEEDBABE;
	}
};

int main() {

	SafeThread thread([](const std::function<void()>& checks) {
		for (;; std::this_thread::sleep_for(std::chrono::seconds(1))) {
			checks();

			std::cout << "Thread is safe!" << std::endl;
		}
	});

	thread.join();

	for(;; std::this_thread::sleep_for(std::chrono::seconds(1)));

	return getchar();
}
