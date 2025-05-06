//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

uv_loop_t* loop;
uv_signal_t sigint_handle;

void on_sigint(uv_signal_t* handle, int signum)
{
    printf("\nReceived SIGINT (Ctrl + C), shutting down gracefully...\n");

    // stop the signal handler
    uv_signal_stop(handle);

    // stop the loop
    uv_stop(loop);
    printf("\nHandler completed!\n");
}

int main() {
    loop = uv_default_loop();

    uv_signal_init(loop, &sigint_handle);
    uv_signal_start(&sigint_handle, on_sigint, SIGINT);

    printf("Libuv server starts!\n");
    uv_run(loop, UV_RUN_DEFAULT);

    printf("\nCleaning up resources...\n");
    uv_loop_close(loop);

    return 0;
}