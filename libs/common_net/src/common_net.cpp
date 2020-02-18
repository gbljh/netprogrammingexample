#include "common_net.h"

bool resolve_addr(char* str_addr, in_addr* baddr)
{
    unsigned long ip = inet_addr(str_addr);
    if (ip > 0) {
        memcpy(baddr, &ip, sizeof(unsigned long));
        return true;
    }
    else {
        hostent* hp = gethostbyname(str_addr);
        if (hp == NULL)
        {
            fprintf(stderr, "Error host %s", str_addr);
            return false;
        }
        memcpy(baddr, hp->h_addr_list, hp->h_length);
        return true;
    }
}

bool parse_cmd(int argc, char* argv[], char* host, short* port)
{
    if (argc < 2) {
        return false;
    }

    char all_args[256];
    memset(all_args, 0, sizeof all_args);

    for (int i = 1; i < argc; ++i) {
        strcat(all_args, argv[i]);
        strcat(all_args, " ");
    }


    const int count_vars = 3;
    const int host_buf_sz = 128;
    int tmp_ports[count_vars] = { -1, -1, -1 };
    char tmp_hosts[count_vars][host_buf_sz];
    for (int i = 0; i < count_vars; ++i) {
        memset(tmp_hosts[i], 0, host_buf_sz);
    }
    const char* formats[count_vars] = { "-h %s -p %d", "-p %d -h %s", "-p %d" };

    int results[] = {
            sscanf(all_args, formats[0], tmp_hosts[0], &tmp_ports[0]) - 2,
            sscanf(all_args, formats[1], &tmp_ports[1], tmp_hosts[1]) - 2,
            sscanf(all_args, formats[2], &tmp_ports[2]) - 1
    };

    for (int i = 0; i < sizeof(results) / sizeof(int); ++i) {
        if (!results[i]) {
            if (strlen(tmp_hosts[i]) > 0) {
                strcpy(host, tmp_hosts[i]);
            }
            if (tmp_ports[i] > 0) {
                *port = (short)tmp_ports[i];
                return true;
            }
        }
    }

    return false;

}

int common_init_handler()
{
#ifdef _WIN32
    WSADATA ws;
	CHECK_IO(WSAStartup(MAKEWORD(2, 2), &ws), -1, "Error init of WinSock2");
	return 0;
#elif LINUX
	return 0;
#else
 #error "Unsupported platform"
#endif
}

void common_exit_handler() {
#ifdef _WIN32
    WSADATA ws;
    CHECK_VOID_IO(WSACleanup(), "Error shutdown of WinSock2");
#elif LINUX
    return;
#else
 #error "Unsupported platform"
#endif
}

int close_socket(int socket) {
#ifdef _WIN32
    CHECK_IO(closesocket(socket), -1, "Error close socket");
    return 0;
#elif LINUX
    return close(socket);
#else
 #error "Unsupported platform"
#endif
}

void error_msg(const char* msg) {
    printf("%s\n", msg);
}