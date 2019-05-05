#include "decodethread.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
}

#include <QDebug>
#include "queue.h"
#include <time.h>

extern QUEUE stVidsQueue;
extern int run_flag;
DecodeThread::DecodeThread()
{

}

void DecodeThread::run()
{
    run_flag = 1;
    AVCodec *pCodec;
    AVCodecContext *pCodecCtx = NULL;
    AVCodecParserContext *pCodecParserCtx = NULL;

    AVFrame	*pFrame, *pFrameRGB;
    uint8_t *out_buffer[4];

    const int in_buffer_size = 1024 * 1024;
    uint8_t in_buffer[in_buffer_size] = {0};
    uint8_t *cur_ptr;
    int cur_size;

    AVPacket packet;
    int ret;

	int first_time = 1;

    AVCodecID codec_id = AV_CODEC_ID_H264;

    struct SwsContext *img_convert_ctx;

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
    if (!pFrame)
    {
         fprintf(stderr, "Could not allocate video frame\n");
         exit(1);
     }

    av_init_packet(&packet);
    pFrameRGB = av_frame_alloc();

    QUEUE_INDEX *index = NULL;
    //FILE *fp = fopen("c://h264/1.h264", "rb");
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

        //cur_size = fread(in_buffer, 1, sizeof(in_buffer), fp);
        //cur_ptr = in_buffer;
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


            ret = avcodec_send_packet(pCodecCtx, &packet);
            if (ret < 0) {
                continue ;
            }            
            while(ret >= 0)
            {
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                {
                    continue;
                }

                else if(ret < 0)
                {
                    fprintf(stderr, "Error during decoding\n");
                    goto run_out;
                }
                if (first_time)
                {
                    //SwsContext
                    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
                      if ((ret = av_image_alloc(out_buffer, pFrameRGB->linesize,
                                                pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, 16)) < 0) {
                          fprintf(stderr, "Could not allocate source image\n");
                          goto run_out;
                      }
                    first_time = 0;
                }
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, out_buffer, pFrameRGB->linesize);

				QImage tmpImg((uchar *)out_buffer[0], pCodecCtx->width, pCodecCtx->height,QImage::Format_RGB888);
                QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
                emit sigGetFrame(image);  //发送信号

            }
#if 0
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
                    avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height); // allocator memory for BGR buffer
                    first_time = 0;
                }
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB888);
                emit sigGetFrame(tmpImg);  //发送信号
            }
#endif

        }
        packet.data = NULL;
        packet.size = 0;
    }

run_out:
    sws_freeContext(img_convert_ctx);
    av_parser_close(pCodecParserCtx);

    av_frame_free(&pFrameRGB);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    av_free(pCodecCtx);
    quit();
}
