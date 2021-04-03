#include <lib/acceptor.h>
#include "lib/http_server.h"
#include "lib/common.h"
#include "lib/event_loop.h"
#include "lib/sql.h"
#include "lib/callback.h"

int main(int c, char **v) {
    //主线程event_loop
    struct event_loop *eventLoop = event_loop_init();
    //初始化sql
    struct sqlData* sqlData = sql_data_init();
    //初始tcp_server，可以指定线程数目，如果线程是0，就是在这个线程里acceptor+i/o；如果是1，有一个I/O线程
    //tcp_server自己带一个event_loop
    struct http_server *httpServer = http_server_new(eventLoop, SERV_PORT, 
                                                     onParseData,
                                                     onGetSqlData,
                                                     onFormatData,
                                                     sqlData, 2);
    http_server_start(httpServer);

    // main thread for acceptor
    event_loop_run(eventLoop);
}
