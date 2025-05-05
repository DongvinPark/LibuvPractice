//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <glib.h>

#define BUFFER_SIZE 5
#define FILE_PATH "/home/dongvin/Documents/LibuvPractice/src/example.txt"

typedef struct
{
    uv_fs_t open_req;
    uv_fs_t read_req;
    uv_buf_t buffer;
    uv_loop_t* loop;
    int fd;
} file_context_t;

// 'work callback' and 'after work callback' for libuv's thread pool
void work_cb(uv_work_t* req)
{
    file_context_t* ctx = (file_context_t*)req->data;
    ssize_t result = uv_fs_read(ctx->loop, &(ctx->read_req), ctx->fd, &(ctx->buffer), 1, -1, NULL);
    if (result < 0)
    {
        fprintf(stderr, "Read error: %s\n", uv_strerror((int)result));
    }
}

void after_work_cb(uv_work_t* req, int status) {
    file_context_t *ctx = (file_context_t *)req->data;

    if (ctx->read_req.result < 0) {
        fprintf(stderr, "Async read failed: %s\n", uv_strerror(ctx->read_req.result));
        uv_fs_close(ctx->loop, &ctx->open_req, ctx->fd, NULL);
        free(ctx->buffer.base);
        free(ctx);
        free(req);
        printf("!!! buffer, ctx, req freed with error !!!\n");
        return;
    }

    if (ctx->read_req.result == 0) {
        // EOF reached, cleanup
        uv_fs_close(ctx->loop, &ctx->open_req, ctx->fd, NULL);
        free(ctx->buffer.base);
        free(ctx);
        free(req);
        printf("!!! buffer, ctx, req freed with EOF !!!\n");
        return;
    }

    // Ensure buffer is null-terminated before using strtok
    ctx->buffer.base[ctx->read_req.result] = '\0';

    // Print lines from buffer
    char *line = strtok(ctx->buffer.base, "\n");
    while (line != NULL) {
        printf("line: %s\n", line);
        line = strtok(NULL, "\n");
    }

    // Queue the next read
    printf("\t!!! new work req allocated !!!\n");
    uv_work_t *new_req = malloc(sizeof(uv_work_t)); // Memory leak???
    new_req->data = ctx;
    uv_queue_work(ctx->loop, new_req, work_cb, after_work_cb);

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
    printf("\t!!! work req allocated !!!\n");
    uv_work_t* work_req = (uv_work_t*)malloc(sizeof(uv_work_t)); // Memory leak???
    work_req->data = ctx;
    uv_queue_work(ctx->loop, work_req, work_cb, after_work_cb);
}

/*
텍스트 파일을 비동기적으로 열고, 내용물을 한 줄 한 줄 프린트하는 예제이다.
이 동작은 메인 함수를 실행하는 스레드를 블록킹하지 않는다.

기존의 fread 류의 함수들은 해당 동작을 하는 동안에 main.c 를 실행하는 스레드가 블록킹 된다.
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