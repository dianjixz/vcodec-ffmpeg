/*
 * @Copyright: iflytek
 * @Autor: zghong
 * @Date: 2020-04-02 16:52:55
 * @Description: 测试vcodec类
 * 
 * libx264和libx265第三方库集成在`./include, ./lib`中。
 * 已经测试的编解码算法有：h264, h265
 */

#include <stdio.h>
#include <string>
#include "vcodec.hpp"

using namespace std;
vcodec *adecoder_h265;
void get_pictor(size_t count, void *data, size_t len)
{
    adecoder_h265->yuv2rgb(data);
    printf("get c:%d h:%d   w:%d    len:%d\r\n",count,adecoder_h265->rgb.h, adecoder_h265->rgb.w, adecoder_h265->rgb.len);
    // if(count == 382)
    // {
    //     FILE* file = fopen("rgb.bin","wb");
    //     fwrite(adecoder_h265->rgb.data, 1, adecoder_h265->rgb.len, file);
    //     fclose(file);
    // }
}
int main()
{
    // decoder_h264
    vcodec decoder_h264 = vcodec("../bin/video/input.h264", "./decoder_h264.yuv", "h264");
    decoder_h264.decode();
    // encoder_h264
    vcodec encoder_h264 = vcodec("./decoder_h264.yuv", "./encoder_h264.h264", "libx264");
    encoder_h264.encode();
    // encoder_h265
    vcodec encoder_h265 = vcodec("./decoder_h264.yuv", "./encoder_h265.h265", "libx265");
    encoder_h265.encode();
    // decoder_h265
    vcodec decoder_h265 = vcodec("./encoder_h265.h265", "./decoder_h265.yuv", "hevc", get_pictor);
    adecoder_h265 = &decoder_h265;
    decoder_h265.decode();
    // printf("start--------------------------------\r\n");
    // vcodec decoder_h265 = vcodec("./h265_bin_code", "./decoder_h265_bin_code.yuv", "hevc", get_pictor);
    // adecoder_h265 = &decoder_h265;
    // decoder_h265.decode();
    // printf("over--------------------------------\r\n");
    return 0;
}