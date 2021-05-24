//
//  main.c
//  PSNR
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include "bmp.h"
#include "psnr.h"

int bmp_psnr(int argc, char* argv[])
{
    if (argc < 3) {
        printf("%s\n", argv[0]);
        printf("invalid input parameter!\n\n");
        printf("help: Format example\nPSNR.exe ref.bmp input.bmp [a_calc(0/1) upside_down(0/1)]\n");
        //getchar();
        return -1;
    }
    char* ref_filename = argv[1];
    char* input_filename = argv[2];
    /* RGBA的A算不算 */
    int a_calc = 0;
    if (argc > 3)
        a_calc = atoi(argv[3]);
    /* ref和input是不是相互上下颠倒的 */
    int upside_down = 0;
    if (argc > 4)
        upside_down = atoi(argv[4]);


    int ret = 0;
    float psnr = 0;
    uint8_t* ref_buf = NULL;
    uint8_t* input_buf = NULL;
    BITMAPINFOHEADER ref_info, input_info;
    /* 读取bmp数据 */
    ret = bmp_get(ref_filename, &ref_buf, &ref_info);
    if (ret) {
        printf("cannot open input file:%s\n", ref_filename);
        goto bmp_psnr_end;
    }
    ret = bmp_get(input_filename, &input_buf, &input_info);
    if (ret) {
        printf("cannot open input file:%s\n", input_filename);
        goto bmp_psnr_end;
    }

    /* 检查两张图的输入是否正确 */
    if (ref_info.biWidth != input_info.biWidth || ref_info.biHeight != input_info.biHeight) {
        printf("ref(%dx%d)!=input(%dx%d)\n",
            ref_info.biWidth, ref_info.biHeight, input_info.biWidth, input_info.biHeight);
        ret = -1;
        goto bmp_psnr_end;
    }
    int width = ref_info.biWidth;
    int height = ref_info.biHeight;
    if (ref_info.biBitCount == 24 && input_info.biBitCount == 24 && upside_down == 0) {
        psnr = iqa_psnr(ref_buf, input_buf, width * 3, height, width * 3);
    }
    else {
        /* 确保输入图都转成RGBA */
        if (ref_info.biBitCount == 24) {
            bmp_24to32(&ref_buf, &ref_info);
        }
        if (input_info.biBitCount == 24) {
            bmp_24to32(&input_buf, &input_info);
        }

        if (ref_info.biBitCount != 32 || input_info.biBitCount != 32) {
            printf("biBitCount:ref=%d,input=%d, not support\n", ref_info.biBitCount, input_info.biBitCount);
            ret = -1;
            goto bmp_psnr_end;
        }

        /* 计算psnr */
        psnr = RGBA_psnr(ref_buf, input_buf, width, height, width * 4, a_calc, upside_down);
    }

    /* 输出psnr */
    printf("psnr=%.2f\n", psnr);

bmp_psnr_end:
    if (ref_buf) free(ref_buf);
    if (input_buf) free(input_buf);
    return ret;
}

float psnr(uint8_t* p1, uint8_t* p2, int size);
void mReturn(void);
uint8_t * buf1, *buf2;
FILE* in_file1, *in_file2, *out_file;

int main(int argc, char* argv[])
{
    if (argc > 2 && strstr(argv[1], ".bmp")) {
        /* bmp的psnr对比 */
        return bmp_psnr(argc, argv);
    }

    /* yuv420的psnr对比 */
    int frame_no;
    int size, luma_size, chroma_size;
    int height, width, frame_num;
    float y = 0.0, u = 0.0, v = 0.0, all = 0.0;
    buf1=NULL;
    buf2=NULL;
    if (argc!=7)
    {
        printf("%s\n",argv[0]);
        printf("invalid input parameter!\n\n");
        printf("help: Format example\nPSNR.exe ref.yuv input.yuv output.txt width height frame_num\n");
        //getchar();
        return 0;
    }
    
    width = atoi(argv[4]);
    height = atoi(argv[5]);
    frame_num = atoi(argv[6]);
    luma_size = height * width;
    size = luma_size + (luma_size >> 1);
    chroma_size = luma_size / 4;
    
    buf1 = (uint8_t *)malloc(size);
    buf2 = (uint8_t *)malloc(size);
    
    in_file1 = fopen(argv[1], "rb");
    if (!in_file1)
    {
        printf("cannot open input file %s\n", argv[1]);
        mReturn();
    }
    in_file2 = fopen(argv[2], "rb");
    if (!in_file2)
    {
        printf("cannot open input file  %s\n", argv[2]);
        mReturn();
    }
    out_file = fopen(argv[3], "at");
    if (!out_file)
    {
        printf("cannot open output file  %s\n", argv[3]);
        mReturn();
    }
    
    printf(" == PSNR.exe =====================================\n");
    printf(" reference file : %s\n", argv[1]);
    printf(" input file     : %s\n", argv[2]);
    printf(" output file    : %s\n", argv[3]);
    printf(" width          : %d\n", width);
    printf(" height         : %d\n", height);
    printf(" frame number   : %d\n", frame_num);
    printf(" =================================================\n");
    printf(" == Calculate PSNR ===============================\n");
    for(frame_no = 0; frame_no < frame_num; frame_no++ )
    {
        printf("%d ", frame_no);
        if (fread(buf1, size, 1, in_file1) <= 0)
        {
            printf("cannot read from input file.");
            break;
        }
        if (fread(buf2, size, 1, in_file2) <= 0)
        {
            printf("cannot read from input file.");
            break;
        }
        
        y += psnr(buf1, buf2, luma_size);
        u += psnr(buf1 + luma_size, buf2 + luma_size, chroma_size);
        v += psnr(buf1 + luma_size + chroma_size, buf2 + luma_size + chroma_size, chroma_size);
        all += psnr(buf1, buf2, size);
    }
    
    printf("\n");
    printf(" =================================================\n");
    printf(" PSNR-Y : %.2f \n", y/frame_num);
    printf(" PSNR-U : %.2f \n", u/frame_num);
    printf(" PSNR-V : %.2f \n", v/frame_num);
    printf(" PSNR-ALL:%.2f \n", all/frame_num);
    fprintf(out_file, "%s\t%s\t", argv[1], argv[2]);
    fprintf(out_file, "%.2f\t%.2f\t%.2f\t%.2f\n", y/frame_num, u/frame_num, v/frame_num, all/frame_num);
    printf(" Results are written to %s\n", argv[3]);
    printf(" =================================================\n");
    
    mReturn();

    return 0;
}

void mReturn(void)
{
    if(!in_file1) fclose(in_file1);
    if(!in_file2) fclose(in_file2);
    if(!out_file) fclose(out_file);
    if(!buf1) free(buf1);
    if(!buf2) free(buf2);
}

float psnr(uint8_t* p1, uint8_t* p2, int size)
{
    unsigned long sad = 0;
    int i;
    
    for (i = 0 ; i < size ; i ++)
    {   
        int tmp;   
        tmp = (p1[i] - p2[i]);   
        sad += tmp * tmp;   
    }   
    
    return (float)(10 * log10(65025.0f * size/sad));   
}  
