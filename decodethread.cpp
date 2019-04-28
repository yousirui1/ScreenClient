#include "decodethread.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
}

#include <QDebug>
#include "queue.h"
#include <time.h>

extern QUEUE stVidsQueue;
extern int run_flag;
unsigned int m_width = 0;
unsigned int m_height = 0;

DecodeThread::DecodeThread()
{

}

void DecodeThread::run()
{
    run_flag = 1;
    AVCodec *pCodec;
    AVCodecContext *pCodecCtx = NULL;
    AVCodecParserContext *pCodecParserCtx = NULL;

    int frame_count;
    AVFrame	*pFrame, *pFrameYUV;
    uint8_t *out_buffer = NULL;
    const int in_buffer_size = 1024 * 1024;
    uint8_t in_buffer[in_buffer_size] = {0};
    uint8_t *cur_ptr;
    int cur_size;

    AVPacket packet;
    int ret, got_picture;

	int first_time = 1;

    AVCodecID codec_id = AV_CODEC_ID_H264;

    struct SwsContext *img_convert_ctx;

    avcodec_register_all();

    pCodec = avcodec_find_decoder(codec_id);
    if (!pCodec) {
        printf("Codec not found\n");
        return;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx){
        printf("Could not allocate video codec context\n");
        return;
    }

    pCodecParserCtx = av_parser_init(codec_id);
    if (!pCodecParserCtx){
        printf("Could not allocate video parser context\n");
        return;
    }

    if (pCodec->capabilities&AV_CODEC_CAP_TRUNCATED)
        pCodecCtx->flags |= AV_CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec\n");
        return;
    }

    pFrame = av_frame_alloc();
    av_init_packet(&packet);


    AVFrame* pFrameBGR = av_frame_alloc(); //
    int size;
    int out_len;
    QUEUE_INDEX *index = NULL;
    while (1)
    {
        if(!run_flag)
        {
            break;
        }

        if(Empty_Queue(&stVidsQueue))
        {
            continue;
        }

        index = De_Queue(&stVidsQueue);

        cur_ptr = index->pBuf;
        cur_size = index->uiSize;
        De_QueuePos(&stVidsQueue);
        while (cur_size>0){
            int len = av_parser_parse2(
                        pCodecParserCtx, pCodecCtx,
                        &packet.data, &packet.size,
                        cur_ptr, cur_size,
                        AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

            cur_ptr += len;
            cur_size -= len;

            if (packet.size == 0)
                continue;
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
            if (ret < 0) {
                printf("Decode Error\n");
                continue ;
            }            
            if (got_picture) {
                if (first_time){
                    printf("\nCodec Full Name:%s\n", pCodecCtx->codec->long_name);
                    printf("width:%d\nheight:%d\n\n", pCodecCtx->width, pCodecCtx->height);
                    //qWarning("pCodecCtx->width %d pCodecCtx->height %d m_width %d m_height %d",pCodecCtx->width, pCodecCtx->height, pCodecCtx->width, pCodecCtx->height);
                    //SwsContext                 
                    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
                    size = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
                    if(out_buffer)
                        av_free(out_buffer);
                    out_buffer = (uint8_t *)av_malloc(size);
                    avpicture_fill((AVPicture *)pFrameBGR, out_buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height); // allocator memory for BGR buffer
                    first_time = 0;
                }
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameBGR->data, pFrameBGR->linesize);
                QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB888);
                emit sigGetFrame(tmpImg);  //发送信号
            }

        }
        packet.data = NULL;
        packet.size = 0;
    }

    sws_freeContext(img_convert_ctx);
    av_parser_close(pCodecParserCtx);

    av_frame_free(&pFrameBGR);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    av_free(pCodecCtx);
    quit();
}
