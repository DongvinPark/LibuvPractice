//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <uv.h>
#include <glib.h>
#include <unistd.h> // to use 'read()' function
#include <errno.h> // to use 'errno'

#define BUFFER_SIZE 5

// for ubuntu linux
//#define FILE_PATH "/home/dongvin/Documents/LibuvPractice/src/example.txt"

// for macOS
#define FILE_PATH "/Users/dongvin99/Documents/LibuvPractice/src/example.txt"

// for window

typedef struct
{
    uv_fs_t open_req;
    uv_fs_t read_req;
    uv_buf_t buffer;
    uv_loop_t* loop;
    int fd;
    ssize_t read_bytes;
} file_context_t;

// 'work callback' and 'after work callback' for libuv's thread pool
// 할당된 버퍼를 이용해서 파일의 내용물을 읽어들이는 작업은 여기에서 진행된다.
void work_cb(uv_work_t* req)
{
    file_context_t* ctx = (file_context_t*)req->data;
    ctx->read_bytes = read(ctx->fd, ctx->buffer.base, ctx->buffer.len);

    if (ctx->read_bytes < 0)
    {
        int err = errno;
        fprintf(stderr, "Read error: %s\n", uv_strerror(err));
    }
}

// 파일 리딩 후, 후 처리 작업은 여기에서 진행된다.
// 총 세 가지 경우의 수가 존재한다. Error / EOF / Continue Reading 이다.
void after_work_cb(uv_work_t* req, int status)
{
    file_context_t *ctx = (file_context_t *)req->data;

    if (ctx->read_bytes < 0)
    {   // Error
        fprintf(stderr, "Async read failed: %s\n", uv_strerror((int)ctx->read_req.result));
        uv_fs_close(ctx->loop, &ctx->open_req, ctx->fd, NULL);
        free(ctx->buffer.base);
        free(ctx);
        free(req);
        printf("!!! buffer, ctx, req freed with error !!!\n");
        return;
    }
    else if (ctx->read_bytes == 0)
    {
        // EOF. cleanup
        uv_fs_close(ctx->loop, &ctx->open_req, ctx->fd, NULL);
        free(ctx->buffer.base);
        free(ctx);
        free(req);
        printf("!!! buffer, ctx, req freed with EOF !!!\n");
        return;
    }
    else
    {   // print elements in buffer and enqueue next async nonblocking reading task.

        // Ensure buffer is null-terminated before using strtok
        ctx->buffer.base[ctx->read_bytes] = '\0';

        // Print lines from buffer
        char *line = strtok(ctx->buffer.base, "\n");
        while (line != NULL) {
            printf("line: %s\n", line);
            line = strtok(NULL, "\n");
        }

        // Queue the next read
        printf("\t!!! new work req allocated !!!\n");
        uv_work_t *new_req = malloc(sizeof(uv_work_t));
        new_req->data = ctx;
        uv_queue_work(ctx->loop, new_req, work_cb, after_work_cb);
    }

    // 위의 경우의 수와 상관없이, 현재 after_work_cb 호출의 파라미터로 전달된 req는 free 해야 한다.
    uv_fs_req_cleanup(&ctx->read_req);
    free(req); // Always free the current request
    printf("\t!!! current req freed !!!\n");
}

void on_open(uv_fs_t* req)
{
    printf("!!! on_open called !!!\n");
    if (req->result < 0)
    {
        fprintf(stderr, "Error opening file: %s\n", uv_strerror((int)req->result));
        return;
    }

    file_context_t* ctx = (file_context_t*)req->data;
    ctx->fd = (int)req->result;

    // allocate buffer
    printf("!!! buffer allocated !!!\n");
    ctx->buffer = uv_buf_init( (char*)malloc(BUFFER_SIZE + 1), BUFFER_SIZE );

    // start first async read via thread pool
    // 여기서 malloc 된 work_req는 work_cb와 after_work_cb에 전달되고,
    // after_wort_cb에서 최종적으로 free 된다.
    printf("\t!!! work req allocated !!!\n");
    uv_work_t* work_req = (uv_work_t*)malloc(sizeof(uv_work_t));
    work_req->data = ctx;
    uv_queue_work(ctx->loop, work_req, work_cb, after_work_cb);
}

/*
텍스트 파일을 비동기적으로 열고, 내용물을 한 줄 한 줄 프린트하는 예제이다.
이 동작은 메인 함수를 실행하는 스레드를 블록킹하지 않는다.

기존의 fread 류의 함수들은 해당 동작을 하는 동안에 main.c 를 실행하는 스레드가 블록킹 된다.

실행 로그를 보면,
void on_open(uv_fs_t* req){...}가 작업을 모두 마치기도 전에,

    printf("!!! non blocking work 1 !!!\n");
    printf("!!! non blocking work 2 !!!\n");

가 먼저 출력되는 것을 확인할 수 있다.

work_cb 함수는 워커 스레드 풀 내에서 실행되고,
work_cb의 실행이 끝나면 해당 work_cb와 함께 enqueue 됐던 after_work_cb가
main event loop에서 실행된다.

정리하면,
work_cb → runs in the worker thread pool (blocking work allowed)
after_work_cb → runs in the main event loop thread (non-blocking only)

이것이 가능한 이유는, libuv가 기본적으로 두 가지 종류의 executor가 존재하기 때문이다.
1. worker thread pool
2. main event loop

 */
int main() {
    uv_loop_t* loop = uv_default_loop();

    printf("!!! file_context_t allocated !!!\n");
    file_context_t* ctx = malloc(sizeof(file_context_t));
    ctx->loop = loop;
    ctx->open_req.data = ctx;

    // 파일을 연다.
    uv_fs_open(loop, &(ctx->open_req), FILE_PATH, O_RDONLY, 0, on_open);
    printf("!!! non blocking work 1 !!!\n");
    printf("!!! non blocking work 2 !!!\n");

    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return 0;
}