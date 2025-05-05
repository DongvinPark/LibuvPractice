//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define FILE_PATH "/Users/dongvin99/Documents/LibuvPractice/src/example.txt"

// 파일 작업 컨텍스트 객체
typedef struct
{
    uv_loop_t* loop;
    uv_file file;
    ssize_t offset;
    uv_fs_t close_req;
} file_context_t;

// 파일 리딩 작업 요청 객체
typedef struct
{
    uv_fs_t req;
    char* buffer;
    // 현재 파일 리딩 작업에서 참조해야 하는 '파일 작업 컨텍스트 객체'의 포인터를 저장해 놓는다.
    file_context_t* ctx;
} fs_req_wrap_t;


void on_close(uv_fs_t* req)
{
    // libuv 내부에서 req 와 관련된 다른 자원들을 전부 해제할 수 있게 해준다.
    // 단, req 자체를 할당 해제하는 것은 아니다.
    uv_fs_req_cleanup(req);

    file_context_t* ctx = req->data;
    printf("\nFile closed\n");
    // 파일 작업 컨텍스트 객체를 회수한다.
    free(ctx);
}

// 먼저 선언 해둔다.
void on_read(uv_fs_t* req);

void on_open(uv_fs_t* req)
{
    file_context_t* ctx = req->data;

    if (req->result >= 0)
    {
        ctx->file = (uv_file)req->result;

        // 최초의 파일 리딩 작업 요청 객체를 할당하고, 히딩 작업에 필요한 버퍼도 할당한다.
        fs_req_wrap_t* wrap = malloc(sizeof(fs_req_wrap_t));
        wrap->buffer = malloc(BUFFER_SIZE);
        wrap->ctx = ctx;

        uv_buf_t iov = uv_buf_init(wrap->buffer, BUFFER_SIZE);
        wrap->req.data = wrap;

        uv_fs_read(ctx->loop, &(wrap->req), ctx->file, &iov, 1, ctx->offset, on_read);
    }
    else
    {
        fprintf(stderr, "error opening file: %s\n", uv_strerror((int)req->result));
        // 작업이 중간에 실패했으므로, 파일 작업 컨텍스트 객체 또한 회수해야 한다.
        free(ctx);
    }

    // uv_fs_req_cleanup 함수가 인자로서 받아들이고 있는 req는
    // main 함수에서 만들어낸 open_req 포인터가 맞다.
    // 그러나, uv_fs_req_cleanup 함수는 open_req(== on_open() 함수의 req 인자) 자체를
    // 회수하는 것은 아니다.
    // 파일 리딩과 관련된 모든 콜백 함수들(on_open, on_close, on_read) 의 끝부분에서
    // 현재의 함수 호출에서의 작업을 끝낸 후, 항상 uv_fs_req_cleanup()을 호출하고 있는 것에 주목해야 한다.
    uv_fs_req_cleanup(req);
}

void on_read(uv_fs_t* req)
{
    fs_req_wrap_t* wrap = (fs_req_wrap_t*)req->data;
    file_context_t* ctx = wrap->ctx;

    if (req->result < 0)
    {
        fprintf(stderr, "Read error: %s\n", uv_strerror((int)req->result));
        uv_fs_close(ctx->loop, &ctx->close_req, ctx->file, on_close);
        ctx->close_req.data = ctx;
    }
    else if (req->result == 0)
    {
        uv_fs_close(ctx->loop, &ctx->close_req, ctx->file, on_close);
        ctx->close_req.data = ctx;
    }
    else
    {
        // 파일 리딩 작업을 통해서 읽어들인 내용물을 화면에 표시하기 위한 코드다. stdout 이 들어 있는 것을 보면 알 수 있다.
        fwrite(wrap->buffer, 1, req->result, stdout);

        // 어디까지 읽었는지를 기록한다.
        ctx->offset += req->result;

        // 다음 파일 리딩 작업 요청 객체를 할당한 후, 재귀적으로 uv_fs_read()를 호출한다.
        fs_req_wrap_t* next_wrap = malloc(sizeof(fs_req_wrap_t));
        next_wrap->buffer = malloc(BUFFER_SIZE);
        next_wrap->ctx = ctx;

        uv_buf_t iov = uv_buf_init(next_wrap->buffer, BUFFER_SIZE);
        next_wrap->req.data = next_wrap;

        uv_fs_read(ctx->loop, &next_wrap->req, ctx->file, &iov, 1, ctx->offset, on_read);
    }

    // 이번 리딩 작업 요청에 사용된 버퍼를 회수한다.
    free(wrap->buffer);
    // 현재의 함수 호출의 작업들을 처리한 후, req 요청과 관련하여 libuv 내부에서 사용한 리소스들을 회수한다.
    uv_fs_req_cleanup(req);
    // 이번 요청에서 사용한 wrap 객체 즉, 파일 리딩 작업 요청 객체를 완전히 회수한다.
    free(wrap);
}

/*

05-async-file-read
브랜치에서는 uv_fs_read() 와 같이 libuv에서 지원하는 파일 리딩 api를 사용하지 않고, POSIX 파일 리딩 api를 직접 사용했다.

반면, 이번 구현에서는 uv_fs_read() 함수를 적극 활용했다.
libuv에서 파일 리딩 작업은 항상 worker thread pool에서 처리하기 때문에,
05-async-file-read 브랜치에서 구현한 것과 같이 명시적으로 uv_queue_work() 를 통해서 worker threa pool에게
직접 태스크들을 전달해줄 필요가 없다.

둘 중 누가 더 성능이 좋은지는 실제 테스트를 해봐야 알 수 있을 듯 하다.
 */
int main() {
    // 파일 작업 컨텍스트 객체를 할당한다.
    // 해당 객체는 파일 작업 컨텍스트 객체는 파일 리딩 작업이 완료(성공이든, 에러든) 될 때까지 살아 있는다.
    // 즉, 모든 리딩 요청들이 공유하는 객체다.
    file_context_t* ctx = malloc(sizeof(file_context_t));
    ctx->loop = uv_default_loop();
    ctx->offset = 0;

    // open req를 할당한다.
    uv_fs_t* open_req = malloc(sizeof(uv_fs_t));
    open_req->data = ctx;

    uv_fs_open(ctx->loop, open_req, FILE_PATH, O_RDONLY, 0, on_open);

    printf("!!! not blocked work 1 !!!\n");
    printf("!!! not blocked work 2 !!!\n");

    uv_run(ctx->loop, UV_RUN_DEFAULT);

    // 모든 파일 리딩 작업이 끝난 후, open req를 회수한다.
    free(open_req); // clean up open_req after run ends
    return 0;
}