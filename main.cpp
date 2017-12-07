#include "tf_kernel/tf_http.h"
#include <evpp/event_loop.h>
int main() {
    evpp::EventLoop loop;
    //tf_net tnet;
    tf_http thttp;
    //tnet.netloop(loop);
    //evpp::Duration tm(1.0);//循环定时器
    //loop.RunEvery(tm, &keyer);
    loop.Run();
    return 0;
}